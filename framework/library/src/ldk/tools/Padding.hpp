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

#ifndef WNS_LDK_TOOLS_FORWARDER_HPP
#define WNS_LDK_TOOLS_FORWARDER_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Forwarding.hpp>

namespace wns { namespace ldk { namespace tools {

	/**
	 * @brief Pads Compoundss to a configurable size.
	 *
	 * Only Compounds with a size that doesn't exceed the configured size will be changed.
	 *
	 */
	class Padding :
		public CommandTypeSpecifier<>,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>,
		public Forwarding<Padding>,
		public Cloneable<Padding>
	{
	public:
		Padding(fun::FUN* fuNet, const pyconfig::View& config);
		virtual void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

		virtual void
		processOutgoing(const CompoundPtr& compound)
		{
			activateCommand(compound->getCommandPool());

			Forwarding<Padding>::processOutgoing(compound);
		} // processOutgoing

	private:
		int size;
	};
}}}

#endif // NOT defined WNS_LDK_TOOLS_FORWARDER_HPP



