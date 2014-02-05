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


#ifndef _NEARESTNEIGHBOUR_HPP
#define _NEARESTNEIGHBOUR_HPP

#include <cmath>
#include <WNS/Interpolation.hpp>

namespace wns
{
	/**
	 *  @brief N-dimensional Nearest Neighbour Interpolation.
	 *
	 *  @param T Type of the values to interpolate over.
	 *  @param N Number of dimensions.
	 *
	 *  Nearest Neighbour Interpolation is basically no interpolation at all.
	 *  For a given floating point coordinate it selects the nearest integer
	 *  coordinate.  Say nn is a NearestNeighbour<@a T, 1> object and oc is the
	 *  MultiReadAccessible<@a T, 1> we interpolate over, then nn[2.4] == oc[2]
	 *  and nn[2.5] == oc[3].
	 */
	template<typename T,
		 InterpolationDefs::SizeType N>
	class NearestNeighbour :
		public Interpolation<T, N>
	{
		/**
		 *  @brief We inherit from this class.
		 */
		typedef Interpolation<T, N> SuperType;
	public:
		/**
		 *  @brief Type of sizes used in this class.
		 */
		typedef typename SuperType::SizeType SizeType;

		/**
		 *  @brief Type of coordinates used in this class.
		 */
		typedef typename SuperType::CoordType CoordType;

		/**
		 *  @brief Type of the container that contains the discrete
		 *  (uninterpolated) values.
		 */
		typedef typename SuperType::DiscreteContainer DiscreteContainer;

		/**
		 *  @brief Type of the hyperplanes.
		 */
		typedef NearestNeighbour<T, N-1> HyperplaneType;

		/**
		 *  @brief Constant reference to hyperplanes.
		 */
		typedef const HyperplaneType& ConstReference;

		/**
		 *  @brief Construct a NearestNeighbour interpolation object
		 *  that interpolates over @a discreteValues.
		 *
		 *  @param discreteValues (in) Container that contains the
		 *  values which we interpolate over.
		 */
		NearestNeighbour(const DiscreteContainer& discreteValues)
			: SuperType(discreteValues),
			  hyperplanes(discreteValues.dimSize(0), HyperplaneType(discreteValues[0]))
		{
			for(SizeType i = 0; i < hyperplanes.size(); ++i) {
				hyperplanes[i] = HyperplaneType(discreteValues[i]);
			}
		}

		/**
		 *  @brief Select the nearest neighbour of coord.
		 *
		 *  Returns a const reference to the hyperplane specified by @a
		 *  coord.
		 */
		virtual ConstReference operator[](const CoordType& coord) const
		{
			typedef typename DiscreteContainer::IndexType IndexType;
			return hyperplanes[(IndexType)std::floor(coord+0.5)];
		}

	private:
		typedef std::vector<HyperplaneType> HyperplaneContainer;

		HyperplaneContainer hyperplanes;
	};

	/**
	 *  @brief One dimensional specialization of Nearest Neighbour
	 *  Interpolation.
	 */
	template<typename T>
	class NearestNeighbour<T, 1> :
		public Interpolation<T, 1>
	{
		typedef Interpolation<T, 1> SuperType;
	public:
		typedef typename SuperType::ValueType ValueType;
		typedef typename SuperType::CoordType CoordType;
		typedef typename SuperType::DiscreteContainer DiscreteContainer;
		typedef typename DiscreteContainer::IndexType IndexType;

		NearestNeighbour(const DiscreteContainer& discreteValues)
			: SuperType(discreteValues)
		{}

		virtual ValueType operator[](const CoordType& coord) const
		{
			return Interpolation<T,1>::discretes()[(IndexType)std::floor(coord+0.5)];
		}

	};
}

#endif // _NEARESTNEIGHBOUR_HPP

/**
 * @file
 */


