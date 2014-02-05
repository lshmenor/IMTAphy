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

#ifndef WNS_NODE_TESTS_STUB_HPP
#define WNS_NODE_TESTS_STUB_HPP

#include <WNS/node/Interface.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>

namespace wns {
	namespace node { namespace component {
		class Interface;
	}}
	namespace service {
		class Service;
	}
}


namespace wns { namespace node { namespace tests {
	/**
	 * @brief Stub Node
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 *
	 * Minimal implementation of the Node interface. Will assert if any
	 * method except getProbeIDProviderRegistry() is called.
	 */
	class Stub :
		virtual public Interface
	{
	public:
		Stub();

		virtual
		~Stub();

		virtual void
		addService(const std::string&, service::Service*);

        virtual void
        startup();

		virtual void
		onWorldCreated();

		virtual void
		onShutdown();

		virtual probe::bus::ContextProviderCollection&
		getContextProviderCollection();

		virtual std::string
		getName() const;

        unsigned int
        getNodeID() const;

	private:
		virtual wns::service::Service*
		getAnyService(const component::FQSN& name) const;

		probe::bus::ContextProviderCollection contextProviderRegistry;

		unsigned int id;
		static unsigned int idCounter;
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
