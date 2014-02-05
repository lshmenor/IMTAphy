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

#include <GLUE/Component.hpp>
#include <GLUE/convergence/Upper.hpp>
#include <GLUE/convergence/Lower.hpp>

#include <WNS/service/dll/Handler.hpp>

#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/utils.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Exception.hpp>

#include <boost/bind.hpp>

#include <sstream>

using namespace glue;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Component,
	wns::node::component::Interface,
	"glue.Component",
	wns::node::component::ConfigCreator
	);

Component::Component(
	wns::node::Interface* _node,
	const wns::pyconfig::View& _config) :

	wns::node::component::Component(_node, _config),
	fun(NULL),
	config(_config),
	unicastUpperConvergence(NULL),
	broadcastUpperConvergence(NULL),
	lowerConvergence(NULL),
	sourceMACAddress(wns::service::dll::UnicastAddress(config.get<int>("address"))),
    stationType_(StationTypes::fromString(config.get<std::string>("stationType"))),
	logger(config.get<wns::pyconfig::View>("logger"))
{
} // Component

void
Component::doStartup()
{
	// build FUN
	fun = new wns::ldk::fun::Main(this);
	wns::ldk::configureFUN(fun, config.get<wns::pyconfig::View>("fun"));

	// configure upper convergence (services, MAC address)
	unicastUpperConvergence =
		dynamic_cast<convergence::UnicastUpper*>(
			fun->getFunctionalUnit(
				config.get<std::string>("unicastUpperConvergence.commandName")));
	unicastUpperConvergence->setMACAddress(sourceMACAddress);

	broadcastUpperConvergence =
		dynamic_cast<convergence::BroadcastUpper*>(
			fun->getFunctionalUnit(
				config.get<std::string>("broadcastUpperConvergence.commandName")));
	broadcastUpperConvergence->setMACAddress(sourceMACAddress);

	// register services
	addService(getConfig().get<std::string>("unicastDataTransmission"), unicastUpperConvergence);
	addService(getConfig().get<std::string>("unicastNotification"), unicastUpperConvergence);
	addService(getConfig().get<std::string>("broadcastDataTransmission"), broadcastUpperConvergence);
	addService(getConfig().get<std::string>("broadcastNotification"), broadcastUpperConvergence);

	// configure lower convergence (services, MAC address)
	lowerConvergence =
		dynamic_cast<convergence::Lower*>(
			fun->getFunctionalUnit(
				config.get<std::string>("lowerConvergence.commandName")));
	lowerConvergence->setMACAddress(sourceMACAddress);
}

Component::~Component()
{
	delete fun;
} // ~Component

void
Component::onNodeCreated()
{
	MESSAGE_BEGIN(NORMAL, logger, m, fun->getName());
	m << ": onNodeCreated(), setting phyiscal layer";
	MESSAGE_END();

	// set services in lower convergence to communicate with lower layer
	lowerConvergence->setDataTransmissionService(
		getService<wns::service::Service*>(
			getConfig().get<std::string>("phyDataTransmission")));

	lowerConvergence->setNotificationService(
		getService<wns::service::Service*>(
			getConfig().get<std::string>("phyNotification")));
} // onNodeCreated

void
Component::onWorldCreated()
{
	fun->onFUNCreated();
} // onWorldCreated

void
Component::onShutdown()
{
} // onShutdown


