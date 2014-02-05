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

#ifndef COPPER_TRANSCEIVER_HPP
#define COPPER_TRANSCEIVER_HPP

#include <COPPER/Wire.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/logger/Logger.hpp>

namespace copper
{
	// Forward Declerations
	class Transmitter;
	class Receiver;

	/**
	 * @brief Combines the receiver and transmitter part
	 *
	 * This is the Copper Component
	 */
	class Transceiver :
		public wns::node::component::Component
	{
	public:
		/**
		 * @brief Constructor
		 *
		 * Creates Receiver and Transmitter and connects the Receiver to
		 * Wire provided in pyco.
		 */
		Transceiver(
			wns::node::Interface* node,
			const wns::pyconfig::View& pyco);

		/**
		 * @brief Destructor
		 *
		 * Deletes Transmitter and Receiver
		 *
		 * @todo Remove the Receiver from the Wire
		 */
		virtual
		~Transceiver();

		/**
		 * @name wns::node::component::Interface
		 */
		//{@
		virtual void
		onNodeCreated();

		virtual void
		onWorldCreated();

		virtual void
		onShutdown();
		//@}

	private:
		/**
		 * @brief Registers its services here
		 */
		virtual void
		doStartup();

		/**
		 * @brief Transmitter
		 */
		Transmitter* transmitter;

		/**
		 * @brief Receiver
		 */
		Receiver* receiver;

		/**
		 * @brief Logger
		 */
		wns::logger::Logger logger;

		/**
		 * @brief All Transceiver share a common WireBroker
		 */
		static WireBroker&
		getWireBroker();
	};
}

#endif // NOT defined COPPER_TRANSCEIVER_HPP


