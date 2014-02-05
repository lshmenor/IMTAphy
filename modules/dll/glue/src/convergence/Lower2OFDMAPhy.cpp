/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <GLUE/convergence/Lower2OFDMAPhy.hpp>
#include <GLUE/convergence/Upper.hpp>

#include <WNS/pyconfig/View.hpp>
#include <boost/bind.hpp>

using namespace glue::convergence;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Lower2OFDMAPhy,
    wns::ldk::FunctionalUnit,
    "glue.convergence.Lower2OFDMAPhy",
    wns::ldk::FUNConfigCreator);

Lower2OFDMAPhy::Lower2OFDMAPhy(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
    wns::ldk::CommandTypeSpecifier<LowerCommand>(fun),
    wns::ldk::HasReceptor<>(),
    wns::ldk::HasConnector<>(),
    wns::ldk::HasDeliverer<>(),
    wns::Cloneable<Lower2OFDMAPhy>(),

    logger_(config.get<wns::pyconfig::View>("logger")),
    transmitting_(false),
    upperUnicastName_(config.get<std::string>("unicastRouting")),
    upperBroadcastName_(config.get<std::string>("broadcastRouting")),
    phyModeMapper_(wns::service::phy::phymode::PhyModeMapperInterface::getPhyModeMapper(
        config.getView("phyModeMapper"))),
    dataTransmission_(NULL),
    notificationService_(NULL)
{
} // Lower2OFDMAPhy

Lower2OFDMAPhy::~Lower2OFDMAPhy()
{
} // ~Lower2OFDMAPhy

void Lower2OFDMAPhy::onFUNCreated()
{
    friends_.unicastRouting =
        getFUN()->findFriend<glue::convergence::UnicastUpper*>(
            upperUnicastName_);

    friends_.broadcastRouting =
        getFUN()->findFriend<glue::convergence::BroadcastUpper*>(
            upperBroadcastName_);
} // onFUNCreated

bool Lower2OFDMAPhy::doIsAccepting(const wns::ldk::CompoundPtr& /* compound */) const
{
        return !transmitting_;
} // isAccepting



void
Lower2OFDMAPhy::doSendData(const wns::ldk::CompoundPtr& compound)
{
    assure(compound, "sendData called with an invalid compound.");
    assure(!transmitting_, "Tried to tranmit while already sending");

    transmitting_ = true;

    wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();
    LowerCommand* lc = activateCommand(compound->getCommandPool());
    lc->magic.txStartTime = now;

    assure(phyModeMapper_->getPhyModeCount() == 1, "There should be only one PhyMode");
    wns::service::phy::phymode::PhyModeInterfacePtr phyMode;
    phyMode = phyModeMapper_->getPhyModeForIndex(0);

    wns::simulator::Time txTime = double(compound->getLengthInBits()) / phyMode->getDataRate();
    assure(txTime > 0, "Transmission time cannot be zero.");

    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&Lower2OFDMAPhy::stopTransmitting, this, compound), txTime);

    dataTransmission_->startBroadcast(compound, 0, wns::Power::from_dBm(10), phyMode);
} // doSendData

void
Lower2OFDMAPhy::stopTransmitting(const wns::ldk::CompoundPtr& compound)
{
    assure(transmitting_, "Called stopTransmitting while still transmitting");

    transmitting_ = false;
    dataTransmission_->stopTransmission(compound, 0);

    getReceptor()->wakeup();
}

void Lower2OFDMAPhy::doOnData(const wns::ldk::CompoundPtr& compound)
{
    assure(compound, "onData called with an invalid compound.");

    MESSAGE_BEGIN(NORMAL, logger_, m, getFUN()->getName());
    m << ": doOnData(), forwading to higher FU";
    MESSAGE_END();

    getDeliverer()->getAcceptor(compound)->onData(compound);
} // doOnData

void
Lower2OFDMAPhy::doWakeup()
{
    // This will never be called ...
} // doWakeup

void
Lower2OFDMAPhy::onData(wns::osi::PDUPtr pdu, wns::service::phy::power::PowerMeasurementPtr rx)
{
    assure(wns::dynamicCast<wns::ldk::Compound>(pdu), "not a CompoundPtr");

    // FIRST: create a copy instead of working on the real compound
    wns::ldk::CompoundPtr compound = wns::staticCast<wns::ldk::Compound>(pdu)->copy();

    double ber = 0.0;

    if (hasCommandOf(friends_.unicastRouting, compound)) 
    {
        UnicastUpperCommand* uc = 
            friends_.unicastRouting->getCommand(compound->getCommandPool());
        if (uc->peer.targetMACAddress == address_) 
        {
            MESSAGE_BEGIN(NORMAL, logger_, m, getFUN()->getName());
            m << ": onData(), got Unicast PDU. SINR is ";
            m << rx->getSINR();
            MESSAGE_END();

            LowerCommand* lc = getCommand(compound->getCommandPool());
            lc->local.per = 1.0 - pow(1.0 - ber, pdu->getLengthInBits());
            this->wns::ldk::FunctionalUnit::onData(compound);
        }
    }
    else if (hasCommandOf(friends_.broadcastRouting, compound)) 
    {
        MESSAGE_BEGIN(NORMAL, logger_, m, getFUN()->getName());
        m << ": onData(), got Broadcast PDU. SINR is ";
        m << rx->getSINR();
        MESSAGE_END();

        LowerCommand* lc = getCommand(compound->getCommandPool());
        lc->local.per = 1.0 - pow(1.0 - ber, pdu->getLengthInBits());
        this->wns::ldk::FunctionalUnit::onData(compound);
    }
} // onData

void
Lower2OFDMAPhy::setDataTransmissionService(wns::service::Service* phy)
{
    assure(phy, "must be non-NULL");
    assureType(phy, wns::service::phy::ofdma::DataTransmission*);
    dataTransmission_ = dynamic_cast<wns::service::phy::ofdma::DataTransmission*>(phy);
} // setDataTransmissionService

wns::service::phy::ofdma::DataTransmission*
Lower2OFDMAPhy::getDataTransmissionService() const
{
    assure(dataTransmission_, "no copper::DataTransmission set. Did you call setDataTransmission()?");
    return dataTransmission_;
} // getDataTransmissionService

void
Lower2OFDMAPhy::setNotificationService(wns::service::Service* phy)
{
    assure(phy, "must be non-NULL");
    assureType(phy, wns::service::phy::ofdma::Notification*);

    notificationService_ = dynamic_cast<wns::service::phy::ofdma::Notification*>(phy);
    notificationService_->registerHandler(this);
} // setNotificationService

wns::service::phy::ofdma::Notification*
Lower2OFDMAPhy::getNotificationService() const
{
    assure(notificationService_, "no copper::Notification set. Did you call setNotificationService()?");
    return notificationService_;
} // getNotificationService

void
Lower2OFDMAPhy::setMACAddress(const wns::service::dll::UnicastAddress& address)
{
    address_ = address;
    MESSAGE_SINGLE(NORMAL, logger_, "setting MAC address of lowerConvergence to: " << address_);
} // setMACAddress
