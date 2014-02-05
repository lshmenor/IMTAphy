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

#ifndef _NLINEAR_HPP
#define _NLINEAR_HPP

#include <cmath>
#include <list>
#include <WNS/Interpolation.hpp>

namespace wns
{
	/**
	 *  @brief N-linear interpolation (linear, bilinear, trilinear...).
	 *
	 *  @param T Type of the values to interpolate over.
	 *  @param N Number of dimensions.
	 *
	 *  This class implements a generic n-linear interpolation, so
	 *  NLinear<@a T, 1> is a linear, NLinear<@a T, 2> is a bilinear and
	 *  NLinear<@a T, 3> is a trilinear interpolation.
	 *
	 *  Linear interpolation can easily be extended to multiple dimensions,
	 *  which will be demonstrated by explaining how bilinear interpolation
	 *  works:
	 *
	 *  Bilinear interpolation interpolates over two dimensions. So you get two
	 *  coordinates, e.g. x=3.4 and y=2.5. Then you have to interpolate between
	 *  the points Q11=(3,2), Q12=(4,2), Q21=(3,3) and Q22=(4,3) somehow.  You
	 *  can do this by first doing a linear interpolation between Q11 and Q12
	 *  and then Q21 and Q22, which gives you the values for two new
	 *  points. Then you can do a linear interpolation between these points and
	 *  here you are.
	 *
	 *  So - generally - n-linear interpolation can be done by successive linear
	 *  interpolation in all dimension.
	 *
	 *  See also http://en.wikipedia.org/wiki/Bilinear_interpolation for more
	 *  details.
	 *
	 *  @internal
	 *  @param M Original number of dimensions.
	 */
	template<typename T,
		 InterpolationDefs::SizeType N,
		 InterpolationDefs::SizeType M = N>
	class NLinear : public Interpolation<T, N>
	{
		/**
		 *  @brief We inherit from this class.
		 */
		typedef Interpolation<T, N> SuperType;
	public:
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
		 *  @brief Type to be used in constructor
		 */
		typedef typename SuperType::InitType InitType;

		/**
		 *  @brief Type of the hyperplanes.
		 */
		typedef const NLinear<T, N-1, M> HyperplaneType;

		/**
		 *  @brief Constant reference to hyperplanes.
		 */
		typedef const HyperplaneType& ConstReference;

		/**
		 *  @brief List of coordinates.
		 */
		typedef std::list<CoordType> CoordList;

		/**
		 *  @brief Construct a NLinear interpolation object that
		 *  interpolates over discreteValues.
		 *
		 *  @param discreteValues (in) Container that contains the
		 *  values which we interpolate over.
		 *
		 *  You should call this constructor only with its default
		 *  values!
		 *
		 *  @internal
		 *  @param universe Pointer to the discrete container specified
		 *  at template recursion level @a M.
		 *  @param coords Pointer to the shared CoordList that gets
		 *  constructed by operator[].
		 *
		 *  This constructor serves two purposes, it is the external
		 *  user interface constructor (when used with the default
		 *  values) and the internal "recursive template" constructor.
		 *
		 *  @a discreteValues is used only in the top template recursion
		 *  level (@a M) to set @a universe.  If @a universe is set to
		 *  non-zero its value is passed down up to the @a N == 1
		 *  template.
		 */
		NLinear(const DiscreteContainer& discreteValues,
			const void *const universe = 0,
			CoordList *const coords = 0)
			: SuperType(discreteValues),
			  coords((coords == 0) ? new CoordList() : coords),
			  deleteCoords(coords == 0),
			  //  what value this has actually doesn't matter
			  hyperplane(discreteValues[0],
				     (universe == 0) ? &discreteValues : universe,
				     this->coords)      // because we use this ^
		{}

		/**
		 *  @brief Destructor.
		 */
		virtual ~NLinear()
		{
			if (deleteCoords) delete coords;
		}

		/**
		 *  @brief Specify a coordinate to be interpolated.
		 *
		 *  Interpolate with the coordinate specified by @a coord.
		 *
		 *  @internal We only want to create a CoordList. The real
		 *  calculation and DiscreteContainer access happens at N == 1
		 *  with the help of the CoordList. This is also why we pass the
		 *  original value of N (M) down to the N == 1 template, because
		 *  it needs to know what type the DiscreteContainer is.
		 */
		virtual ConstReference operator[](const CoordType& coord) const
		{
			coords->push_back(coord);
			return hyperplane;
		}

