/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/convergence/Lower2Copper.hpp>
#include <GLUE/convergence/Upper.hpp>
#include <GLUE/Component.hpp>

#include <WNS/ldk/fun/FUN.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/module/Base.hpp>

#include <WNS/probe/bus/json/probebus.hpp>
#include <WNS/probe/bus/utils.hpp>

#include <cstdlib>


using namespace glue::convergence;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Lower2Copper,
	wns::ldk::FunctionalUnit,
	"glue.convergence.Lower2Copper",
	wns::ldk::FUNConfigCreator);

Lower2Copper::Lower2Copper(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
	wns::ldk::CommandTypeSpecifier<LowerCommand>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<Lower2Copper>(),

	config(_config),
	logger(_config.get<wns::pyconfig::View>("logger")),
	dataTransmission(NULL),
	notificationService(NULL),
	isBlocking(_config.get<bool>("blocking"))
{
    wns::probe::bus::ContextProviderCollection localContext(
        &fun->getLayer()->getContextProviderCollection());
    jsonTracing = wns::probe::bus::collector(localContext, config, "phyTraceProbeName");
} // Lower2Copper

Lower2Copper::~Lower2Copper()
{
} // ~Lower2Copper

void Lower2Copper::onFUNCreated()
{
	friends.unicastRouting =
		getFUN()->findFriend<glue::convergence::UnicastUpper*>(
			config.get<std::string>("unicastRouting"));

	friends.broadcastRouting =
		getFUN()->findFriend<glue::convergence::BroadcastUpper*>(
			config.get<std::string>("broadcastRouting"));
} // onFUNCreated

bool Lower2Copper::doIsAccepting(const wns::ldk::CompoundPtr& /* compound */) const
{
	if (this->isBlocking == true)
	{
		return getDataTransmissionService()->isFree();
	}
	else
	{
		return true;
	}
} // isAccepting

void Lower2Copper::onCarrierIdle()
{
	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << ": carrier idle, accepting new transmissions";
	MESSAGE_END();

	getReceptor()->wakeup();
}

void Lower2Copper::onCarrierBusy()
{
	// currently, we do nothing.
}

void Lower2Copper::onCollision()
{
	// currently, we do nothing. In future we might stop ongoing transmissions
} // onCollision

void
Lower2Copper::doSendData(const wns::ldk::CompoundPtr& compound)
{
	assure(compound, "sendData called with an invalid compound.");

	if (hasCommandOf(friends.unicastRouting, compound)) {
		UnicastUpperCommand* command = friends.unicastRouting->getCommand(compound->getCommandPool());
        wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();
        LowerCommand* lc = activateCommand(compound->getCommandPool());
        lc->magic.txStartTime = now;

        /* Only the "real" Glue Layer support getStationType, the stubs in the tests do not*/
        wns::ldk::ILayer* layer = getFUN()->getLayer();
        Component* glueLayer = dynamic_cast<Component*>(layer);
        if(glueLayer != NULL)
            lc->magic.senderType = glueLayer->getStationType();

		getDataTransmissionService()->sendData(command->peer.targetMACAddress, compound);
	}
 	else if (hasCommandOf(friends.broadcastRouting, compound)) {
		BroadcastUpperCommand* command = friends.broadcastRouting->getCommand(compound->getCommandPool());
        wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();
        LowerCommand* lc = activateCommand(compound->getCommandPool());
        lc->magic.txStartTime = now;

        /* Only the "real" Glue Layer support getStationType, the stubs in the tests do not*/
        wns::ldk::ILayer* layer = getFUN()->getLayer();
        Component* glueLayer = dynamic_cast<Component*>(layer);
        if(glueLayer != NULL)
            lc->magic.senderType = glueLayer->getStationType();

		getDataTransmissionService()->sendData(command->peer.targetMACAddress, compound);
	}
	else assure(false, "Not a routing Compound!");
} // doSendData

void Lower2Copper::doOnData(const wns::ldk::CompoundPtr& compound)
{
	assure(compound, "onData called with an invalid compound.");

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << ": doOnData(), forwading to higher FU";
	MESSAGE_END();

	getDeliverer()->getAcceptor(compound)->onData(compound);
} // doOnData

void
Lower2Copper::doWakeup()
{
	// This will never be called ...
} // doWakeup

void
Lower2Copper::onData(const wns::osi::PDUPtr& pdu, double ber, bool collision)
{
	assure(wns::dynamicCast<wns::ldk::Compound>(pdu), "not a CompoundPtr");

    // FIRST: create a copy instead of working on the real compound
    wns::ldk::CompoundPtr compound = wns::staticCast<wns::ldk::Compound>(pdu)->copy();

#ifndef NDEBUG
    if(jsonTracing->hasObservers())
        traceIncoming(compound, collision);
#endif

	// In case of a collision -> throw away ... packet can't be decoded
	if (collision)
	{
		return;
	}

	if (hasCommandOf(friends.unicastRouting, compound)) {
		UnicastUpperCommand* uc = friends.unicastRouting->getCommand(compound->getCommandPool());
		if (uc->peer.targetMACAddress == address) {
			pushUp(compound, ber, pdu);
		}
	}
	else if (hasCommandOf(friends.broadcastRouting, compound)) {
		pushUp(compound, ber, pdu);
	}
	// else throw away
	// Data was not for us
	// should not happen with current copper implementation
} // onData

void
Lower2Copper::setDataTransmissionService(wns::service::Service* phy)
{
	assure(phy, "must be non-NULL");
	assureType(phy, wns::service::phy::copper::DataTransmission*);
	dataTransmission = dynamic_cast<wns::service::phy::copper::DataTransmission*>(phy);
} // setDataTransmissionService

wns::service::phy::copper::DataTransmission*
Lower2Copper::getDataTransmissionService() const
{
	assure(dataTransmission, "no copper::DataTransmission set. Did you call setDataTransmission()?");
	return dataTransmission;
} // getDataTransmissionService

void
Lower2Copper::setNotificationService(wns::service::Service* phy)
{
	assure(phy, "must be non-NULL");
	assureType(phy, wns::service::phy::copper::Notification*);
	notificationService = dynamic_cast<wns::service::phy::copper::Notification*>(phy);
	// attach for both, data handling an carrier sensing
	this->wns::Observer<wns::service::phy::copper::Handler>::startObserving(notificationService);
	this->wns::Observer<wns::service::phy::copper::CarrierSensing>::startObserving(notificationService);
	notificationService->setDLLUnicastAddress(address);
} // setNotificationService

wns::service::phy::copper::Notification*
Lower2Copper::getNotificationService() const
{
	assure(notificationService, "no copper::Notification set. Did you call setNotificationService()?");
	return notificationService;
} // getNotificationService

void
Lower2Copper::setMACAddress(const wns::service::dll::UnicastAddress& _address)
{
	address = _address;
	MESSAGE_SINGLE(NORMAL, logger, "setting MAC address of lowerConvergence to: " << address);
} // setMACAddress

void
Lower2Copper::pushUp(const wns::ldk::CompoundPtr& compound, double ber, const wns::osi::PDUPtr& pdu)
{
	LowerCommand* lc = getCommand(compound->getCommandPool());
	lc->local.per = 1.0 - pow(1.0 - ber, pdu->getLengthInBits());
	notifyBERConsumers(ber, pdu->getLengthInBits());
	this->wns::ldk::FunctionalUnit::onData(compound);
} // pushUp

void
Lower2Copper::traceIncoming(wns::ldk::CompoundPtr compound, bool collision)
{
    wns::probe::bus::json::Object objdoc;

    /* Only the "real" Glue Layer support getStationType, the stubs in the tests do not*/
    wns::ldk::ILayer* layer = getFUN()->getLayer();
    Component* myLayer = dynamic_cast<Component*>(layer);
    if(myLayer == NULL)
        return;

    LowerCommand* lc;
    lc = getCommand(compound->getCommandPool());

    UnicastUpperCommand* uc = 
        friends.unicastRouting->getCommand(compound->getCommandPool());

    wns::service::dll::UnicastAddress dstAdr;

    bool isBroadcast;

    if (hasCommandOf(friends.unicastRouting, compound)) 
    {
        dstAdr = uc->peer.targetMACAddress;
        isBroadcast = false;
    }
    else
    {
        isBroadcast = true;
    }

    std::string src;
    std::string dst("Broadcast");
    std::string me;
    std::string sender;

    std::stringstream s;
    std::stringstream m;
    std::stringstream d;
    std::stringstream snd;

    if(myLayer->getStationType() == StationTypes::router())
    {
        m << "BS";
    }
    else
    {
        m << "UT";
    }

    if(lc->magic.senderType == StationTypes::router())
    {
        s << "BS";
        snd << "BS";
        /* For now we assume the destination has the other station type */
        d << "UT";
    }
    else
    {
        s << "UT";
        snd << "UT";
        /* For now we assume the destination has the other station type */
        d << "BS";
    }
  
    s << uc->peer.sourceMACAddress;
    sender = s.str();

    m << address;
    me = m.str();

    /* Glue does not support L2 multihop, sender always is source */
    snd << uc->peer.sourceMACAddress;
    src = snd.str();
    
    if(!isBroadcast)
    {
        d << dstAdr;
        dst = d.str();
    }

    objdoc["Transmission"]["ReceiverID"] = wns::probe::bus::json::String(me);
    objdoc["Transmission"]["SenderID"] = wns::probe::bus::json::String(sender);
    objdoc["Transmission"]["SourceID"] = wns::probe::bus::json::String(src);
    objdoc["Transmission"]["DestinationID"] = wns::probe::bus::json::String(dst);

    wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();

    objdoc["Transmission"]["Start"] =
         wns::probe::bus::json::Number(lc->magic.txStartTime);
        
    objdoc["Transmission"]["Stop"] = wns::probe::bus::json::Number(now);

    // We abuse this to watch per station results
    objdoc["Transmission"]["Subchannel"] = wns::probe::bus::json::Number(
        uc->peer.sourceMACAddress.getInteger());
    
    objdoc["Transmission"]["TxPower"] = 
        wns::probe::bus::json::Number(0.0); // Unknown
    objdoc["Transmission"]["RxPower"] = 
        wns::probe::bus::json::Number(0.0);
    objdoc["Transmission"]["InterferencePower"] = 
        wns::probe::bus::json::Number(collision?200.0:-200.0);

    wns::probe::bus::json::probeJSON(jsonTracing, objdoc);
}


