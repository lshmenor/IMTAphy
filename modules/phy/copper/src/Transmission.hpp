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

#ifndef COPPER_TRANSMISSION_HPP
#define COPPER_TRANSMISSION_HPP

#include <WNS/osi/PDU.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/service/dll/Address.hpp>

namespace copper
{
	class TransmitterDataSentInterface;


	struct Transmission :
		virtual public wns::RefCountable
	{
		Transmission(
			const wns::osi::PDUPtr& _pdu,
			TransmitterDataSentInterface* _sender);

		virtual
		~Transmission();

		wns::osi::PDUPtr pdu;
		bool collision;
		TransmitterDataSentInterface* sender;
	};


	struct BroadcastTransmission :
		public Transmission
	{
		BroadcastTransmission(
			const wns::osi::PDUPtr& _pdu,
			TransmitterDataSentInterface* sender);
	};


	struct UnicastTransmission :
		public Transmission
	{
		UnicastTransmission(
			const wns::service::dll::UnicastAddress& _target,
			const wns::osi::PDUPtr& _pdu,
			TransmitterDataSentInterface* sender);

		wns::service::dll::UnicastAddress target;
	};


	typedef wns::SmartPtr<Transmission> TransmissionPtr;
	typedef wns::SmartPtr<BroadcastTransmission> BroadcastTransmissionPtr;
	typedef wns::SmartPtr<UnicastTransmission> UnicastTransmissionPtr;
}

#endif // NOT defined COPPER_TRANSMISSION_HPP


