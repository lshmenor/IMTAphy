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

#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

#include <WNS/service/tl/Connection.hpp>
#include <WNS/service/tl/FlowID.hpp>

#include <TCP/FlowIDBuilder.hpp>

namespace tcp {

	class UpperConvergence;

	/**
	 * @brief The top most FU in a TCP FUN
	 */
	class Connection:
		virtual public wns::service::tl::Connection
	{
	public:
		Connection(const wns::service::tl::FlowID& _flowID,
			   UpperConvergence* _upperConvergence,
			   const wns::pyconfig::View& _config);

		// wns::service:.tl::Connection interface
		virtual void
		registerDataHandler(wns::service::tl::DataHandler *_dh);

		virtual void
		sendData(const wns::osi::PDUPtr &_pdu);

		wns::service::tl::FlowID
		getFlowID();

		/**
		 * @brief Deliver the retrieved data to the corresponding data
		 * handler
		 * @param[in] The data
		 */
		void
		onData(const wns::osi::PDUPtr& _pdu);

	private:
		/**
		 * @brief The connection's flow id
		 */
		wns::service::tl::FlowID flowID;

		/**
		 * @brief My logger
		 */
		wns::logger::Logger logger;

		/**
		 * @brief The DataHandler of the upper layer.
		 */
		wns::service::tl::DataHandler* dh;

		/**
		 * @brief The UpperConvergence of TCP's FUN
		 */
		UpperConvergence* upperConvergence;
	};
} // namespace tcp

#endif // NOT defined TCP_CONNECTION_HPP
