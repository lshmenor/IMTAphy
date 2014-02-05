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

#include <GLUE/arqfsm/selectiverepeat/FSMFU.hpp>
#include <GLUE/arqfsm/selectiverepeat/ReadyForTransmissionBufferEmpty.hpp>

#include <iostream>

using namespace glue::arqfsm::selectiverepeat;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(FSMFU, FunctionalUnit, "glue.arqfsm.selectiverepeat.FSMFU", FUNConfigCreator);

FSMFU::FSMFU(fun::FUN* fun, const wns::pyconfig::View& _config) :
	MyFUInterface(Variables(_config)),
	wns::ldk::CommandTypeSpecifier<SelectiveRepeatCommand>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::ldk::SuspendSupport(fun, _config),
	wns::Cloneable<FSMFU>(),
	wns::events::MultipleTimeout<CompoundPtr>(),
	config(_config),
	bitsPerIFrame(config.get<int>("bitsPerIFrame")),
	bitsPerACKFrame(config.get<int>("bitsPerACKFrame"))
{
	changeState(createState<ReadyForTransmissionBufferEmpty>());
} // FSMFU


void
FSMFU::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
	// What are the sizes in the upper FUs?
	getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

	SelectiveRepeatCommand* command = getCommand(commandPool);
	switch(command->peer.type) {
	case SelectiveRepeatCommand::I:
		commandPoolSize += bitsPerIFrame;
		break;
	case SelectiveRepeatCommand::ACK:
		commandPoolSize += bitsPerACKFrame;
		break;
	}
} // calculateSizes


void
FSMFU::onTimeout(const CompoundPtr& compound)
{
	assureType(this->getState(), BaseState*);

	BaseState* bs = dynamic_cast<BaseState*>(this->getState());

	do
	{
		++inAction;
                BaseFSM::StateInterface* stateInterface = bs->onTimeout(compound);
		--inAction;
		this->changeState(stateInterface);
	}
	while (linkHandler());
	return;
} // onTimeout


bool
FSMFU::onSuspend() const
{
	return getStateName() == std::string("glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferEmpty");
} // onSuspend


void
FSMFU::doDelayDelivery()
{
	getVariables().delayingDelivery = true;
} // doDelayDelivery


void
FSMFU::doDeliver()
{
	getVariables().delayingDelivery = false;

	assureType(this->getState(), BaseState*);
	BaseState* bs = dynamic_cast<BaseState*>(this->getState());

	bs->deliverCompounds();
} // doDeliver



