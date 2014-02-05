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

#ifndef WNS_LDK_FLOWGATE_HPP
#define WNS_LDK_FLOWGATE_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/ldk/FlowSeparator.hpp>

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Key.hpp>

#include <WNS/logger/Logger.hpp>

#include <WNS/DerefLess.hpp>

namespace wns { namespace ldk {

	class FlowGateInterface
	{
	public:
		virtual
		~FlowGateInterface(){}

		virtual void
		createFlow(const ConstKeyPtr& key) = 0;

		virtual void
		destroyFlow(const ConstKeyPtr& key) = 0;

		virtual void
		openFlow(const ConstKeyPtr& key) = 0;

		virtual void
		closeFlow(const ConstKeyPtr& key) = 0;
	};

	class FlowGate :
		virtual public FunctionalUnit,
		virtual public FlowGateInterface,
		public CommandTypeSpecifier<>,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>,
		public Cloneable<FlowGate>
	{
 		typedef std::map<ConstKeyPtr, bool, DerefLess<ConstKeyPtr> > FlowStatus;
		FlowStatus flowStatus;

		KeyBuilder* keyBuilder;

		wns::logger::Logger logger;

		bool
		flowIsKnown(const ConstKeyPtr& key) const;

	public:
		/** @brief default FUN/Config Constructor */
		FlowGate(fun::FUN* fun, const wns::pyconfig::View& config);
		/** @brief Constructor for use by the unitTest */
		FlowGate(fun::FUN* fun, const wns::pyconfig::View& config, KeyBuilder* builder);

		virtual
		~FlowGate();

		virtual void
		onFUNCreated();

		/**
		 * @name CompoundHandlerInterface
		 *
		 * @brief The CompoundHandlerInterface methods (isAccepting,
		 * doSendData, doOnData, wakeup) are delegated to the matching
		 * instance.
		 *
		 * Instance retrieval is delegated to the internal
		 * implementation _getInstance.  If no matching instance is
		 * found, new instances may get instantiated on the fly.  In
		 * future this may depend on the FlowSeparator configuration.
		 */
		//@{
		virtual void doSendData(const CompoundPtr& compound);
		virtual void doOnData(const CompoundPtr& compound);
		virtual bool doIsAccepting(const CompoundPtr& compound) const;
		virtual void doWakeup();
		//@}


		/**
		 * @name FlowGateInterface
		 *
		 * @brief
		 *
		 * Description
		 */
		//@{
		virtual void
		createFlow(const ConstKeyPtr& key);

		virtual void
		destroyFlow(const ConstKeyPtr& key);

		virtual void
		openFlow(const ConstKeyPtr& key);

		virtual void
		closeFlow(const ConstKeyPtr& key);
		//@}


	};

}
}

#endif // note defined WNS_LDK_FLOWGATE_HPP


