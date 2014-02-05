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

#include <CONSTANZE/ConstanzeComponent.hpp>
#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/Binding.hpp>
#include <CONSTANZE/Listener.hpp>

#include <WNS/module/Base.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/node/Node.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	ConstanzeComponent,
	wns::node::component::Interface,
	"Constanze",
	wns::node::component::ConfigCreator);

ConstanzeComponent::ConstanzeComponent(
	wns::node::Interface* _node,
	const wns::pyconfig::View& _pyco) :

	Component(_node, _pyco),
	pyco(_pyco),
	log(pyco.get("logger"))
{
	MESSAGE_SINGLE(NORMAL, log, "New Component created.");
}

void
ConstanzeComponent::doStartup()
{
	MESSAGE_SINGLE(VERBOSE, log, "Component::doStartup() called.");
}

ConstanzeComponent::~ConstanzeComponent()
{
	while(!listOfGenerators.empty())
	{
		delete *(listOfGenerators.begin());
		listOfGenerators.erase(listOfGenerators.begin());
	}
	while(!listOfGeneratorBindings.empty())
	{
		delete *(listOfGeneratorBindings.begin());
		listOfGeneratorBindings.erase(listOfGeneratorBindings.begin());
	}
	while(!listOfListeners.empty())
	{
		delete *(listOfListeners.begin());
		listOfListeners.erase(listOfListeners.begin());
	}
	while(!listOfListenerBindings.empty())
	{
		delete *(listOfListenerBindings.begin());
		listOfListenerBindings.erase(listOfListenerBindings.begin());
	}
}

void
ConstanzeComponent::onNodeCreated()
{
	int numgen=pyco.len("generators");
	int numlis=pyco.len("listeners");
	MESSAGE_SINGLE(NORMAL, log, "onNodeCreated(): initializing " << numgen << " generators, " << numlis << " listeners.");

	// data source (Generators)
	for (int ii=0; ii < numgen ; ++ii)
	{
		// create generator
		wns::pyconfig::View generatorView = pyco.get("generators", ii);
		std::string generatorPlugin = generatorView.get<std::string>("__plugin__");
		MESSAGE_SINGLE(NORMAL, log, "source "<<ii<<": generator plugin="<<generatorPlugin);
		// generatorPlugin can be "PointProcess","MMPP","MMBP", todo: "ARMA"
		// OLD: GeneratorBase* generator = new Generator(generatorView);
		GeneratorBase::Creator* generatorCreator;
		generatorCreator = GeneratorBase::Factory::creator(generatorPlugin);
		GeneratorBase* generator = generatorCreator->create(generatorView);
		listOfGenerators.push_back(generator);

		// create binding
		wns::pyconfig::View bindingView = pyco.get("generatorBindings", ii);
		std::string bindingName = bindingView.get<std::string>("nameInBindingFactory");
		Binding::Creator* bindingCreator;
		bindingCreator = Binding::Factory::creator(bindingName);
		Binding* generatorBinding = bindingCreator->create(bindingView);
		MESSAGE_SINGLE(VERBOSE, log, "source "<<ii<<": generatorBinding created");
		listOfGeneratorBindings.push_back(generatorBinding);
		assure(generator, "No generator exists to register binding.");
		generator->registerBinding(generatorBinding);
		assure(generatorBinding, "No generator binding exists to register component.");
		/**
		 * @todo Find a better method to get the services. At the moment there
		 * exists a strong coupling between the binding and the component. At
		 * least try to get rid of the bindingView parameter.
		 */
		generatorBinding->registerComponent(this);
	}

	// data sink (Listeners)
	for (int ii=0; ii < numlis; ++ii)
	{
		wns::pyconfig::View listenerView = pyco.get("listeners", ii);
		Listener* listener = new Listener(listenerView, this);
		listOfListeners.push_back(listener);

		// create binding
		wns::pyconfig::View bindingView = pyco.get("listenerBindings", ii);
		std::string bindingName = bindingView.get<std::string>("nameInBindingFactory");
		Binding::Creator* bindingCreator = Binding::Factory::creator(bindingName);
		Binding* listenerBinding = bindingCreator->create(bindingView);
		MESSAGE_SINGLE(VERBOSE, log, "sink " << ii << ": listenerBinding created");
		listOfListenerBindings.push_back(listenerBinding);
		assure(listener, "No listener exists to register binding.");
		listener->registerBinding(listenerBinding);
		assure(listenerBinding, "No listener binding exists to register component.");
		listenerBinding->registerComponent(this);
	}
	MESSAGE_SINGLE(VERBOSE, log, "onNodeCreated() ready");
}

void
ConstanzeComponent::onWorldCreated()
{

}

void
ConstanzeComponent::onShutdown()
{

}

