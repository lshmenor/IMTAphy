/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
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

#ifndef IP_IPMODULE_HPP
#define IP_IPMODULE_HPP

#include <WNS/module/Module.hpp>
#include <WNS/logger/Logger.hpp>

#include <list>

namespace ip
{
	namespace node { namespace component { namespace ipv4 { class Component;}}}

         /**
	 * @brief IP Module implementation.
	 *
	 * \pyco{ip.IP.IP}
	 */
	class IPModule :
		public wns::module::Module<IPModule>
	{
	public:
		IPModule(const wns::pyconfig::View& _pyConfigView);
		virtual ~IPModule();

		virtual void configure();
		virtual void startUp();
		virtual void shutDown();

	private:
		wns::logger::Logger log;
	};
}

#endif // NOT defined IP_IPMODULE_HPP


