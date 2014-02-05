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

#ifndef WNS_WEIGHTEDAVERAGE_HPP
#define WNS_WEIGHTEDAVERAGE_HPP

#include <WNS/Average.hpp>
#include <WNS/Assure.hpp>

namespace wns {
	/**
	 * @brief Build the weighted average
	 * @ingroup statistics
	 */
	template <typename C>
	class WeightedAverage
		: public Average<C>
	{
	public:
		/**
		 * @brief Default constructor
		 */
		WeightedAverage()
			: Average<C>(),
			  totalWeight(0.0)
		{};

		/**
		 * @brief Destructor
		 */
		virtual ~WeightedAverage()
		{};

		/**
		 * @brief Reset the average to 0.0
		 */
		virtual void reset()
		{
			totalWeight = 0.0;
			Average<C>::reset();
		};

		/**
		 * @brief Put a value and average (Special: weight is always 1)
		 */
		virtual void put(const C& c)
		{
			put(c, 1);
		};

		/**
		 * @brief Put a value and average
		 */
		virtual void put(const C& c, double w)
		{
			assure(w>=0, "Only values greater or equal to zero allowed");
			totalWeight += w;
			if(totalWeight > 0) {
				this->mean *= (1-w/totalWeight);
				this->mean += convertForAveraging(c)*w/totalWeight;
			}
		};

	private:
		/**
		 * @brief Sum of all weights
		 */
		double totalWeight;
	};
}

#endif // WNS_WEIGHTEDAVERAGE_HPP


