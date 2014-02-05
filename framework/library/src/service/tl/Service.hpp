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

#ifndef WNS_SERVICE_TL_SERVICE_HPP
#define WNS_SERVICE_TL_SERVICE_HPP

#include <WNS/service/Service.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/qos/QoSClasses.hpp>

namespace wns { namespace service { namespace tl {

	class ConnectionHandler;
	class Connection;

	typedef long int Port;

	/**
	 * @brief tl service specification.
	 * @ingroup interface
	 */
	class Service:
		virtual public wns::service::Service
	{
	public:
		/**
		 * @brief Open passive connection and listen for incoming
		 * connections.
		 *
		 * @param[in] _port On which port to listen.
		 *
		 * @param[in] _ch ConnectionHandler to report incoming
		 * connections to.
		 */
		virtual void
		listenOnPort(
			Port _port,
			ConnectionHandler* _ch) = 0;

		/**
		 * @brief Open a new connection.
		 *
		 * @param[in] _port On which port to contact peer.
		 *
		 * @param[in] _peer On which address to contact peer.
		 *
		 * @param[in] _ch ConnectionHandler to report to.
		 */
		virtual void
		openConnection(
			Port _port,
			wns::service::nl::FQDN _source,
			wns::service::nl::FQDN _peer,
			wns::service::qos::QoSClass _qosClass,
			ConnectionHandler* _ch) = 0;

		/**
		 * @brief Close an opened connection.
		 *
		 * @param[in] _connection Connection to close.
		 */
		virtual void
		closeConnection(Connection* _connection) = 0;


	};
}
}
}



#endif // WNS_SERVICE_TL_SERVICE_HPP

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
