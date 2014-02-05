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

#ifndef WNS_NODE_COMPONENT_TESTS_IP_HPP
#define WNS_NODE_COMPONENT_TESTS_IP_HPP

#include <WNS/node/Interface.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/service/Service.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace node { namespace component { namespace tests {

	class IPInterface :
		virtual public service::Service
	{
	public:
		virtual std::string
		getAddress() const = 0;

		virtual std::string
		getName() = 0;

	};

	class IP :
		virtual public IPInterface
	{
	public:
		IP(component::Interface* ci, const pyconfig::View& pyco) :
			componentInterface(ci),
			name(pyco.get<std::string>("name"))
		{
		}

		virtual std::string
		getAddress() const
		{
			return "127.0.0.1";
		};

		virtual void
		onNodeCreated()
		{
			// We don't need any other Components
		}

		virtual void
		onWorldCreated()
		{
			// We don't need any other Components
		}

		virtual std::string
		getName()
		{
			return name;
		}
	private:
		component::Interface* componentInterface;
		std::string name;
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
