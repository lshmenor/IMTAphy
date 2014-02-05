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

#ifndef WNS_LDK_BUFFER_HPP
#define WNS_LDK_BUFFER_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/FUNConfigCreator.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/SlidingWindow.hpp>
#include <WNS/events/PeriodicTimeout.hpp>

namespace wns { namespace ldk { namespace buffer {

	/**
	 * @brief Interface for size calculation strategies.
	 *
	 */
	struct SizeCalculator :
		public virtual CloneableInterface
	{
		typedef wns::Creator<SizeCalculator> Creator;
		typedef wns::StaticFactory<Creator> Factory;

		virtual unsigned long int
		operator()(const CompoundPtr& compound) const = 0;

		virtual
		~SizeCalculator()
		{}
	};

	/**
	 * @brief Size calculation stategies for the Dropping buffer
	 *
	 */
	namespace sizecalculators
	{
		struct PerPDU :
			public SizeCalculator,
			public Cloneable<PerPDU>
		{
			virtual unsigned long int
			operator()(const CompoundPtr& compound) const;
		};

		struct PerBit :
			public SizeCalculator,
			public Cloneable<PerBit>
		{
			virtual unsigned long int
			operator()(const CompoundPtr& compound) const;
		};
	}

	/**
	 * @brief functional units with the only purpose to buffer.
	 *
	 */
	class Buffer :
		virtual public FunctionalUnit,
		public events::PeriodicTimeout
	{
		typedef unsigned long int PDUCounter;

	public:
		Buffer(fun::FUN* fuNet, const pyconfig::View& config);

		virtual
		~Buffer();

		virtual void
		onFunCreated();

		virtual unsigned long int
		getSize() = 0;

		virtual unsigned long int
		getMaxSize() = 0;

	protected:
		void
		increaseTotalPDUs();

		// Size may be Bit or number of PDUs (depends on size
		// calculation strategy)
		void
		increaseDroppedPDUs(int size);

		void
		probe();

		virtual void
		periodically();

	private:
		wns::probe::bus::ContextCollectorPtr lossRatioProbeBus;
		wns::probe::bus::ContextCollectorPtr sizeProbeBus;

		PDUCounter totalPDUs;
		PDUCounter droppedPDUs;
		/** @brief window sampled every probeDroppedPDUInterval seconds */
		SlidingWindow droppedPDUWindow;
		/** @brief sampling time for periodically() */
		simTimeType probeDroppedPDUInterval;

		wns::logger::Logger logger;
		bool probingEnabled;
	};

	typedef FUNConfigCreator<Buffer> BufferCreator;
	typedef wns::StaticFactory<BufferCreator> BufferFactory;
}}}


#endif // NOT defined WNS_LDK_BUFFER_HPP


