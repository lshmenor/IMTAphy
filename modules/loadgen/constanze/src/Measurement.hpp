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

#ifndef CONSTANZE_MEASUREMENT_HPP
#define CONSTANZE_MEASUREMENT_HPP

#include <CONSTANZE/BaumWelch.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <fstream>

namespace constanze
{
	class Measurement
        {
        public:

                Measurement(const wns::pyconfig::View& config);

                ~Measurement();

		/** @brief boudary of value rate between 2 states*/
		std::vector<double> *boundaryVector;

		/**
		 * @brief calculate parameter of HMM with baumwelch algorithm.
		 * numberOfStates: number of states
		 * observationVector: state sequence, that was observed
		 */
		HMM*
		calculate(int numberOfStates, std::vector<int> *observationVector);

		/** @brief output the element in a matrix, for unit test */
		void
		output(std::vector<std::vector<baumWelchDataType>*> *transitionsMatrix,int numberOfStates);

                /** @brief output for config */
		void output_python(std::vector<std::vector<baumWelchDataType>*> *transitionsMatrix,int numberOfStates,std::string estimationResultFileName);

		/**
		 * @brief interface to access estimatorNumberOfStates from
		 * exterior circumstance
		 */
		int getEstimatorNumberOfStates();

		/**
		 * @brief interface to access estimatorResultFileName from
		 * exterior circumstance
		 */
		std::string getMMPPestimationResultFileName();

        private:

		/** @brief Python view */
		wns::pyconfig::View pyco;

		/** @brief Logger */
		wns::logger::Logger log;

		/** @brief vector to store mean rate of every state*/
		std::vector<double> *meanRateVector;

                /**
                 * @brief number of states for estimation. Toggle on in Node.py
                 */
                int estimatorNumberOfStates;

                /**
                 * @brief windowed probes calculate rate values averaged over
		 * this time
                 */
                double probeWindow;

                /**
                 * @brief filename where MMPP estimation result is written into
                 */
		std::string MMPPestimationResultFileName;

		/** @brief a object of class BaumWelch, for the baum-welch calculation */
                BaumWelch* baumwelch;

        };
}

#endif // NOT defined CONSTANZE_MEASUREMENT_HPP

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
