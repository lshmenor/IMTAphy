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

#include <GLUE/arqfsm/selectiverepeat/BaseState.hpp>

using namespace glue::arqfsm::selectiverepeat;
using namespace wns::ldk;

void
BaseState::deliverCompounds()
{
	while (vars().receivedCompounds[vars().leastNR])
	{
		MESSAGE_BEGIN(NORMAL, vars().logger, m, getFUNName());
		m << " Delivering compound "
		  << vars().leastNR;
		MESSAGE_END();

		sendOnData(vars().receivedCompounds[vars().leastNR]);
		vars().receivedCompounds[vars().leastNR] = CompoundPtr();

		vars().leastNR = (vars().leastNR + 1) % vars().sequenceNumberSize;
	}
} // deliverCompounds


void
BaseState::tryToSendACKs()
{
	while (!vars().pendingACKsQueue.empty())
	{
		CompoundPtr ackCompound = vars().pendingACKsQueue.front();
		if (isAccepting(ackCompound))
		{
			vars().pendingACKsQueue.pop_front();
			sendSendData(ackCompound->copy());
		}
		else
			break;
	}
} // tryToSendACKs


void
BaseState::tryToSendIs()
{
	while (!vars().pendingCompoundsQueue.empty())
	{
		CompoundPtr compound = vars().pendingCompoundsQueue.front();
		if (isAccepting(compound))
		{
			vars().pendingCompoundsQueue.pop_front();

			FSMFU* fsmfu = dynamic_cast<FSMFU*>(getFU());
			fsmfu->setTimeout(compound, vars().resendTimeout);

			sendSendData(compound->copy());
		}
		else
			break;
	}
} // tryToSendIs


StateInterface*
BaseState::onI(const CompoundPtr& compound, int sequenceNumber)
{
	MESSAGE_BEGIN(NORMAL, vars().logger, m, getFUNName());
	m << " Received I frame "
	  << sequenceNumber;
	MESSAGE_END();

	if (sequenceNumber < 0 || sequenceNumber >= vars().sequenceNumberSize)
		throw wns::Exception("Sequence number out of bounds");

	if ((sequenceNumber - vars().leastNR + vars().sequenceNumberSize) % vars().sequenceNumberSize
	   < vars().windowSize)
	{
		vars().receivedCompounds[sequenceNumber] = compound;

		if (!vars().delayingDelivery)
			deliverCompounds();
	}

	if ((vars().delayingDelivery &&
	     (sequenceNumber - vars().leastNR + vars().sequenceNumberSize) % vars().sequenceNumberSize < vars().windowSize) ||
	    !vars().delayingDelivery)
	{
		if (!pendingACK(sequenceNumber))
		{
			CompoundPtr ackCompound = createReply(compound);

			SelectiveRepeatCommand* ackCommand =
				dynamic_cast<SelectiveRepeatCommand*>(getFU()->activateCommand(ackCompound->getCommandPool()));
			assure(ackCommand, "Command is not a SelectiveRepeatCommand.");

			ackCommand->peer.type = SelectiveRepeatCommand::ACK;
			ackCommand->peer.NS = sequenceNumber;

			vars().pendingACKsQueue.push_back(ackCompound);

			tryToSendACKs();
		}
	}

	return this;
} // onI


bool
BaseState::pendingACK(int sequenceNumber)
{
	Variables::CompoundQueue::iterator it;

	for (it = vars().pendingACKsQueue.begin();
	     it != vars().pendingACKsQueue.end();
	     ++it)
	{
		SelectiveRepeatCommand* ackCommand =
			dynamic_cast<SelectiveRepeatCommand*>(getFU()->getCommand((*it)->getCommandPool()));
		assure(ackCommand, "Command is not a SelectiveRepeatCommand.");

		assure(SelectiveRepeatCommand::ACK == ackCommand->peer.type, "Invalid frame type.");

		if (ackCommand->peer.NS == sequenceNumber)
			return true;
	}

	return false;
} // pendingACK


