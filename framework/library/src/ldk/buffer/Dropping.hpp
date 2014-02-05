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

#ifndef WNS_LDK_BUFFER_DROPPING_HPP
#define WNS_LDK_BUFFER_DROPPING_HPP

#include <WNS/ldk/buffer/Buffer.hpp>

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Delayed.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>

#include <WNS/logger/Logger.hpp>

#include <list>
#include <memory>

namespace wns { namespace ldk { namespace buffer {

	/**
	 * @brief Dropping buffer support classes.
	 *
	 */
	namespace dropping
	{
		typedef std::list<CompoundPtr> ContainerType;

		/**
		 * @brief Interface for packet dropping strategies.
		 *
		 */
		class Drop :
			public virtual CloneableInterface
		{
		public:
			typedef wns::Creator<Drop> Creator;
			typedef wns::StaticFactory<Creator> Factory;

			virtual CompoundPtr
			operator()(ContainerType& container) const = 0;

			virtual
			~Drop()
			{}
		};

		/**
		 * @brief Drop strategies for the Dropping buffer.
		 *
		 */
		namespace drop
		{
			/**
			 * @brief Drop compounds from the tail of the queue.
			 *
			 */
			class Tail :
				public Drop,
				public Cloneable<Tail>
			{
				virtual CompoundPtr
				operator()(ContainerType& container) const;
			};

			/**
			 * @brief Drop compounds from the front of the queue.
			 *
			 */
			class Front :
				public Drop,
				public Cloneable<Front>
			{
				virtual CompoundPtr
				operator()(ContainerType& container) const;
			};

			/* TODO:
			 * class Random...
			 */
		}
	}

	/**
	 * @brief Discarding buffer of a fixed size.
	 *
	 * FIFO buffer, discarding some compounds, when the buffer is
	 * full.
	 * The maximum size is given as number of compounds to store.
	 *
	 */
	class Dropping :
		public Buffer,
		public fu::Plain<Dropping>,
		public Delayed<Dropping>
	{
		typedef unsigned long int PDUCounter;

	public:

		/**
		 * @brief Constructor
		 */
		Dropping(fun::FUN* fuNet, const wns::pyconfig::View& config);

		/**
		 * @brief Copy Constructor
		 */
		Dropping(const Dropping& other);

		/**
		 * @brief Destructor
		 */
		virtual
		~Dropping();

		//
		// Delayed interface
		//
		virtual void
		processIncoming(const CompoundPtr& compound);

		virtual bool
		hasCapacity() const;

		virtual void
		processOutgoing(const CompoundPtr& compound);

		virtual const CompoundPtr
		hasSomethingToSend() const;

		virtual	CompoundPtr
		getSomethingToSend();

		//
		// Buffer interface
		//
		virtual unsigned long int
		getSize();

		virtual unsigned long int
		getMaxSize();

	protected:
		dropping::ContainerType buffer;

	private:
		unsigned long int maxSize;
		unsigned long int currentSize;

		std::auto_ptr<SizeCalculator> sizeCalculator;
		std::auto_ptr<dropping::Drop> dropper;

		PDUCounter totalPDUs;
		PDUCounter droppedPDUs;

		logger::Logger logger;
	};

}}}


#endif // NOT defined WNS_LDK_BUFFER_DROPPING_HPP


