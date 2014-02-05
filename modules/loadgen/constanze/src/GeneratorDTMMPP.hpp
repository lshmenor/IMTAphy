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

#ifndef CONSTANZE_GENERATORCTMMPP_HPP
#define CONSTANZE_GENERATORCTMMPP_HPP

#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/SubGenerator.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/distribution/Distribution.hpp>
#include <WNS/markovchain/MarkovDiscreteTimeTraffic.hpp>

namespace constanze
{

	/**
	 * @brief DT-MMPP Packet Generator (Discrete Time Markov Modulated Poisson Process)
         * @author Rainer Schoenen (rs@comnets.rwth-aachen.de)
	 */
	class GeneratorDTMMPP :
		public GeneratorBase,
		public wns::markovchain::MarkovDiscreteTimeTraffic
	{
	public:

		/**
		 * @brief Constructor of a GeneratorDTMMPP
		 * @param[in] _pyco Configuration class of type Constanze.MMPP
		 * that describes the flow characteristics and destination.
		 */
		explicit GeneratorDTMMPP(const wns::pyconfig::View& _pyco);

		virtual	~GeneratorDTMMPP();

		/**
		 * @brief called by GeneratorBase::bindingReady()
		 */
		virtual void start();
		/**
		 * @brief called by GeneratorBase::bindingReleased()
		 */
		virtual void stop();

		/**
		 * @brief called by MarkovContinuousTime::nextState()
		 */
		virtual void stateChangeNotification(const int &chainNumber);

	private:

		/**
		 * @brief desired mean output rate. Set this to automatically determine rateScale
		 */
		double targetRate;

		/**
		 * @brief we have C subgenerators, one for each Markov chain
		 */
		std::vector<SubGenerator> subGenerators;

		/**
		 * @brief loggerName is modified for the subGenerators
		 */
		std::string loggerName;
	}; // GeneratorDTMMPP

} // constanze

#endif // NOT defined CONSTANZE_GENERATORCTMMPP_HPP


