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

#include <WNS/node/tests/NodeTest.hpp>
#include <WNS/node/Registry.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/node/component/FQSN.hpp>
#include <WNS/node/component/tests/IP.hpp>
#include <WNS/node/component/tests/TCP.hpp>
#include <WNS/node/component/tests/ComponentStub.hpp>

#include <WNS/pyconfig/helper/Functions.hpp>

using namespace wns::node::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( NodeTest );

STATIC_FACTORY_REGISTER_WITH_CREATOR(
 	wns::node::tests::ComponentA,
 	wns::node::component::Interface,
 	"wns.node.tests.ComponentA",
 	wns::node::component::ConfigCreator
 	);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
 	wns::node::tests::ComponentB,
 	wns::node::component::Interface,
 	"wns.node.tests.ComponentB",
 	wns::node::component::ConfigCreator
 	);


void NodeTest::prepare()
{
	registry = new wns::node::Registry();
}

void NodeTest::cleanup()
{
	delete registry;
	// Clean the record of registered IDs
	wns::node::Node::ids.clear();
}

void NodeTest::getName()
{
	Node node(
		registry,
		pyconfig::helper::createViewFromDropInConfig("openwns.node",
							     "NodeDropIn"));
	CPPUNIT_ASSERT(node.getName() == "The Node");
}

void NodeTest::noDuplicateNodeIDs()
{
	// The Python Node class should generate unique nodeIDs
	// C++ should detect malformed configurations with duplicate
	// IDs
	std::string config =
		"from openwns.node import Node, TCPDropIn, IPDropIn\n"
		"node1 = Node('AP1')\n"
		"node2 = Node('AP2')\n"
		"#Be a bad boy\n"
		"node1.nodeID = 1\n"
		"node2.nodeID = 1\n";

	wns::pyconfig::View pyco(pyconfig::helper::createViewFromString(config));

	// This should work
	node::Interface* node =
		new Node(registry, pyco.get<wns::pyconfig::View>("node1"));

	// This must throw, duplication detected!
	CPPUNIT_ASSERT_THROW(new Node(registry,
				      pyco.get<wns::pyconfig::View>("node2")),
			     wns::Exception);

	delete node;
}

void NodeTest::addAndGetService()
{
	std::string config =
		"from openwns.node import Node, TCPDropIn, IPDropIn\n"
		"node = Node('AP1')\n"
		"dummy = Node('dummy')\n"
		"tcp = TCPDropIn(dummy)\n";

	wns::pyconfig::View pyco(pyconfig::helper::createViewFromString(config));

	// begin example "Node::addComponent.example"
	// create Node with test configuration
	node::Interface* node =
		new Node(registry, pyco.get<wns::pyconfig::View>("node"));

	component::Interface* ci = new component::tests::ComponentStub(node, "component");

	// create TCP instance ...
	component::tests::TCPInterface* tmp =
		new component::tests::TCP(
			ci, pyco.get<wns::pyconfig::View>("tcp"));

	// ... and add the TCP instance to the Node
	ci->addService(tmp->getName(), tmp);
	// end example

	// begin example "Node::getComponentByName.example"
	// retrieve TCP instance from Node
	component::tests::TCPInterface* tcp =
		node->getService<component::tests::TCPInterface*>("TCP");
	// end example

	// begin example "Node::useTCPInterface.example"
	CPPUNIT_ASSERT_EQUAL( 31337, tcp->getFreePort() );
	CPPUNIT_ASSERT( tcp->getName() == "TCP" );
	// end example
	delete node;
}

void NodeTest::addAndGetTwoServices()
{
	std::string config =
		"from openwns.node import Node, TCPDropIn, IPDropIn\n"
		"node = Node('AP1')\n"
		"dummy = Node('dummy')\n"
		"tcp = TCPDropIn(dummy)\n"
		"ip = IPDropIn(dummy)\n";

	wns::pyconfig::View pyco(pyconfig::helper::createViewFromString(config));

	pyconfig::View nodeConfig(pyco.get<wns::pyconfig::View>("node"));
	pyconfig::View tcpConfig(pyco.get<wns::pyconfig::View>("tcp"));
	pyconfig::View ipConfig(pyco.get<wns::pyconfig::View>("ip"));

	// begin example "Node::TCPIP.example"
	node::Interface* node = new Node(registry, nodeConfig);
	component::Interface* ci = new component::tests::ComponentStub(node, "component");
	component::tests::TCPInterface* tcp =
		new component::tests::TCP(ci, tcpConfig);
	component::tests::IPInterface* ip =
		new component::tests::IP(ci, ipConfig);

	node->addService(tcp->getName(), tcp);
	node->addService(ip->getName(), ip);
	// end example

	ip = node->getService<component::tests::IPInterface*>("IP");
	tcp = node->getService<component::tests::TCPInterface*>("TCP");

	CPPUNIT_ASSERT( ip->getAddress() == "127.0.0.1" );
	CPPUNIT_ASSERT_EQUAL( 31337, tcp->getFreePort() );
	delete node;
}

void NodeTest::nodeWithComponents()
{
	std::string nodeConfig =
		"from openwns.node import Node\n"
		"class ComponentA:\n"
		"    nameInComponentFactory='wns.node.tests.ComponentA'\n"
		"    name = 'componentA'\n"
		"class ComponentB:\n"
		"    nameInComponentFactory='wns.node.tests.ComponentB'\n"
		"    name = 'componentB'\n"
		"node = Node('AP1')\n"
		"componentA = ComponentA()\n"
		"componentB = ComponentB()\n"
		"node.addComponent(componentA)\n"
		"node.addComponent(componentB)\n";

	NodeTestObject node(
		registry,
		pyconfig::helper::createViewFromString(nodeConfig).get<pyconfig::View>("node"));

    node.startup();

	ComponentA* componentA = dynamic_cast<ComponentA*>(node.getComponent("componentA"));
	ComponentB* componentB = dynamic_cast<ComponentB*>(node.getComponent("componentB"));
	CPPUNIT_ASSERT( componentA != NULL );
	CPPUNIT_ASSERT( componentB != NULL );
	CPPUNIT_ASSERT_EQUAL( 1, componentA->getCalledStartup() );
	CPPUNIT_ASSERT_EQUAL( 1, componentB->getCalledStartup() );

	CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(2), node.numberOfComponents() );
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
