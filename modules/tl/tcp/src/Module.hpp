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

#ifndef TCP_MODULE_HPP
#define TCP_MODULE_HPP

#include <WNS/module/Module.hpp>
#include <WNS/logger/Logger.hpp>

namespace tcp
{
	class Module :
		public wns::module::Module<Module>
	{
	public:
        explicit
		Module(const wns::pyconfig::View& pyco);

		virtual
		~Module();

		// Module interface
		virtual void
		configure();

		virtual void
		startUp();

		virtual void
		shutDown();

	private:
		wns::logger::Logger log;
	};
} // namespace tcp

#endif // NOT defined TCP_MODULE_HPP
