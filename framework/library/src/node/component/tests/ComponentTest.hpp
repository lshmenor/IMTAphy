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

#ifndef WNS_NODE_TEST_COMPONENT_COMPONENTTEST_HPP
#define WNS_NODE_TEST_COMPONENT_COMPONENTTEST_HPP

#include <WNS/node/component/Component.hpp>
#include <WNS/node/Registry.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace node { namespace component { namespace tests {
	class ComponentTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( ComponentTest );
		/** @todo msg: we need abstract tests here!! It doesn't make
// 		sense to test an interface ... */
// 		CPPUNIT_TEST( getName );
//  		CPPUNIT_TEST( findOtherComponents );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();
		void getName();
		void findOtherComponents();
	private:
		wns::node::Registry* registry;
		wns::node::Interface* node;
	};
} // tests
} // component
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
