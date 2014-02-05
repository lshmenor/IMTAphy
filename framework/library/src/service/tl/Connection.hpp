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

#ifndef WNS_SERVICE_TL_CONNECTION_HPP
#define WNS_SERVICE_TL_CONNECTION_HPP

#include <WNS/osi/PDU.hpp>

namespace wns { namespace service { namespace tl {

	class DataHandler;


	/**
	 * @brief tl Connection interface.
	 *
	 * Connection is used to send data and dispatch incoming data to a
	 * DataHandler.
	 */

	class Connection
	{
	public:

		/**
		 * @brief Destructor.
		 */
		virtual
		~Connection()
		{}

		/**
		 * @brief Register DataHandler for incoming data.
		 *
		 * @param[in] _dh DataHandler were incoming data should be
		 * delivered to.
		 */
		virtual void
		registerDataHandler(wns::service::tl::DataHandler* _dh) = 0;

		/**
		 * @brief Send Data.
		 * @param[in] _pdu Payload to be sent.
		 */
		virtual void
		sendData(const wns::osi::PDUPtr& _pdu) =0;

	};

} // tl
} // service
} // wns


#endif // WNS_SERVICE_UDP_CONNECTION_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
