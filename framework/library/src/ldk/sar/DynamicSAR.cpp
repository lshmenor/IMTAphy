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

#include <WNS/ldk/sar/DynamicSAR.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Layer.hpp>

#include <WNS/search/ISearch.hpp>
#include <WNS/search/SearchCreator.hpp>

#include <WNS/Assure.hpp>

#include <boost/bind.hpp>
#include <algorithm>

using namespace wns::ldk;
using namespace wns::ldk::sar;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    DynamicSAR,
    FunctionalUnit,
    "wns.sar.DynamicSAR",
    FUNConfigCreator);

DynamicSAR::DynamicSAR(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    fu::Plain<DynamicSAR, DynamicSARCommand>(fuNet),
    maxSegmentSize_(config.get<int>("maxSegmentSize")),
    searchAlgo_(wns::search::SearchFactory::creator(config.get<std::string>("searchAlgo"))
                ->create(0, 0, boost::bind(&DynamicSAR::compare, this, _1))),
    currentCompound_(),
    currentCompoundSize_(0),
    currentCompoundSentSize_(0),
    segmentNumber_(1),
    currentSegInfoPtr_(),
    logger_(config.get("logger"))
{
    MESSAGE_BEGIN(NORMAL, logger_, m, "");
    m << "Instantiating FU...\n"
      << "Parameters: max. segment size = " << maxSegmentSize_ << "bit\n"
      << "            algorithm to determine current segment size = "
      << config.get<std::string>("searchAlgo");
    MESSAGE_END();
} // DynamicSAR

DynamicSAR::~DynamicSAR()
{
    delete searchAlgo_;
} // ~DynamicSAR

void
DynamicSAR::doSendData(const CompoundPtr& compound)
{
    currentCompound_ = compound;

    Bit commandPoolSize = 0;
    Bit dataSize = 0;

    CompoundPtr compoundCopy = compound->copy();
    activateCommand(compoundCopy->getCommandPool());
    getFUN()->calculateSizes(compoundCopy->getCommandPool(), commandPoolSize, dataSize, this);

    currentCompoundSize_ = commandPoolSize + dataSize;

    MESSAGE_BEGIN(NORMAL, logger_, m, "");
    m << "Segmenting new compound: compound size = " << currentCompoundSize_ << "bit";
    MESSAGE_END();

    currentCompoundSentSize_ = 0;

    segmentNumber_ = 1;
    currentSegInfoPtr_ = DynamicSARCommand::SegmentationInfoPtr(new DynamicSARCommand::SegmentationInfo());

    sendSegments();
} // doSendData

void
DynamicSAR::doOnData(const CompoundPtr& compound)
{
    DynamicSARCommand* command = getCommand(compound->getCommandPool());

    MESSAGE_BEGIN(NORMAL, logger_, m, "");
    Bit commandPoolSize = 0;
    Bit dataSize = 0;
    calculateSizes(compound->getCommandPool(), commandPoolSize, dataSize);
    m << "Receiving segment: segment size = " << commandPoolSize + dataSize << "bit"
      << ", segment number = " << command->local.segmentNumber << "\n"
      << "                   received segments = [";
    for (std::list<int>::iterator it = command->magic.segInfoPtr->receivedSegments.begin();
         it != command->magic.segInfoPtr->receivedSegments.end();
         it++)
    {
        if (it != command->magic.segInfoPtr->receivedSegments.begin())
            m << ", ";
            m << (*it);
    }
    m << "]";
    MESSAGE_END();

    assure(std::find(command->magic.segInfoPtr->receivedSegments.begin(),
                     command->magic.segInfoPtr->receivedSegments.end(),
                     command->local.segmentNumber)
           == command->magic.segInfoPtr->receivedSegments.end(),
           "Received same segment twice!");

    command->magic.segInfoPtr->receivedSegments.push_back(command->local.segmentNumber);

    if (command->magic.segInfoPtr->numberSegments
        == command->magic.segInfoPtr->receivedSegments.size())
    {
        MESSAGE_BEGIN(NORMAL, logger_, m, "");
        Bit commandPoolSize = 0;
        Bit dataSize = 0;
        getFUN()->calculateSizes(compound->getCommandPool(), commandPoolSize, dataSize, this);
        m << "Delivering reassembled compound: compound size = " << commandPoolSize + dataSize << "bit"
          << ", number of segments = " << command->magic.segInfoPtr->numberSegments;
        MESSAGE_END();

        getDeliverer()->getAcceptor(compound)->onData(compound);
    }
} // doOnData


bool
DynamicSAR::doIsAccepting(const CompoundPtr& compound) const
{
    if(currentCompound_ != NULL)
        return false;

    CompoundPtr compoundCopy = compound->copy();

    activateCommand(compoundCopy->getCommandPool());

    MESSAGE_BEGIN(NORMAL, logger_, m, "");
    Bit commandPoolSize = 0;
    Bit dataSize = 0;
    calculateSizes(compoundCopy->getCommandPool(), commandPoolSize, dataSize);
    m << "Checking if compound with size = " << commandPoolSize + dataSize
      << "bit is being accepted by lower FU";
    MESSAGE_END();

    bool accepting = getConnector()->hasAcceptor(compoundCopy);

    MESSAGE_BEGIN(NORMAL, logger_, m, "");
    m << "Lower FU is ";
    if (!accepting)
        m << "not ";
        m << "accepting the compound";
    MESSAGE_END();

    return accepting;
} // doIsAccepting


void
DynamicSAR::doWakeup()
{
    if (currentCompound_ != NULL)
        sendSegments();

    getReceptor()->wakeup();
} // doWakeup


void
DynamicSAR::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    DynamicSARCommand* command = getCommand(commandPool);

    commandPoolSize = 0;
    sduSize = command->magic.segmentSize;
} // calculateSizes


