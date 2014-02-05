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

#include <GLUE/arqfsm/stopandwait/ReadyForTransmission.hpp>
#include <GLUE/arqfsm/stopandwait/WaitingForACK.hpp>
#include <GLUE/arqfsm/stopandwait/FSMFU.hpp>

#include <WNS/ldk/SuspendSupport.hpp>

using namespace glue::arqfsm::stopandwait;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(ReadyForTransmission,
				     StateInterface,
				     "glue_arqfsm_stopandwait_ReadyForTransmission",
				     wns::fsm::FSMConfigCreator);

void
ReadyForTransmission::onInitState()
{
	assureType(getFU(), wns::ldk::SuspendSupport*);
	dynamic_cast<wns::ldk::SuspendSupport*>(getFU())->trySuspend();
} // onInitState


StateInterface*
ReadyForTransmission::onSendData(const CompoundPtr& compound)
{
	vars().activeCompound = compound;

	StopAndWaitCommand* command = dynamic_cast<StopAndWaitCommand*>(getFU()->activateCommand(compound->getCommandPool()));
	assure(command, "Command is not a StopAndWaitCommand.");

	command->peer.type = StopAndWaitCommand::I;
	command->peer.NS = vars().NS;

	MESSAGE_BEGIN(NORMAL, vars().logger, m, getFUNName());
	m << " processOutgoing"
	  << " NS -> " << command->peer.NS;
	MESSAGE_END();

	vars().NS = (vars().NS + 1) % vars().modulo;

	if (isAccepting(compound))
		sendSendData(compound->copy());
	else
		vars().sendNow = true;

	return getFSM()->createState<WaitingForACK>();
} // onSendData


StateInterface*
ReadyForTransmission::onACK(int sequenceNumber)
{
	MESSAGE_BEGIN(NORMAL, vars().logger, m, getFUNName());
	m << " Unexpected RR frame"
	  << " received " << sequenceNumber;
	MESSAGE_END();

	return this;
} // onACK


StateInterface*
ReadyForTransmission::onWakeup()
{
	if (vars().ackCompound && isAccepting(vars().ackCompound))
	{
		sendSendData(vars().ackCompound);
		vars().ackCompound = CompoundPtr();
	}
	else
		sendWakeup();

	return this;
} // onWakeup


void
ReadyForTransmission::onIsAccepting(bool& accepting) const
{
	accepting = true;
} // onIsAccepting


StateInterface*
ReadyForTransmission::onTimeout()
{
	assure(false, "Invalid timeout");
	return this;
} // onTimeout


