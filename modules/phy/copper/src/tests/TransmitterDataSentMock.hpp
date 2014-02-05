/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
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

#ifndef COPPER_TESTS_TRANSMITTERDATASENTMOCK_HPP
#define COPPER_TESTS_TRANSMITTERDATASENTMOCK_HPP

#include <COPPER/Transmitter.hpp>
#include <WNS/osi/PDU.hpp>
#include <vector>

namespace copper { namespace tests {

	class TransmitterDataSentMock :
		virtual public TransmitterDataSentInterface

	{
	public:
		TransmitterDataSentMock() :
			pdus()
		{
		}

		virtual void
		onDataSent(wns::osi::PDUPtr pdu)
		{
			pdus.push_back(pdu);
		}

		std::vector<wns::osi::PDUPtr> pdus;
	};

} // tests
} // copper

#endif // NOT defined COPPER_TESTS_TRANSMITTERDATASENTMOCK_HPP
