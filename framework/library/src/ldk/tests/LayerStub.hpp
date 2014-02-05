/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#ifndef WNS_LDK_TESTS_LAYERSTUB_HPP
#define WNS_LDK_TESTS_LAYERSTUB_HPP

#include <WNS/ldk/Layer.hpp>
#include <WNS/node/tests/Stub.hpp>

namespace wns { namespace ldk { namespace tests {

	/**
	 * @brief Simple ILayer Stub for unit tests
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 *
	 * This LayerStub will automatically create a node::tests::Stub
	 */
	class LayerStub :
		public wns::ldk::ILayer
	{
	public:
        LayerStub();

		virtual
		~LayerStub();

		virtual std::string
		getName() const;

		virtual wns::node::Interface*
		getNode() const;

		virtual void
		onNodeCreated();

		virtual void
		onWorldCreated();

		virtual void
		onShutdown();

		virtual std::string
		getNodeName() const;

        virtual ControlServiceRegistry*
        getCSR() { return &csr; }

        virtual void
        addControlService(const std::string& name, ControlServiceInterface* csi) { csr.insert(name, csi); }

        virtual ManagementServiceRegistry*
        getMSR() { return &msr; }

        virtual void
        addManagementService(const std::string& name, ManagementServiceInterface* msi) { msr.insert(name, msi);}

        virtual ManagementServiceInterface*
        findManagementService(std::string name) const { return msr.find(name);}

        virtual ControlServiceInterface*
        findControlServiceInterface(std::string name) const {return csr.find(name);}

	private:
		virtual void
		doStartup();

		node::Interface* nodeStub;

        ControlServiceRegistry csr;
        ManagementServiceRegistry msr;
	};
} // tests
} // ldk
} // wns

#endif // NOT defined WNS_LDK_LAYER_HPP


