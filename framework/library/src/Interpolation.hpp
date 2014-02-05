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

#ifndef WNS_INTERPOLATION_HPP
#define WNS_INTERPOLATION_HPP

#include <vector>
#include <WNS/container/Matrix.hpp>
#include <WNS/container/MultiAccessible.hpp>

namespace wns
{

	/**
	 *  @brief Contains definitions valid for all Interpolation-classes.
	 */
	namespace InterpolationDefs
	{
		/**
		 *  @brief Type used for coordinates.
		 */
		typedef double CoordType;

		/**
		 *  @brief Type used for sizes.
		 */
		typedef container::MultiAccessibleDefs::SizeType SizeType;
	}

	/**
	 *  @brief Interface and base class for @a N dimensional interpolation
	 *  over a container with type @a T values.
	 *
	 *  @param T Type of the values to interpolate over.
	 *  @param N Number of dimensions.
	 */
	template<typename T,
		 InterpolationDefs::SizeType N>
	class Interpolation
		: public container::MultiReadAccessible<T, N, T, InterpolationDefs::CoordType>
	{
		/**
		 *  @brief We inherit from this class.
		 */
		typedef container::MultiReadAccessible<T, N, T, InterpolationDefs::CoordType>
		SuperType;
	public:
		/**
		 *  @brief Type of sizes used in this class.
		 */
		typedef InterpolationDefs::SizeType SizeType;

		/**
		 *  @brief Type of the values we interpolate over.
		 */
		typedef T ValueType;

		/**
		 *  @brief Type of coordinates used in this class.
		 */
		typedef typename SuperType::IndexType CoordType;

		/**
		 *  @brief Type of the container that contains the discrete
		 *  (uninterpolated) values.
		 */
		typedef container::MultiReadAccessible<ValueType, N, ValueType> DiscreteContainer;

		/**
		 *  @brief Type to be used in constructor
		 */
		typedef container::Matrix<ValueType, N, ValueType> InitType;


		/**
		 *  @brief Constructs an Interpolation object over @a
		 *  discreteValues.
		 *
		 *  @param discreteValues (in) Container that contains the
		 *  values which we interpolate over.
		 */
		Interpolation(const DiscreteContainer& discreteValues)
			: SuperType(),
			  values(&discreteValues)
		{}

		/**
		 *  @brief Get a const reference to the container which we
		 *  interpolate over.
		 *
		 *  @return Constant reference to the container used when
		 *  constructing an instance of this class.
		 */
		const DiscreteContainer& discretes() const
		{
			return *values;
		}

		/**
		 *  @copydoc container::MultiReadAccessible::dimSize(const SizeType dim)
		 *  const
		 */
		virtual SizeType dimSize(const SizeType& dim) const
		{
			return values->dimSize(dim);
		}

	private:
		const DiscreteContainer* values;
	};

	/**
	 *  @brief One dimensional specialization for Interpolation
	 */
	template<typename T>
	class Interpolation<T, 1>
		: public container::MultiReadAccessible<T, 1, T, InterpolationDefs::CoordType>
	{
		typedef container::MultiReadAccessible<T, 1, T, InterpolationDefs::CoordType>
		SuperType;
	public:
		typedef InterpolationDefs::SizeType SizeType;
		typedef T ValueType;

		typedef typename SuperType::IndexType CoordType;

		typedef container::MultiReadAccessible<ValueType, 1, ValueType> DiscreteContainer;
		typedef container::Matrix<ValueType, 1, ValueType> InitType;

		Interpolation(const DiscreteContainer& discreteValues)
			: SuperType(),
			  values(&discreteValues)
		{}

		const DiscreteContainer& discretes() const
		{
			return *values;
		}

		virtual SizeType dimSize(const SizeType& dim) const
		{
			return values->dimSize(dim);
		}

	private:
		const DiscreteContainer* values;
	};

}

#endif // WNS_INTERPOLATION_HPP


