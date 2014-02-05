/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include "WaitingForACKsBufferFull.hpp"
#include "ReadyForTransmissionBufferPartlyFilled.hpp"
#include "ReadyForTransmissionBufferEmpty.hpp"

#include <WNS/Assure.hpp>
#include <WNS/Exception.hpp>

using namespace glue::arqfsm::selectiverepeat;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(WaitingForACKsBufferFull,
				     StateInterface,
				     "glue_arqfsm_selectiverepeat_WaitingForACKsBufferFull",
				     wns::fsm::FSMConfigCreator);

StateInterface*
WaitingForACKsBufferFull::onSendData(const CompoundPtr&)
{
	assure(false, "onSendData may not be called in state WaitingForACKsBufferFull.");
	return this;
} // onSendData


StateInterface*
WaitingForACKsBufferFull::onACK(int sequenceNumber)
{
	MESSAGE_BEGIN(NORMAL, vars().logger, m, getFUNName());
	m << " Received ACK frame "
	  << sequenceNumber;
	MESSAGE_END();

	if (sequenceNumber < 0 || sequenceNumber >= vars().sequenceNumberSize)
		throw wns::Exception("Sequence number out of bounds");

	CompoundPtr compound = vars().sentCompounds[sequenceNumber];

	if (compound)
	{
		FSMFU* fsmfu = dynamic_cast<FSMFU*>(getFU());
		if (fsmfu->hasTimeoutSet(compound))
			fsmfu->cancelTimeout(compound);

		vars().pendingCompoundsQueue.remove(compound);

		vars().sentCompounds[sequenceNumber] = CompoundPtr();
	}

	while (!vars().sentCompounds[vars().lowerBoundNS])
	{
		vars().lowerBoundNS = (vars().lowerBoundNS + 1) % vars().sequenceNumberSize;

		--vars().levelSendBuffer;

		if (vars().levelSendBuffer == 0)
		{
			sendWakeup();
			return getFSM()->createState<ReadyForTransmissionBufferEmpty>();
		}
	}

	if (vars().levelSendBuffer < vars().windowSize)
	{
		sendWakeup();
		return getFSM()->createState<ReadyForTransmissionBufferPartlyFilled>();
	}

	return this;
} // onACK


StateInterface*
WaitingForACKsBufferFull::onWakeup()
{
	tryToSendACKs();
	tryToSendIs();
	return this;
} // onWakeup


void
WaitingForACKsBufferFull::onIsAccepting(bool& accepting) const
{
	accepting = false;
} // onIsAccepting


StateInterface*
WaitingForACKsBufferFull::onTimeout(const wns::ldk::CompoundPtr& compound)
{
	if (std::find(vars().pendingCompoundsQueue.begin(),
		      vars().pendingCompoundsQueue.end(),
		      compound)
	    == vars().pendingCompoundsQueue.end())
	{
		vars().pendingCompoundsQueue.push_back(compound);
	}

	tryToSendIs();

	return this;
} // onTimeout


