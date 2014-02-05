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

#ifndef WNS_NODE_TEST_NODETEST_HPP
#define WNS_NODE_TEST_NODETEST_HPP

#include <WNS/node/Registry.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/node/component/tests/ComponentStub.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace node { namespace tests {

	class ComponentA :
		public node::component::tests::ComponentStub
	{
	public:
		ComponentA(node::Interface* _node, const wns::pyconfig::View& _pyco) :
			node::component::tests::ComponentStub(_node, _pyco.get<std::string>("name"))
		{
		}

	};

	class ComponentB :
		public node::component::tests::ComponentStub
	{
	public:
		ComponentB(node::Interface* _node, const wns::pyconfig::View& _pyco) :
			node::component::tests::ComponentStub(_node, _pyco.get<std::string>("name"))
		{
		}
	};

	class NodeTest :
		public wns::TestFixture
	{
		class NodeTestObject :
			public node::Node
		{
		public:
			NodeTestObject(Registry* _registry, const pyconfig::View& _pyco) :
				Node(_registry, _pyco)
			{}

			size_t
			numberOfComponents() const
			{
				return localComponents.size();
			}

			node::component::Interface*
			getComponent(const std::string& name) const
			{
				for(ComponentContainer::const_iterator itr = localComponents.begin();
				    itr != localComponents.end();
				    ++itr)
				{
					if((*itr)->getName() == name)
					{
						return *itr;
					}
				}
				wns::Exception e;
				e << "Component with name " << name << "not found!";
				throw(e);
				return NULL;
			}
		};

		CPPUNIT_TEST_SUITE( NodeTest );
		CPPUNIT_TEST( getName );
		CPPUNIT_TEST( noDuplicateNodeIDs );
		CPPUNIT_TEST( addAndGetService );
		CPPUNIT_TEST( addAndGetTwoServices );
		CPPUNIT_TEST( nodeWithComponents );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();
		void getName();
		void noDuplicateNodeIDs();
		void addAndGetService();
		void addAndGetTwoServices();
		void nodeWithComponents();
	private:
		wns::node::Registry* registry;
	};
} // tests
} // node
} // wns
#endif

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
