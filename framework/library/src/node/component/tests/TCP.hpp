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

#ifndef WNS_NODE_COMPONENT_TESTS_TCP_HPP
#define WNS_NODE_COMPONENT_TESTS_TCP_HPP

#include <WNS/node/Interface.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/service/Service.hpp>
#include <WNS/node/component/tests/IP.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace node { namespace component { namespace tests {

	// begin example "Component::TCPInterface.example"
	// Define a new Interface
	class TCPInterface :
		virtual public service::Service
	{
	public:
		virtual int
		getFreePort() = 0;

		virtual std::string
		getAddressOfIPInstance() = 0;

		virtual std::string
		getName() = 0;
	};
	// end example

	// begin example "Component::TCP.example"
	// Implementation of TCPInterface
	class TCP :
		virtual public TCPInterface
	{
	public:
		TCP(component::Interface* ci, const pyconfig::View& _pyco) :
			pyco(_pyco),
			ip(NULL),
			componentInterface(ci),
			name(_pyco.get<std::string>("name"))
		{
		}

		// TCPInterface
		int getFreePort()
		{
			// Return always 31337 (only for tests)
			return 31337;
		}

		// TCPInterface
		std::string
		getAddressOfIPInstance()
		{
			return ip->getAddress();
		}

		// ComponentInterface
		void onNodeCreated()
		{
			ip = componentInterface->getService<IPInterface*>(pyco.get<std::string>("ipInstance"));
		}

		void onWorldCreated()
		{
			// No inter-node dependecies.
		}

		virtual std::string
		getName()
		{
			return name;
		}

	private:
		wns::pyconfig::View pyco;
		// The IP instance below this TCP instance
		IPInterface* ip;
		component::Interface* componentInterface;
		std::string name;
	};
	// end example
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