	private:
		/**
		 *  @internal
		 *  @brief List of the coordinates we need to interpolate with.
		 *
		 *  This list is shared with all hyperplanes.
		 */
		CoordList* const coords;

		/**
		 *  @internal
		 *  @brief Does our constructor need to destroy coords.
		 */
		const bool deleteCoords;

		/**
		 *  @internal
		 *  @brief the hyperplanes.
		 */
		HyperplaneType hyperplane;
	};

	/**
	 *  @brief One dimensional specialization of NLinear Interpolation
	 */
	template<typename T,
		 InterpolationDefs::SizeType M>
	class NLinear<T, 1, M> : public Interpolation<T, 1>
	{
		typedef Interpolation<T, 1> SuperType;
	public:
		typedef typename SuperType::ValueType ValueType;
		typedef typename SuperType::CoordType CoordType;
		typedef typename SuperType::DiscreteContainer DiscreteContainer;
		typedef typename SuperType::InitType InitType;
		typedef typename DiscreteContainer::IndexType IndexType;

		typedef std::list<CoordType> CoordList;

		NLinear(const DiscreteContainer& discreteValues,
			const void *const universe = 0,
			CoordList *const coords = 0)
			: SuperType(discreteValues),
			  universe((universe == 0) ? &discreteValues : universe),
			  coords((coords == 0) ? new CoordList() : coords),
			  deleteCoords(coords == 0)
		{}

		virtual ~NLinear()
		{
			if (deleteCoords) delete coords;
		}

		virtual ValueType operator[](const CoordType& coord) const
		{
			coords->push_back(coord);
			const ValueType v = this->interpolate(*coords);
			coords->clear();
			return v;
		}

	private:
		/**
		 *  @internal
		 *  @brief Type of the container passed to the top recursion level template.
		 *
		 *  The original value of N is passed down by M for this.
		 */
		typedef typename Interpolation<T, M>::DiscreteContainer UniverseType;
		typedef std::list<IndexType> IndexList;

		const void *const universe;
		CoordList *const coords;
		const bool deleteCoords;

		/**
		 *  @internal
		 *  @brief Perform linear interpolation with @a coords.
		 *
		 *  @param coords (in) List of the coords to interpolate with.
		 *  @param indices (in) Internal list of indices to access.
		 *
		 *  @return Interpolated value.
		 *
		 *  This function recursivly calculates a nlinear interpolation
		 *  for the coordinates specified by @a coords.  This is done by
		 *  reducing the size of @a coords in each recursion step by one
		 *  and calling interpolate two times with @a indices lengthened
		 *  by @c floor resp. @c ceil of the removed coordinate. The
		 *  returned values are then interpolated. If @a coords is
		 *  empty, interpolate accesses the discreteContainer
		 *  (*universe) with the @a indices list and returns that value.
		 */
		ValueType interpolate(const CoordList& coords,
				      const IndexList& indices = IndexList()) const
		{
			if (coords.empty()) {
				// If coords is empty, there is nothing to interpolate.
				// So just return the values in the discreteContainer.
				return static_cast<const UniverseType *const>(universe)->at(indices.begin(), indices.end());
			}
			else {
				// calculate new indices
				const CoordType& x = coords.back();
				const IndexType x1 = (IndexType)(std::floor(x));
				const IndexType x2 = (IndexType)(std::ceil(x));
				// shorten coordlist
				CoordList c = CoordList(coords);
				c.pop_back();
				// construct new index lists
				IndexList i1 = IndexList(indices); i1.push_front(x1);
				IndexList i2 = IndexList(indices); i2.push_front(x2);
				// recursivly get values to interpolate
				const ValueType f1 = interpolate(c, i1);
				const ValueType f2 = interpolate(c, i2);
				// interpolate and return
				return f1 + (f2 - f1) * (x - x1);
			}
		}
	};

}

#endif // _NLINEAR_HPP

/**
 * @file
 */


