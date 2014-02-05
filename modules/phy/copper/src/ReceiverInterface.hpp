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

#ifndef COPPER_RECEIVERINTERFACE_HPP
#define COPPER_RECEIVERINTERFACE_HPP

namespace copper {

	/**
	 * @brief Interface used by Wire to inform the Receiver
	 *
	 * This is interface is used by the wire in order to notify the Receiver
	 * of transmission events
	 */
	class ReceiverInterface
	{
	public:
		/**
		 * @brief Destructor
		 */
		virtual
		~ReceiverInterface()
		{}

		/**
		 * @brief Data (unicast) for this receiver arrived
		 *
		 * Each receiver implementation will need to check if the packet
		 * is good or bad. The current BER is also determined by the
		 * receiver. This leaves as much room for the
		 * implementation as possible.
		 */
		virtual bool
		onData(const UnicastTransmissionPtr& transmission) = 0;

		/**
		 * @brief Data (broadcast) for this receiver arrived
		 *
		 * Each receiver implementation will need to check if the packet
		 * is good or bad. The current BER is also determined by the
		 * receiver. This leaves as much room for the
		 * implementation as possible.
		 */
		virtual bool
		onData(const BroadcastTransmissionPtr& transmission) = 0;

		/**
		 * @brief Called, if the wire got free
		 */
		virtual void
		onCopperFree() = 0;

		/**
		 * @brief Called, if the wire got busy
		 */
		virtual void
		onCopperBusy() = 0;

		/**
		 * @brief Called, if a collision occured
		 */
		virtual void
		onCollision() = 0;
	};
} // copper

#endif // COPPER_RECEIVERINTERFACE_HPP


