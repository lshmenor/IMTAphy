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

#include <WNS/distribution/Distribution.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>


#ifndef WNS_MARKOVCHAIN_TRAFFICSPEC_HPP
#define WNS_MARKOVCHAIN_TRAFFICSPEC_HPP
namespace wns { namespace markovchain {

	/**
	 * @brief Class to represent the type of a Markov state,
	 * when state should describe the traffic to be generated when we are within the state
	 * @author Sara Gutierrez de Mesa (sgm@comnets.rwth-aachen.de)
	 * @author Rainer Schoenen (rs@comnets.rwth-aachen.de)
	 */

	class TrafficSpec {
	public:
		/**
		 * @brief creates an empty TrafficSpec
		 */
		TrafficSpec()
		{
		}

		/**
		 * @brief creates a TrafficSpec with explicit distribution for a point process.
		 * _iatpdf: interarrival distribution.
		 * _arg0, _arg1: arguments for the distributions
		 */
		TrafficSpec(std::string _iatpdf,
			    double _arg0,
			    double _arg1) :
			interArrivalTimeDistribution(NULL),
			packetSizeDistribution(NULL),
			meanRate(0.0)
		{
			wns::pyconfig::Parser config;
			std::string iatDistName = convertGdfDistributionName2wns(_iatpdf, _arg0, _arg1);
			config.loadString(
				"import openwns.distribution\n"
				"packetSize = openwns.distribution.Fixed(1024)\n" // there's no better way now
				"iat = openwns.distribution." + iatDistName + "\n"
				);
				initializeGenerators(config);
		}

		/**
		 * @brief creates a TrafficSpec with given python config
		 */
		TrafficSpec(const wns::pyconfig::View& pyco) :
			interArrivalTimeDistribution(NULL),
			packetSizeDistribution(NULL),
			meanRate(0.0)
		{
			initializeGenerators(pyco);
		}

		/**
		 * @brief TrafficSpec destructor
		 */
		~TrafficSpec()
		{
		}

		/**
		 * @brief getInterArrivalTimeDistribution
		 */
		wns::distribution::Distribution*
		getInterArrivalTimeDistribution() const
		{
			return interArrivalTimeDistribution;
		}

		/**
		 * @brief getPacketSizeDistribution
		 */
		wns::distribution::Distribution*
		getPacketSizeDistribution() const
		{
			return packetSizeDistribution;
		}

		/** @brief returns average traffic rate (unscaled) */
		double getMeanRate() const
		{
			return meanRate; // without scaling
		}

	private:

		/**
		 * @brief stringify: converts a double to a string
		 */
		std::string
		stringify(double x) const
		{
			std::ostringstream o;
			o << x;
			return o.str();
		}

		/**
		 * @brief convert distribution names from the gdf file (Opnet name)
		 * to wns distribution names in Python format.
		 * 1-2 arguments are handled.
		 */
		std::string
		convertGdfDistributionName2wns(std::string _pdf, double _arg0, double _arg1) const
		{
			std::string DistName;
			if (_pdf == "constant") {
				DistName = "Fixed(" + stringify(_arg0) + ")";
			} else if (_pdf == "exponential") {
				DistName = "NegExp(" + stringify(_arg0) +")";
			} else if (_pdf == "uniform") {
				DistName = "Uniform(" + stringify(_arg0) + "," + stringify(_arg1) +")";
			} else { // WNS name allowed in gdf file
				DistName = _pdf + "(" + stringify(_arg0) + ")";
			}
			return DistName;
		}

		friend class MarkovContinuousTimeTrafficTest;
		friend class MarkovDiscreteTimeTrafficTest;

		/**
		 * @brief initializeGenerators: creates
		 * interArrivalTimeDistribution generator and
		 * packetSizeDistribution generator from Python config
		 */
		void
		initializeGenerators(const wns::pyconfig::View& pyco)
		{
			wns::pyconfig::View iatConfig(pyco, "iat");
			std::string iatName = iatConfig.get<std::string>("__plugin__");
			interArrivalTimeDistribution =
				wns::distribution::DistributionFactory::creator(iatName)->create(iatConfig);
			wns::pyconfig::View packetConfig(pyco, "packetSize");
			std::string psName = packetConfig.get<std::string>("__plugin__");
			packetSizeDistribution =
				wns::distribution::DistributionFactory::creator(psName)->create(packetConfig);
			//std::cout << "iat=" << interArrivalTimeDistribution->paramString()
			//	  << ", packetSize=" <<
			//	  packetSizeDistribution->paramString() << std::endl;
			double meanIAT;
            meanIAT = 
                dynamic_cast<wns::distribution::IHasMean*>(interArrivalTimeDistribution)
                    ->getMean();
			assure(meanIAT>0.0,"zero meanIAT");
			meanRate =  
                dynamic_cast<wns::distribution::IHasMean*>(packetSizeDistribution)
                    ->getMean()/meanIAT;
		}

	protected:
		/** @brief Random number generator for the inter arrival time distribution. */
		wns::distribution::Distribution* interArrivalTimeDistribution;
		/** @brief Random number generator for the packet size distribution. */
		wns::distribution::Distribution* packetSizeDistribution;
		/** @brief average traffic rate (unscaled) */
		double meanRate; // without scaling
	}; // class TrafficSpec

}//markovchain
}//wns
#endif // WNS_MARKOVCHAIN_TRAFFICSPEC_HPP


/**
 * @file
 */


