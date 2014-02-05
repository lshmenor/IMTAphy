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

#ifndef WNS_LDK_BUFFER_BOUNDED_HPP
#define WNS_LDK_BUFFER_BOUNDED_HPP

#include <WNS/ldk/buffer/Buffer.hpp>
#include <WNS/ldk/fu/Plain.hpp>

#include <WNS/pyconfig/View.hpp>

#include <list>

namespace wns { namespace ldk { namespace buffer {

	/**
	 * @brief Fixed size buffering of compounds.
	 *
	 */
	class Bounded :
		public Buffer,
		public fu::Plain<Bounded>
	{
		typedef std::list<CompoundPtr> ContainerType;
	public:

		Bounded(fun::FUN* fuNet, const wns::pyconfig::View& config);
		virtual ~Bounded();

		virtual void doSendData(const CompoundPtr& sdu);
		virtual void doOnData(const CompoundPtr& compound);

		//
		// Buffer interface
		//
		virtual unsigned long int getSize();
		virtual unsigned long int getMaxSize();

	protected:
		void tryToSend();
		bool tryToSendOnce();

	private:
		//
		// CompoundHandlerInterface
		//
		virtual bool doIsAccepting(const CompoundPtr& compound) const;
		virtual void doWakeup();

		ContainerType buffer;
		unsigned long int maxSize;
		unsigned long int currentSize;
		SizeCalculator* sizeCalculator;
		bool inWakeup;
	};

}}}


#endif // NOT defined WNS_LDK_BUFFER_BOUNDED_HPP