bool
DynamicSAR::compare(Bit currentSegmentSize)
{
    CompoundPtr compoundCopy = currentCompound_->copy();

    DynamicSARCommand* command = activateCommand(compoundCopy->getCommandPool());
    command->magic.segmentSize = currentSegmentSize;

    return getConnector()->hasAcceptor(compoundCopy);
} // compare


void
DynamicSAR::sendSegments()
{
    while (true)
    {
        int currentMaxSegmentSize = maxSegmentSize_;

        if (currentCompoundSize_ - currentCompoundSentSize_ < maxSegmentSize_)
            currentMaxSegmentSize = currentCompoundSize_ - currentCompoundSentSize_;

        int segmentSize = searchAlgo_->search(currentMaxSegmentSize + 1);
        if (segmentSize < 1)
            return;

        MESSAGE_BEGIN(NORMAL, logger_, m, "");
        m << "Sending segment: current max. segment size = " << currentMaxSegmentSize
          << "bit, segment size = " << segmentSize
          << "bit, segment number = " << segmentNumber_;
        MESSAGE_END();

        currentCompoundSentSize_ += segmentSize;

        CompoundPtr compoundCopy = currentCompound_->copy();
        DynamicSARCommand* command = activateCommand(compoundCopy->getCommandPool());
        command->local.segmentNumber = segmentNumber_;
        command->magic.segmentSize = segmentSize;
        command->magic.segInfoPtr = currentSegInfoPtr_;

        if (currentCompoundSentSize_ == currentCompoundSize_)
        {
            MESSAGE_BEGIN(NORMAL, logger_, m, "");
            m << "                 This is the last segment of the current compound!";
            MESSAGE_END();

            command->magic.segInfoPtr->numberSegments = segmentNumber_;
            currentCompound_ = CompoundPtr();
            getConnector()->getAcceptor(compoundCopy)->sendData(compoundCopy);
            return;
        }

        segmentNumber_++;
        getConnector()->getAcceptor(compoundCopy)->sendData(compoundCopy);
    }
} // sendSegments
