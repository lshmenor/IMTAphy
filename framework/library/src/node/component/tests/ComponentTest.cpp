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

#include <WNS/node/component/tests/ComponentTest.hpp>
#include <WNS/node/Registry.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/node/component/tests/IP.hpp>
#include <WNS/node/component/tests/TCP.hpp>

using namespace wns::node::component::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( ComponentTest );

void ComponentTest::prepare()
{
	registry = new wns::node::Registry();
	node = new node::Node(
		registry,
		pyconfig::helper::createViewFromDropInConfig("wns.Node",
							     "NodeDropIn"));
}

void ComponentTest::cleanup()
{
	delete node;
	delete registry;
}

void ComponentTest::getName()
{
// 	std::string config =
// 		"from wns.Node import Node, TCPDropIn, IPDropIn\n"
// 		"dummy = Node('dummy')\n"
// 		"ip = IPDropIn(dummy)\n";

// 	wns::pyconfig::View pyco(pyconfig::helper::createViewFromString(config));

// 	IPInterface* ip = new IP(node, pyco.get<wns::pyconfig::View>("ip"));

// 	CPPUNIT_ASSERT( ip->getName() == "IP" );

// 	// not part of node, delete by hand
// 	delete ip;
}

void ComponentTest::findOtherComponents()
{
// 	delete node;

// 	std::string config =
// 		"from wns.Node import Node, TCPDropIn, IPDropIn\n"
// 		"node = Node('foobar')\n"
// 		"tcp = TCPDropIn(node)\n"
// 		"ip = IPDropIn(node)\n";

// 	wns::pyconfig::View pyco(pyconfig::helper::createViewFromString(config));

// 	node = new Node(registry, pyco.get<wns::pyconfig::View>("node"));

//  	component::tests::TCPInterface* tcp =
//  		node->getComponentByName<component::tests::TCPInterface*>("TCP");

//  	CPPUNIT_ASSERT(tcp->getAddressOfIPInstance() == "127.0.0.1");
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
