/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/convergence/Upper.hpp>
#include <GLUE/Component.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/Assure.hpp>

using namespace glue::convergence;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	UnicastUpper,
	wns::ldk::FunctionalUnit,
	"glue.convergence.UnicastUpper",
	wns::ldk::FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	BroadcastUpper,
	wns::ldk::FunctionalUnit,
	"glue.convergence.BroadcastUpper",
	wns::ldk::FUNConfigCreator);

wns::ldk::CommandPool*
UnicastUpper::createReply(const wns::ldk::CommandPool* original) const
{
	wns::ldk::CommandPool* reply = getFUN()->getProxy()->createCommandPool();

	UnicastUpperCommand* originalCommand = getCommand(original);
	UnicastUpperCommand* replyCommand = activateCommand(reply);

	replyCommand->peer.sourceMACAddress = sourceMACAddress_;
	replyCommand->peer.targetMACAddress = originalCommand->peer.sourceMACAddress;

	return reply;
} // UnicastUpper::createReply


wns::ldk::CommandPool*
BroadcastUpper::createReply(const wns::ldk::CommandPool* /*original*/) const
{
 	assure(false, "BroadcastUpper::createReply currently not implemented");
 	// prevent compiler warning:
 	return NULL;
} // BroadcastUpper::createReply
