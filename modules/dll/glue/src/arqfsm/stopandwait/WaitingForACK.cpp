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

#include <GLUE/arqfsm/stopandwait/WaitingForACK.hpp>
#include <GLUE/arqfsm/stopandwait/ReadyForTransmission.hpp>
#include <GLUE/arqfsm/stopandwait/FSMFU.hpp>

using namespace glue::arqfsm::stopandwait;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(WaitingForACK,
				     StateInterface,
				     "glue_arqfsm_stopandwait_WaitingForACK",
				     wns::fsm::FSMConfigCreator);

void
WaitingForACK::onInitState()
{
	assureType(getFU(), FSMFU*);
	FSMFU* fsmfu = dynamic_cast<FSMFU*>(getFU());

	fsmfu->setTimeout(vars().resendTimeout);
} // onInitState


void
WaitingForACK::onExitState()
{
	assureType(getFU(), FSMFU*);
	FSMFU* fsmfu = dynamic_cast<FSMFU*>(getFU());

	fsmfu->cancelTimeout();
} // onExitState


StateInterface*
WaitingForACK::onSendData(const CompoundPtr&)
{
	throw wns::Exception("Invalid signal (onsendData) in state WaitingForACK");

	return this;
} // onSendData


StateInterface*
WaitingForACK::onACK(int sequenceNumber)
{
	if(sequenceNumber != vars().NS)
	{
		MESSAGE_BEGIN(NORMAL, vars().logger, m, getFUNName());
		m << " Unexpected RR frame"
		  << " expected " << vars().NS
		  << " received " << sequenceNumber;
		MESSAGE_END();

		return this;
	}

	MESSAGE_BEGIN(NORMAL, vars().logger, m, getFUNName());
	m << " RR frame " << sequenceNumber;
	MESSAGE_END();

	vars().activeCompound = CompoundPtr();
	vars().sendNow = false;

	sendWakeup();
	return getFSM()->createState<ReadyForTransmission>();
} // onACK


StateInterface*
WaitingForACK::onWakeup()
{
	if (vars().ackCompound && isAccepting(vars().ackCompound))
	{
		sendSendData(vars().ackCompound);
		vars().ackCompound = CompoundPtr();
	}

	if (vars().sendNow == true && isAccepting(vars().activeCompound))
	{
		sendSendData(vars().activeCompound->copy());
		vars().sendNow = false;
	}

	return this;
} // onWakeup


void
WaitingForACK::onIsAccepting(bool& accepting) const
{
	accepting = false;
} // onIsAccepting


StateInterface*
WaitingForACK::onTimeout()
{
	assure(vars().activeCompound, "Unexpected timeout.");

	assureType(getFU(), FSMFU*);
	FSMFU* fsmfu = dynamic_cast<FSMFU*>(getFU());

	fsmfu->setTimeout(vars().resendTimeout);

	if (isAccepting(vars().activeCompound))
		sendSendData(vars().activeCompound->copy());
	else
		vars().sendNow = true;

	return this;
} // onTimeout



