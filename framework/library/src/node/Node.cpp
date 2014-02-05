/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WNS/node/Node.hpp>
#include <WNS/node/Registry.hpp>
#include <WNS/service/Service.hpp>
#include <WNS/node/component/Interface.hpp>
#include <WNS/Functor.hpp>
#include <WNS/Assure.hpp>

#include <WNS/probe/bus/ContextProviderCollection.hpp>
#include <WNS/probe/bus/ContextProvider.hpp>

using namespace wns::node;

std::list<unsigned long int> Node::ids;

Node::Node(Registry* registry, const pyconfig::View& pyco) :
	localServices(),
	contextProviderRegistry(),
	name(pyco.get<std::string>("name")),
	nodeID(pyco.get<unsigned long int>("nodeID")),
	config(pyco),
	log(pyco.get("logger")),
	globalNodes(registry),
	localComponents()
{
	assure(name != "", "No name given!");
	assure(globalNodes,  "must be non-NULL");

	// Check if nodeID was not allocated yet
	if (std::find(ids.begin(), ids.end(), nodeID) != ids.end())
	{
		// We throw an error now. Unfortunately we need to cleanup our
		// allocated data, the destructor will not be called
		// if you throw an exception within a constructor.
		// @todo dbn : apply auto_ptr to IDProvider Registries
		throw wns::Exception("Duplicate Node ID detected!");
	}

	// All went well, mark this nodeID allocated
	Node::ids.insert(ids.begin(),nodeID);

	for (int ii = 0; ii < pyco.len("contextProviders"); ++ii)
	{
		// for now only ConstantContextProviders are allowed
		wns::pyconfig::View providerView = pyco.get("contextProviders", ii);
		getContextProviderCollection().addProvider(wns::probe::bus::contextprovider::Constant(providerView));
	}

    // add Node to registry
    globalNodes->insert(name, this);
}

void
Node::startup()
{
	// loop over the list to create all components
	for(int ii = 0;
	    ii < config.len("components");
	    ++ii)
	{
		pyconfig::View componentConfig = config.get<pyconfig::View>("components", ii);

		// get the type of the Component and ask the ComponentFactory
		std::string plugin = componentConfig.get<std::string>("nameInComponentFactory");

		component::Creator* cc = component::Factory::creator(plugin);

		// create and add the Component to this Node
		component::Interface* ci = cc->create(this, componentConfig);
		localComponents.push_back(ci);
		ci->startup();
	}

	// tell all Components that we're ready with construction
	for(ComponentContainer::iterator itr = localComponents.begin();
	    itr != localComponents.end();
	    ++itr)
	{
		(*itr)->onNodeCreated();
	}
} // Node


Node::~Node()
{
	// delete all Components
	for(ComponentContainer::iterator it = localComponents.begin();
	    it != localComponents.end();
	    ++it)
	{
		delete *it;
	}

	// clear the container
	localComponents.clear();
} // ~Node


void
Node::addService(const std::string& name, service::Service* si)
{
	assure(si, "must be non-NULL");
	localServices.insert(name, si);
}

void
Node::onWorldCreated()
{
	std::for_each(localComponents.begin(),
		      localComponents.end(),
		      std::mem_fun(&wns::node::component::Interface::onWorldCreated));
}

void
Node::onShutdown()
{
	std::for_each(localComponents.begin(),
		      localComponents.end(),
		      std::mem_fun(&wns::node::component::Interface::onShutdown));
}

wns::probe::bus::ContextProviderCollection&
Node::getContextProviderCollection()
{
	return contextProviderRegistry;
}

std::string
Node::getName() const
{
    assure(name != "", "No name given!");
    return name;
}

unsigned int
Node::getNodeID() const
{
    return nodeID;
}



wns::service::Service*
Node::getAnyService(const component::FQSN& fqsn) const
{
	if(fqsn.getNodeName() == getName())
	{
		// we can answer the question
		return localServices.find(fqsn.getServiceName());
	}
	else
	{
		// ask someone else
		node::Interface* node =
			globalNodes->find(fqsn.getNodeName());
		return node->getService<wns::service::Service*>(fqsn.getServiceName());
	}
}


/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
