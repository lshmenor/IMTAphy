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

#include <list>
#include <WNS/service/nl/Address.hpp>

#ifndef WNS_SERVICE_TL_CONNECTIONHANDLER_HPP
#define WNS_SERVICE_TL_CONNECTIONHANDLER_HPP

namespace wns { namespace service{namespace tl {
	class Connection;

	/**
	 * @brief tl ConnectionHandler interface.
	 *
	 * tl components need a callback to setup new connections. Users of
	 * @ref wns::node::component::tcp::Service provide such a callback by
	 * calling implementing the ConnectionHandler interface and passing an
	 * implementation to the Service interface during conneciton setup
	 * (either passive or active).
	 */
	class ConnectionHandler
	{
	public:
		/**
		 * @brief Destructor
		 */
		virtual
		~ConnectionHandler()
		{}

		/**
		 * @brief Called when a connection is successfully established. 
		 *
		 * As response to Service::openConnection or
		 * Service::listenOnPort when a connection is established by
		 * another component.
		 * @param[in] _sourceIP IP address of entity requesting the
		 * connection
		 *
		 * @param[in] _newConnection Connection that was created.
		 */
		virtual void
		onConnectionEstablished(wns::service::nl::Address _sourceIP, Connection* _newConnection) = 0;

		/**
		 * @brief Called when an existing connection was disconnected.
		 *
		 * @param[in] _disconnectedConnection The connection that was
		 * disconnected.
		 */
		virtual void
		onConnectionClosed(
			Connection* _disconnectedConnection) = 0;
		virtual void
		onConnectionClosedByPeer(
			Connection* _disconnectedConnection) = 0;

		/**
		 * @brief Called when a connection is lost, i.e. resulting from
		 * timeouts or error recovery.
		 *
		 * @param[in] _lostConnection Connection that is lost.
		 */
		virtual void
		onConnectionLost(Connection* _lostConnection) = 0;

	};

} // tl
} // service
} // wns


#endif // WNS_SERVICE_TL_CONNECTIONHANDLER_HPP

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
