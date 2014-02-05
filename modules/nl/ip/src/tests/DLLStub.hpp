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

#ifndef IP_TESTS_DLLSTUB_HPP
#define IP_TESTS_DLLSTUB_HPP

#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/node/component/Component.hpp>

namespace ip  { namespace tests  {

	class DLLStub :
		public virtual wns::service::dll::UnicastDataTransmission
	{
	public:

		DLLStub() {};

		virtual
		~DLLStub() {};

		virtual void
		sendData(
			const wns::service::dll::UnicastAddress&,
			const wns::osi::PDUPtr&,
			wns::service::dll::protocolNumber /*protocol*/) {};

		virtual wns::service::dll::UnicastAddress
		getMACAddress() const { return wns::service::dll::UnicastAddress(); }
	};
} // tests
} // ip
#endif //IP_TESTS_DLLSTUB_HPP
