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

#include <WNS/ldk/tools/InSequenceChecker.hpp>

using namespace wns::ldk::tools;

STATIC_FACTORY_REGISTER_WITH_CREATOR(InSequenceChecker,
				     wns::ldk::FunctionalUnit,
				     "wns.ldk.tools.InSequenceChecker",
				     wns::ldk::FUNConfigCreator);

InSequenceChecker::InSequenceChecker(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
	Processor<InSequenceChecker>(),
	CommandTypeSpecifier<InSequenceCheckerCommand>(fun),
	HasReceptor<>(),
	HasConnector<>(),
	HasDeliverer<>(),
	wns::Cloneable<InSequenceChecker>(),
	sequenceIDOutgoing(0),
	sequenceIDExpected(0),
	logger(config.get<wns::pyconfig::View>("logger"))
{
} // InSequenceChecker


InSequenceChecker::~InSequenceChecker()
{
} // ~InSequenceChecker


void
InSequenceChecker::processOutgoing(const wns::ldk::CompoundPtr& compound)
{
	InSequenceCheckerCommand* command = activateCommand(compound->getCommandPool());

	MESSAGE_BEGIN(VERBOSE, logger, m, "Tagging outgoing compound")
		m << " with id " << sequenceIDOutgoing;
	MESSAGE_END();

	command->magic.sequenceID = sequenceIDOutgoing;
	++sequenceIDOutgoing;
} // processOutgoing


void
InSequenceChecker::processIncoming(const wns::ldk::CompoundPtr& compound)
{
	InSequenceCheckerCommand* command = getCommand(compound->getCommandPool());

	MESSAGE_BEGIN(VERBOSE, logger, m, "Received incoming compound")
		m << " with id " << command->magic.sequenceID;
	MESSAGE_END();

	if (command->magic.sequenceID != sequenceIDExpected)
	{
		std::stringstream ss;

		ss << "Received out-of-sequence compound. Received compound with id "
		   << command->magic.sequenceID << " but expected id "
		   << sequenceIDExpected << ".";

		throw wns::Exception(ss.str());
	}

	++sequenceIDExpected;
} // processIncoming



