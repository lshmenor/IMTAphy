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

#ifndef CONSTANZE_SUBGENERATOR_HPP
#define CONSTANZE_SUBGENERATOR_HPP

#include <CONSTANZE/Generator.hpp>
#include <WNS/events/CanTimeout.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/distribution/Distribution.hpp>

namespace constanze
{

	/**
	 * @brief Simple Packet Generator (Point Process) used within GeneratorMMPP.
	 * This generator can be reconfigured during runtime.
	 * An MMPP generator has C of them (C=numberOfChains)
         * @author Rainer Schoenen (rs@comnets.rwth-aachen.de)
	 */
	class SubGenerator :
		protected wns::events::CanTimeout
	{
	public:

		/**
		 * @brief Constructor of a SubGenerator using Python parameters
		 * @param[in] _pyco Configuration class of type Constanze.CBR()
		 */
		explicit SubGenerator(constanze::GeneratorBase* _master, const wns::pyconfig::View& _pyco);

		/**
		 * @brief Constructor of a SubGenerator using Distributions
		 */
		explicit SubGenerator(constanze::GeneratorBase* _master, wns::logger::Logger& _log, wns::distribution::Distribution* _interArrivalTimeDistribution, wns::distribution::Distribution* _packetSizeDistribution, double _rateScale);

		virtual	~SubGenerator();

		/**
		 * @brief customize logger (name). Useful for an array of subGenerators
		 */
		virtual void reconfigLogger(wns::logger::Logger& _log);

		/**
		 * @brief change distributions during runtime
		 * @param[in] distributions for interarrival time and packet size
		 * reconfig automatically starts events.
		 */
		virtual void reconfig(wns::distribution::Distribution* _interArrivalTimeDistribution, wns::distribution::Distribution* _packetSizeDistribution);

		/**
		 * @brief start traffic generation immediately (now)
		 */
		virtual void start();

		/**
		 * @brief stop traffic generation (cancel timeouts)
		 */
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
		 * @brief Logger instance.
		 */
		wns::logger::Logger log;

		/**
		 * @brief The master generator that contains us subGenerators
		 */
		constanze::GeneratorBase* master;

		/**
		 * @brief Binding for plugging the Generator on top of any layer,
		 * eg., UDP or IP.
		 */
		//constanze::Binding* binding;

		/**
		 * @brief value to tune up (>1) or down (<1) the traffic rate
		 */
		double rateScale;

	}; // SubGenerator

} // constanze

#endif // NOT defined CONSTANZE_SUBGENERATOR_HPP


