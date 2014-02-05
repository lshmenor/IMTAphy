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

#ifndef WNS_LDK_TOOLS_SYNCHRONIZER_HPP
#define WNS_LDK_TOOLS_SYNCHRONIZER_HPP

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Delayed.hpp>

#include <WNS/pyconfig/View.hpp>

namespace wns { namespace ldk { namespace tools {

	/**
	 * @brief A one-packet-delay
	 *
	 * Use the Synchronizer to break up long isAccepting-chains.
	 * This is necessary in some situations, where lower isAccepting decisions
	 * depend on Commands that have not been activated so far. Trying to access
	 * these would fail.
	 * <br>
	 * U will usually have groups of functional units implementing functional blocks
	 * (for example RLC as a group consisting of functional units like an ARQ, ...).
	 * Consider separating these groups by a Synchronizer.
	 * <br>
	 * @todo bad documentation
	 */
	class Synchronizer :
		public CommandTypeSpecifier<>,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>,
		public Delayed<Synchronizer>,
		public Cloneable<Synchronizer>
	{
	public:
		Synchronizer(fun::FUN* fuNet, const pyconfig::View& _config) :
				CommandTypeSpecifier<>(fuNet),
				HasReceptor<>(),
				HasConnector<>(),
				HasDeliverer<>(),
				Delayed<Synchronizer>(),
				Cloneable<Synchronizer>(),

				ind(0),
				req(0),
				config(_config),
				buffer()
		{}

		// Delayed interface
		virtual void processIncoming(const CompoundPtr& compound);
		virtual void processOutgoing(const CompoundPtr&);
		virtual bool hasCapacity() const;
		virtual const CompoundPtr hasSomethingToSend() const;
		virtual CompoundPtr getSomethingToSend();

		int ind;
		int req;

	private:
		pyconfig::View config;
		CompoundPtr buffer;
	};
}}}


#endif // NOT defined WNS_LDK_TOOLS_SYNCHRONIZER_HPP


