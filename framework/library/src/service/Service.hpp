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

#ifndef WNS_SERVICE_SERVICE_HPP
#define WNS_SERVICE_SERVICE_HPP

#include <WNS/node/component/FQSN.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/Object.hpp>
#include <string>

namespace wns { namespace service {
	/**
	 * @brief Every Service must derive from this class in order to be
	 * stored in a ServiceRegistry
	 +
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 */
	class Service :
		virtual public wns::Object
	{
	};

	template <typename HANDLER>
	class Notification :
		virtual public Service
	{
	public:
		virtual void
		registerHandler(HANDLER* handler) = 0;

	};
} // service
} // wns

#endif // WNS_SERVICE_SERVICE_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
