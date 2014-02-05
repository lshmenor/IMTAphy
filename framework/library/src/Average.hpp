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

#ifndef WNS_AVERAGE_HPP
#define WNS_AVERAGE_HPP

#include <stdint.h>

namespace wns {
	/**
	 * @brief Base class to compute the average
	 * @ingroup statistics
	 */
	class AverageBase
	{
	public:
		/**
		 * @brief Default Constructor
		 */
		AverageBase() :
			mean(0.0),
			samples(0)
		{
			
		}

		/**
		 * @brief Destructor
		 */
		virtual
		~AverageBase()
		{
			
		}

		/**
		 * @brief Reset the mean to 0.0
		 */
		virtual void
		reset()
		{
			mean = 0.0;
			samples = 0;
			
		}

	protected:
		/**
		 * @brief Stores the average
		 */
		double mean;

		/**
		 * @brief The number of samples that have been used to compute
		 * the mean
		 */
		unsigned long int samples;
	}; // AverageBase

	/**
	 * @brief Computes the average of a number of samples
	 * @ingroup statistics
	 */
	template <typename C>
	class Average :
		public AverageBase
	{
	public:
		/**
		 * @brief Default constructor
		 */
		Average() :
			AverageBase()
		{
			
		}

		/**
		 * @brief Destructor
		 */
		virtual
		~Average()
		{
			
		}

		/**
		 * @brief Add a sample
		 */
		virtual void
		put(const C& c)
		{
			mean *= 1-1.0/++samples;
			mean += convertForAveraging(c)/samples;
			
		}

		/**
		 * @brief Returns the current average
		 */
		virtual C
		get() const
		{
			
			return convertForAveraged(mean);
		}

	protected:
		/**
		 * @brief Supports averaging of other types like Power or Ratio
		 *
		 * @note To support the averaging of other types these types
		 * need to provide a method convertForAveraged which is called
		 * when the average is requested via the get() method.
		 */
		C
		convertForAveraged(double result) const
		{
			
			return C::convertForAveraged(result);
		}

		/**
		 * @brief Supports averaging of other values like Power or Ratio
		 *
		 * @note To support the averaging of other types these types
		 * need to provide a method convertForAveraging which is called
		 * when a value is added to the average via the put() method.
		 */
		double
		convertForAveraging(const C& c) const
		{
			
			return c.convertForAveraging();
		}
	}; // Average<C>

	/**
	 * @brief Compute the average for double values
	 * @ingroup statistics
	 */
	template <>
	class Average<double> :
		public AverageBase
	{
	public:
		/**
		 * @brief Special version of Average::put(const C& c) for
		 * doubles
		 */
		virtual void
		put(const double& c)
		{
			mean *= 1-1.0/++samples;
			mean += convertForAveraging(c)/samples;
			
		}

		/**
		 * @brief Special version of Average::get() for doubles
		 */
		virtual double
		get() const
		{
			
			return mean;
		}

	protected:
		/**
		 * @brief Special version of Average::convertForAveraged(double result)
		 * for doubles
		 */
		double
		convertForAveraged(double result)
		{
			
			return result;
		}

		/**
		 * @brief Special version of Average::convertForAveraging(const C& c)
		 * for doubles
		 */
		double
		convertForAveraging(const double& c)
		{
			
			return c;
		}
	}; // Average<double>

} // wns

#endif // NOT defined WNS_AVERAGE_HPP


/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
