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

#ifndef WNS_TIMEWEIGHTEDAVERAGE_HPP
#define WNS_TIMEWEIGHTEDAVERAGE_HPP

#include <WNS/Exception.hpp>
#include <WNS/WeightedAverage.hpp>

namespace wns {
	/**
	 * @brief Build the time weighted average
	 * @ingroup statistics
	 */
	template <typename T,
		  typename Clock>
	class TimeWeightedAverage
		: public WeightedAverage<T>
	{
	public:
		/**
		 * @brief Default constructor, pointer to Clock is NULL!
		 */
		TimeWeightedAverage()
			: WeightedAverage<T>(),
			  lastTime(0.0),
			  c(NULL)
		{};

		/**
		 * @brief Takes a Clock as parameter
		 */
		TimeWeightedAverage(Clock* _c)
			: WeightedAverage<T>(),
			  lastTime(0.0),
			  c(_c)
		{};

		/**
		 * @brief Set the clock that should be used to compute the time
		 * differences between two put() calls.
		 */
		void setClock(Clock* _c)
		{
			c=_c;
		}

		/**
		 * @brief Reset the average
		 */
		virtual void reset()
		{
			assert(c);
			lastTime = c->getTime();
			WeightedAverage<T>::reset();
		};

		/**
		 * @brief Don't use this method when using TimeWeightedAverage
		 */
		virtual void put(const T&, double)
		{
			throw(Exception("This method should not be used!"));
		}

		/**
		 * @brief Add a sample to the average
		 */
		virtual void put(const T& t)
		{
			assert(c);
			double newTime = c->getTime();
			double weight = newTime - lastTime;
			WeightedAverage<T>::put(t, weight);
			lastTime = newTime;
		};

	private:
		/**
		 * @brief Last time a sample was added
		 */
		double lastTime;

		/**
		 * @brief The clock that is used for timing
		 */
		Clock* c;
	};
}

#endif // WNS_TIMEWEIGHTEDAVERAGE_HPP


