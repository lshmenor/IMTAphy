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

#include <GLUE/Routing.hpp>
#include <GLUE/convergence/Upper.hpp>

using namespace glue;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Routing,
				     wns::ldk::FunctionalUnit,
				     "glue.Routing",
				     wns::ldk::FUNConfigCreator);


Routing::Routing(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
	wns::ldk::CommandTypeSpecifier<RoutingCommand>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<Routing>(),

	allowRouteChange(_config.get<bool>("allowRouteChange")),
	routingCommandPool(NULL),
	config(_config),
	logger(_config.get<wns::pyconfig::View>("logger"))
{
	friends.addressProviderFU = NULL;
} // Routing


Routing::~Routing()
{
} // ~Routing


void
Routing::onFUNCreated()
{
	friends.addressProviderFU = getFUN()->findFriend<FunctionalUnit*>(config.get<std::string>("addressProvider"));
} // onFUNCreated


void
Routing::doSendData(const wns::ldk::CompoundPtr& compound)
{
	assure(isAccepting(compound), "sendData called although not accepting!");

	if (!getFUN()->getProxy()->commandIsActivated(compound->getCommandPool(),
						      friends.addressProviderFU))
	{
 		if (!routingCommandPool)
 		{
 			MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
 			m << ": Routing information not (yet) available. Discarding compound...";
 			MESSAGE_END();

 			return;
 		}
		else
		{
			getFUN()->getProxy()->copy(compound->getCommandPool(), routingCommandPool);

			MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
			m << ": Routing information to compound added";
			MESSAGE_END();
		}
	}

	activateCommand(compound->getCommandPool());

	getConnector()->getAcceptor(compound)->sendData(compound);
} // doSendData


void
Routing::doOnData(const wns::ldk::CompoundPtr& compound)
{
	if (!routingCommandPool)
	{
		routingCommandPool = createReply(compound->getCommandPool());

		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << ": Setting routing information";
		MESSAGE_END();
	}
	else if (allowRouteChange)
	{
		if (routingCommandPool)
			delete routingCommandPool;
		routingCommandPool = createReply(compound->getCommandPool());

		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << ": Setting new routing information";
		MESSAGE_END();
	}

	getDeliverer()->getAcceptor(compound)->onData(compound);
} // doOnData


bool
Routing::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
	if (getFUN()->getProxy()->commandIsActivated(compound->getCommandPool(),
						     friends.addressProviderFU))
	{
		return getConnector()->hasAcceptor(compound);
	}
	else
	{
		if (routingCommandPool)
		{
			CompoundPtr compoundCopy = compound->copy();
			getFUN()->getProxy()->copy(compoundCopy->getCommandPool(), routingCommandPool);
			return getConnector()->hasAcceptor(compoundCopy);
		}
	}

	return false;
} // doIsAccepting



