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
#ifndef CONSTANZE_GENERATORPP_HPP
#define CONSTANZE_GENERATORPP_HPP

#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/ConstanzePDU.hpp>
#include <WNS/events/CanTimeout.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/distribution/Distribution.hpp>

namespace constanze
{

	/**
	 * @brief Simple Packet Generator (Point Process)
         * @author Rainer Schoenen (rs@comnets.rwth-aachen.de)
	 */
	class GeneratorPP :
		public GeneratorBase,
		protected wns::events::CanTimeout
	{
	public:

		/**
		 * @brief Constructor of a GeneratorPP
		 * @param[in] _pyco Configuration class of type Constanze.CBR()
		 * that describes the flow characteristics and destination.
		 */
		explicit GeneratorPP(const wns::pyconfig::View& _pyco);

		virtual	~GeneratorPP();

		virtual void start();
		virtual void stop();

	private:
		/**
		 * @brief Periodically executed to generate packets.
		 */
		virtual void onTimeout();

		/**
		 * @brief Inter Arrival Time distribution.
		 */
		wns::distribution::Distribution* interArrivalTimeDistribution;

		/**
		 * @brief Packet Size distribution.
		 */
		wns::distribution::Distribution* packetSizeDistribution;

		/**
		 * @brief use statistically correct ForwardRecurrenceTime if true
		 */
		bool correctFRT;

	}; // GeneratorPP

} // constanze

#endif // NOT defined CONSTANZE_GENERATORPP_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
