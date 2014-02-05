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

#include <GLUE/arqfsm/selectiverepeat/ReadyForTransmissionBufferEmpty.hpp>
#include <GLUE/arqfsm/selectiverepeat/ReadyForTransmissionBufferPartlyFilled.hpp>

#include <WNS/Assure.hpp>

using namespace glue::arqfsm::selectiverepeat;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(ReadyForTransmissionBufferEmpty,
				     StateInterface,
				     "glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferEmpty",
				     wns::fsm::FSMConfigCreator);

void
ReadyForTransmissionBufferEmpty::onInitState()
{
	assureType(getFU(), wns::ldk::SuspendSupport*);
	dynamic_cast<wns::ldk::SuspendSupport*>(getFU())->trySuspend();
} // onInitState


StateInterface*
ReadyForTransmissionBufferEmpty::onSendData(const CompoundPtr& compound)
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

	return getFSM()->createState<ReadyForTransmissionBufferPartlyFilled>();
} // onSendData


StateInterface*
ReadyForTransmissionBufferEmpty::onACK(int)
{
	MESSAGE_BEGIN(NORMAL, vars().logger, m, getFUNName());
	m << " ignoring unexpected ACK";
	MESSAGE_END();

	return this;
} // onACK


StateInterface*
ReadyForTransmissionBufferEmpty::onWakeup()
{
	tryToSendACKs();
	sendWakeup();
	return this;
} // onWakeup


void
ReadyForTransmissionBufferEmpty::onIsAccepting(bool& accepting) const
{
	accepting = true;
} // onIsAccepting


StateInterface*
ReadyForTransmissionBufferEmpty::onTimeout(const CompoundPtr&)
{
	assure(false, "Invalid timeout signal!");
	return this;
} // onTimeout



