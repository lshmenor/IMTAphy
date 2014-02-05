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

#ifndef WNS_LDK_TOOLS_OVERHEAD_HPP
#define WNS_LDK_TOOLS_OVERHEAD_HPP

#include <WNS/ldk/Forwarding.hpp>
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Command.hpp>

namespace wns { namespace ldk { namespace tools {

	class OverheadCommand :
		public Command
	{
	public:
		OverheadCommand();

		virtual Bit
		getSize() const;

		struct {
		} peer;

		struct {
		} local;

		struct {
			// Overhead in Bit
			Bit overhead;
		} magic;

	};

	/**
	 * @brief This FU lets you specify a fixed overhead at configuration
	 * time
	 */
	class Overhead :
		public fu::Plain<Overhead, OverheadCommand>,
		public Forwarding<Overhead>
	{
	public:
		/**
		 * @brief Constructor
		 */
		Overhead(fun::FUN* fuNet, const wns::pyconfig::View& config);

		// Forwarding Interface
		virtual void
		processOutgoing(const CompoundPtr& componud);

		virtual void
		processIncoming(const CompoundPtr& componud);

	private:

		Bit overhead;
	};

} // tools
} // ldk
} // wns

#endif // NOT defined WNS_LDK_TOOLS_OVERHEAD_HPP


