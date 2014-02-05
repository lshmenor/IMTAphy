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

#include "MarkovBase.hpp"
#include "MarkovDiscreteTime.hpp"
#include "TrafficSpec.hpp"
#include <WNS/logger/Logger.hpp>
#include <float.h>  // DBL_MAX

#ifndef WNS_MARKOVCHAIN_MARKOVDISCRETETIMETRAFFIC_HPP
#define WNS_MARKOVCHAIN_MARKOVDISCRETETIMETRAFFIC_HPP
namespace wns { namespace markovchain {

	/**
	 * @brief class MarkovDiscreteTimeTraffic
	 * @author Sara Gutierrez de Mesa(sgm@comnets.rwth-aachen.de)
	 * @author Rainer Schoenen (rs@comnets.rwth-aachen.de)
	 */

	class MarkovDiscreteTimeTraffic :
		public MarkovDiscreteTime<TrafficSpec>

	{
	public:
		/**
		 * @brief Creates an empty MarkovDiscreteTimeTraffic.
		 * numberOfChains is needed.
		 */
		MarkovDiscreteTimeTraffic(int _numberOfChains) :
			MarkovDiscreteTime<TrafficSpec>(_numberOfChains),
			rateScale(1.0),
			meanRate(0.0),
			minRate(DBL_MAX),
			maxRate(0.0)
		{
				MESSAGE_SINGLE(VERBOSE,logger, "MarkovDiscreteTimeTraffic() called");
		}

		/**
		 * @brief MarkovDiscreteTimeTraffic destructor
		 */
		~MarkovDiscreteTimeTraffic()
		{
		}


		/**
		 * @brief Reads the traffic specification from a stream.
		 */
		void
		readStatesFromFile (std::istream& in)
		{
			std::string line;
 			int stateCounter = 0;
			vectorOfStates = std::vector<TrafficSpec>(numberOfStates);

			while (in.good() && (stateCounter<numberOfStates)) {
				std::getline(in, line);
				int comment_pos=line.find_first_not_of(" \t");
				if (line[comment_pos] == '#') { // comment
				} else { // not a comment line

					MESSAGE_SINGLE(NORMAL,logger, "readStatesFromFile(#" << stateCounter <<"): \""<< line << "\"");
					std::istringstream instream(line);
					int stateNumber;
					std::string pdf;
					double arg0, arg1;
					//double meanRate = -1.0;
					instream >> stateNumber;
					instream >> pdf;
					instream >> arg0;
					instream >> arg1;
					//instream >> meanRate;
					TrafficSpec trafficSpec = TrafficSpec(pdf,arg0,arg1);
					vectorOfStates[stateNumber] = trafficSpec;
					stateCounter++;
				}
			}
		}

		/**
		 * @brief calculate mean, min, max rate
		 */
		virtual void
		calculateStateProbabilities()
		{
			// here the state probabilities are calculated:
			MarkovDiscreteTime<TrafficSpec>::calculateStateProbabilities();
			// in this class we have the traffic properties within the state
			// so we can calculate more...
			meanRate=0.0; maxRate=0.0; minRate=DBL_MAX;
			for(int i=0; i<numberOfStates; i++) {
				double p=stateProbabilities[i];
				const TrafficSpec* trafficSpec = getStateContent(i);
				//double meanIAT = trafficSpec->getInterArrivalTimeDistribution()->getMean();
				//double meanPKS = trafficSpec->getPacketSizeDistribution()->getMean();
				double stateMeanRate = rateScale * trafficSpec->getMeanRate();
				MESSAGE_SINGLE(NORMAL, logger, "state[" << i << "]: p=" << p << " rate=" << stateMeanRate);
				meanRate += p * stateMeanRate;
				if (stateMeanRate<minRate) minRate=stateMeanRate;
				if (stateMeanRate>maxRate) maxRate=stateMeanRate;
			}
			MESSAGE_BEGIN(NORMAL, logger, m, "traffic properties (rates in bits/s):\n");
			m << "mean=" << meanRate
			  << " min=" << minRate
			  << " max=" << maxRate;
			MESSAGE_END();
		}

		double getMeanRate() const
		{
			assure(minRate<DBL_MAX,"calculateStateProbabilities() not called before");
			return meanRate;
		}

		double getMinRate() const
		{
			assure(minRate<DBL_MAX,"calculateStateProbabilities() not called before");
			return minRate;
		}

		double getMaxRate() const
		{
			assure(minRate<DBL_MAX,"calculateStateProbabilities() not called before");
			return maxRate;
		}

	protected:
		/**
		 * @brief value to tune up (>1) or down (<1) the traffic rate
		 */
		double rateScale;
		// ^ must be known here to calculate the mean/min/max rate correctly
		// it is not used to scale the traffic here
		// because this is done in a SubGenerator
		/** @brief mean traffic rate of the aggregated process */
		double meanRate;
		/** @brief minimum traffic rate */
		double minRate;
		/** @brief maximum traffic rate */
		double maxRate;
	};

}//markovchain
}//wns
#endif //WNS_MARKOVCHAIN_MARKOVDISCRETETIMETRAFFIC_HPP

/**
 * @file MarkovDiscreteTimeTraffic.hpp
 */


