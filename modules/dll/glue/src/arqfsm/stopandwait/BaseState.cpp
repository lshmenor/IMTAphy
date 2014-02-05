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

#include <GLUE/arqfsm/stopandwait/BaseState.hpp>

using namespace glue::arqfsm::stopandwait;
using namespace wns::ldk;

StateInterface*
BaseState::onI(const CompoundPtr& compound, int sequenceNumber)
{
	MESSAGE_BEGIN(NORMAL, vars().logger, m, getFUNName());
	m << " Received I frame "
	  << " expected " << vars().NR
	  << " received " << sequenceNumber;
	MESSAGE_END();

	if(sequenceNumber == vars().NR) {
		// this is the I frame we waited for.
		sendOnData(compound);
		vars().NR = (vars().NR + 1) % vars().modulo;
	}

	vars().ackCompound = createReply(compound);

	StopAndWaitCommand* ackCommand = dynamic_cast<StopAndWaitCommand*>(getFU()->activateCommand(vars().ackCompound->getCommandPool()));
	assure(ackCommand, "Command is not a StopAndWaitCommand.");

	ackCommand->peer.type = StopAndWaitCommand::ACK;
	ackCommand->peer.NS = vars().NR;

	if (isAccepting(vars().ackCompound))
	{
		sendSendData(vars().ackCompound);
		vars().ackCompound = CompoundPtr();
	}

	return this;
} // onI



