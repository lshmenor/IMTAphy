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

#ifndef COPPER_TESTS_RECEIVERMOCK_HPP
#define COPPER_TESTS_RECEIVERMOCK_HPP

#include <COPPER/ReceiverInterface.hpp>
#include <COPPER/Transmission.hpp>

#include <WNS/service/dll/Address.hpp>
#include <WNS/osi/PDU.hpp>


namespace copper { namespace tests {

	class ReceiverMock :
		virtual public ReceiverInterface
	{
	public:
		ReceiverMock(const wns::service::dll::UnicastAddress& ua) :
			cOnCopperFree(0),
			cOnCopperBusy(0),
			pdu(),
			cOnCollision(0),
			collision(false),
			unicastAddress(ua)
		{}

		bool onData(const UnicastTransmissionPtr& _transmission)
		{
			bool forMe = _transmission->target == unicastAddress;
			if(forMe) {
				pdu = _transmission->pdu;
				collision = _transmission->collision;
			}
			return forMe;
		}

		bool onData(const BroadcastTransmissionPtr& _transmission)
		{
			pdu = _transmission->pdu;
			collision = _transmission->collision;
			return true;
		}

		void onCopperFree()
		{
			++cOnCopperFree;
		}

		void onCopperBusy()
		{
			++cOnCopperBusy;
		}

		void onCollision()
		{
			++cOnCollision;
		}

		int cOnCopperFree;
		int cOnCopperBusy;
		wns::osi::PDUPtr pdu;
		int cOnCollision;
		bool collision;
		wns::service::dll::UnicastAddress unicastAddress;
	};
} // tests
} // copper
#endif
