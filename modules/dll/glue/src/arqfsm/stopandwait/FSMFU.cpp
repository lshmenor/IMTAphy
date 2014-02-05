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

#include <GLUE/arqfsm/stopandwait/FSMFU.hpp>
#include <GLUE/arqfsm/stopandwait/ReadyForTransmission.hpp>

using namespace glue::arqfsm::stopandwait;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(FSMFU, FunctionalUnit, "glue.arqfsm.stopandwait.FSMFU", FUNConfigCreator);

FSMFU::FSMFU(fun::FUN* fun, const wns::pyconfig::View& _config) :
	MyFUInterface(Variables(_config)),
	wns::ldk::CommandTypeSpecifier<StopAndWaitCommand>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::ldk::SuspendSupport(fun, _config),
	wns::Cloneable<FSMFU>(),
	config(_config),
	bitsPerIFrame(config.get<int>("bitsPerIFrame")),
	bitsPerRRFrame(config.get<int>("bitsPerRRFrame"))
{
	changeState(createState<ReadyForTransmission>());
} // FSMFU


void
FSMFU::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
	// What are the sizes in the upper FUs?
	getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

	StopAndWaitCommand* command = getCommand(commandPool);
	switch(command->peer.type) {
	case StopAndWaitCommand::I:
		commandPoolSize += bitsPerIFrame;
		break;
	case StopAndWaitCommand::ACK:
		commandPoolSize += bitsPerRRFrame;
		break;
	}
} // calculateSizes


void
FSMFU::onTimeout()
{
	assureType(this->getState(), BaseState*);

        BaseState* bs = dynamic_cast<BaseState*>(this->getState());

        do
	{
		++inAction;
                BaseFSM::StateInterface* stateInterface = bs->onTimeout();
		--inAction;
		this->changeState(stateInterface);
	}
        while (linkHandler());
        return;
} // onTimeout


bool
FSMFU::onSuspend() const
{
	return getStateName() == std::string("glue_arqfsm_stopandwait_ReadyForTransmission");
} // onSuspend



