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
#ifndef WNS_LDK_BOTTLENECKDETECTIVE_HPP
#define WNS_LDK_BOTTLENECKDETECTIVE_HPP

#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/pyconfig/View.hpp>

#include <map>

namespace wns { namespace ldk { namespace tools {

	class BottleNeckDetective;

	class BottleNeckDetectiveCommand :
		public Command
	{
	public:
		explicit
		BottleNeckDetectiveCommand();

		virtual
		~BottleNeckDetectiveCommand();

		struct {} local;
		struct {
		} peer;
		struct {
			BottleNeckDetective* sender;
			long long int id;
		} magic;
	};

	/**
	 * @brief Tries to find bottle necks in a FUN
	 */
	class BottleNeckDetective :
		virtual public FunctionalUnit,
	    virtual public Processor<BottleNeckDetective>,
		public wns::ldk::CommandTypeSpecifier<BottleNeckDetectiveCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<BottleNeckDetective>,
		public wns::events::PeriodicTimeout
	{
		typedef std::map<long int, CompoundPtr> CompoundContainer;
	public:
		explicit
		BottleNeckDetective(fun::FUN* fuNet, const wns::pyconfig::View& config);

		virtual
		~BottleNeckDetective();

 		//
 		//  interface
 		//
 		virtual void
		processIncoming(const CompoundPtr& compound);

 		virtual void
		processOutgoing(const CompoundPtr& compound);

		virtual void
		periodically();

	private:
		long long int id;
		CompoundContainer compounds;
		logger::Logger logger;
	}; // BottleNeckDetective

} // tools
} // ldk
} // wns


#endif // NOT defined WNS_LDK_BOTTLENECKDETECTIVE_HPP


