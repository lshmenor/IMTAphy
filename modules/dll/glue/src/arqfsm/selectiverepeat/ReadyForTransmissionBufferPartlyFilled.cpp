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

#include "ReadyForTransmissionBufferPartlyFilled.hpp"
#include "ReadyForTransmissionBufferEmpty.hpp"
#include "WaitingForACKsBufferFull.hpp"

#include <WNS/Assure.hpp>
#include <WNS/Exception.hpp>

#include <iostream>

using namespace glue::arqfsm::selectiverepeat;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(ReadyForTransmissionBufferPartlyFilled,
				     StateInterface,
				     "glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferPartlyFilled",
				     wns::fsm::FSMConfigCreator);

StateInterface*
ReadyForTransmissionBufferPartlyFilled::onSendData(const CompoundPtr& compound)
{
	SelectiveRepeatCommand* command = dynamic_cast<SelectiveRepeatCommand*>(getFU()->activateCommand(compound->getCommandPool()));
	assure(command, "Command is not a SelectiveRepeatCommand.");

	command->peer.type = SelectiveRepeatCommand::I;
	command->peer.NS = vars().NS;

	MESSAGE_BEGIN(NORMAL, vars().logger, m, getFUNName());
	m << " processOutgoing"
	  << " NS -> " << command->peer.NS;
	MESSAGE_END();

	vars().sentCompounds[vars().NS] = compound;

	vars().NS = (vars().NS + 1) % vars().sequenceNumberSize;
	++vars().levelSendBuffer;

	vars().pendingCompoundsQueue.push_back(compound);

	tryToSendIs();

	if (vars().levelSendBuffer == vars().windowSize)
		return getFSM()->createState<WaitingForACKsBufferFull>();

	return this;
} // onSendData


StateInterface*
ReadyForTransmissionBufferPartlyFilled::onACK(int sequenceNumber)
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
			return getFSM()->createState<ReadyForTransmissionBufferEmpty>();
	}

	return this;
} // onACK


StateInterface*
ReadyForTransmissionBufferPartlyFilled::onWakeup()
{
	tryToSendACKs();
	tryToSendIs();
	sendWakeup();
	return this;
} // onWakeup


void
ReadyForTransmissionBufferPartlyFilled::onIsAccepting(bool& accepting) const
{
	accepting = true;
} // onIsAccepting


StateInterface*
ReadyForTransmissionBufferPartlyFilled::onTimeout(const CompoundPtr& compound)
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


