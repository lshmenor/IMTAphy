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

#ifndef WNS_LDK_TOOLS_GATE_HPP
#define WNS_LDK_TOOLS_GATE_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>



namespace wns { namespace ldk { namespace tools {

	/**
	 * @brief Public interface to configure a Gate.
	 *
	 */
	class GateInterface
	{
	public:
		typedef enum {OPEN, CLOSED} State;

		virtual void setIncomingState(State state) = 0;
		virtual void setOutgoingState(State state) = 0;

		virtual ~GateInterface() {};
	};


	/**
	 * @brief Start/stop accepting incoming/outgoing compounds.
	 *
	 * Control a Gate via the GateInterface.
	 */
	class Gate :
		public virtual FunctionalUnit,
		public CommandTypeSpecifier<>,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>,
		public Cloneable<Gate>,
		public virtual GateInterface
	{
	public:
		Gate(fun::FUN* fuNet, const wns::pyconfig::View& config);

		// GateInterface
		virtual void setIncomingState(State state);
		virtual void setOutgoingState(State state);

		virtual void doSendData(const CompoundPtr& compound);
		virtual void doOnData(const CompoundPtr& compound);

	private:
		// PDUHandler interface
		virtual bool doIsAccepting(const CompoundPtr& compound) const;
		virtual void doWakeup();

		logger::Logger logger;

		State incomingState;
		State outgoingState;
	};

}}}

#endif // NOT defined WNS_LDK_TOOLS_GATE_HPP


