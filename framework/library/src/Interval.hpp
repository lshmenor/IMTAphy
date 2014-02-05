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

#ifndef WNS_INTERVAL_HPP
#define WNS_INTERVAL_HPP

#include <WNS/Enumerator.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Assure.hpp>

#include <string>

namespace wns
{
    /**
     *  @brief Interval representation.
     *
     *  @param T Interval of this type.
     *
     *  This template class represents Intervals in the set @c T. Please be aware
     *  that in mathematics intervals are usually subsets of R, but computers - being finite -
     *  don't have reals. So you can create Intervals of integer and floating point types with
     *  this class.
     *
     *  Intervals are created quite easily. Examples:
     *
     *  - @code
     *    typedef Interval<double> FrequencyRange;
     *    FrequencyRange frequencies = FrequencyRange::From(150).To(2000);
     *    @endcode
     *    (closed: [150, 2000])
     *
     *  - @code
     *    typedef Interval<double> DistanceRange;
     *    DistanceRange distances = DistanceRange::Between(0).And(INFINITY);
     *    @endcode
     *    (open: (0, oo))
     *
     *  - @code
     *    typedef Interval<double> TimeRange;
     *    TimeRange allowedDelay = TimeRange::FromIncluding(0).ToExcluding(200);
     *    @endcode
     *    (half open: [0, 200))
     *
     *  Note that the constructor is private; use the static constructing functions instead.
     *
     *  Interval supports all combinations of open and closed intervals: (a,b), (a,b], [a,b) and [a,b].
     *
     *  Intervals are immutable.
     */
    template<typename T>
    class Interval
    {
	friend class Endpoint;
    public:
	typedef T ValueType;
	
	/**
	 *  @brief Open or closed endpoint of an interval.
	 */
	class Endpoint
	{
	    friend class Interval;
	public:
	    Endpoint(const ValueType& bound, const ValueType& extremum)
		: bound(bound), extremum(extremum)
	    {}

	    bool operator==(const Endpoint& b) const
	    {
		return (bound == b.bound) && (extremum == b.extremum);
	    }

	    bool operator<(const Endpoint& b) const
	    {
		return (extremum < b.extremum);
	    }
	    
	    bool operator<=(const Endpoint& b) const
	    {
		return (extremum <= b.extremum);
	    }
	    
	    bool operator>(const Endpoint& b) const
	    {
		return (extremum > b.extremum);
	    }
	    
	    bool operator>=(const Endpoint& b) const
	    {
		return (extremum >= b.extremum);
	    }
	    
	private:
	    ValueType bound;
	    ValueType extremum;
	};

	/**
	 *  @brief Endpoint for syntax FromIncluding/FromExcluding ... ToIncluding/ToExcluding.
	 */
	class FromInclExclEndpoint : public Endpoint
	{
	public:
	    FromInclExclEndpoint(const ValueType& bound, const ValueType& extremum)
		: Endpoint(bound, extremum)
	    {}

	    Interval ToIncluding(const ValueType& b) const
	    {
		return Interval(*this, Endpoint(b, b));
	    }

	    Interval ToExcluding(const ValueType& b) const
	    {
		return Interval(*this, Endpoint(b, --makeEnumerable(b)));
	    }
	};

	/**
	 *  @brief Endpoint for syntax From ... To.
	 */
	class FromEndpoint : public Endpoint
	{
	public:
	    FromEndpoint(const ValueType& bound, const ValueType& extremum)
		: Endpoint(bound, extremum)
	    {}

	    Interval To(const ValueType& b) const
	    {
		return Interval(*this, Endpoint(b, b));
	    }

	};

	/**
	 *  @brief Endpoint for syntax Between ... And.
	 */
	class BetweenEndpoint : public Endpoint
	{
	public:
	    BetweenEndpoint(const ValueType& bound, const ValueType& extremum)
		: Endpoint(bound, extremum)
	    {}

	    Interval And(const ValueType& b) const
	    {
		return Interval(*this, Endpoint(b, --makeEnumerable(b)));
	    }
	};

	/**
	 *  @brief Create interval from pyconfig::View.
	 *
	 *  @param config (in) Configuration specifying the endpoints.
	 *
	 */
	static Interval CreateFrom(const pyconfig::View& config)
	{
    	    const ValueType min = config.get<ValueType>("lowerBound");

    	    const ValueType max = config.get<ValueType>("upperBound");

   	    const std::string intervalType = config.get<std::string>("intervalType");

    	    Endpoint a(min, (intervalType[0] == '(') ? ++makeEnumerable(min) : min);
    	    Endpoint b(max, (intervalType[1] == ')') ? --makeEnumerable(max) : max);
    	    return Interval<ValueType>(a, b);
	}
	
	/**
	 *  @brief Create open interval.
	 *
	 *  @param a (in) lower bound.
	 *
	 *  Use like this: @c Between(a).@c And(b). This creates the interval (a,b).
	 */
	static BetweenEndpoint Between(const ValueType& a)
	{
	    return BetweenEndpoint(a, ++makeEnumerable(a));
	}

	/**
	 *  @brief Create closed interval.
	 *
	 *  @param a (in) lower bound.
	 *
	 *  Use like this: @c From(a).@c To(b). This creates the interval [a,b].
	 */
	static FromEndpoint From(const ValueType& a)
	{
	    return FromEndpoint(a, a);
	}

	/**
	 *  @brief Create half-open (or closed) interval.
	 *
	 *  @param a (in) lower bound.
	 *
	 *  Use like this: @c FromIncluding(a).@c ToExcluding(b). This creates [a,b).
	 *  (You could also use @c FromIncluding(a).@c ToIncluding(b) but @c From(a).To(b) is shorter.)
	 */
	static FromInclExclEndpoint FromIncluding(const ValueType& a)
	{
	    return FromInclExclEndpoint(a, a);
	}

	/**
	 *  @brief Create half-open (or open) interval.
	 *
	 *  @param a (in) lower bound.
	 *
	 *  Use like this: @c FromExcluding(a).@c ToIncluding(b). This creates (a,b].
	 *  (You could also use @c FromExcluding(a).@c ToExcluding(b) but Between(a).And(b) is shorter.)
	 */
	static FromInclExclEndpoint FromExcluding(const ValueType& a)
	{
	    return FromInclExclEndpoint(a, ++makeEnumerable(a));
	}

	/** @brief Return empty interval.
	 */
	static Interval EmptyInterval()
	{
	    return Interval();
	}

	/** @brief Return empty interval.
	 */
	Interval()
	    : a(Endpoint(ValueType(), ++makeEnumerable(ValueType()))),
	      b(Endpoint(ValueType(), --makeEnumerable(ValueType())))
	{}

	/**
	 *  @brief Check if two intervals are equal.
	 */
	bool operator==(const Interval& x) const
	{
	    return (isEmpty() && x.isEmpty()) || ((a == x.a) && (b == x.b));
	}

	bool operator!=(const Interval& x) const
	{
	    return !((*this) == x);
	}

	/**
	 *  @brief Check if the value @a x is greater than all values of the interval.
	 *
	 *  @param x Is that value greater?
	 */
	bool isBelow(const ValueType& x) const
	{
	    return !isEmpty() && (b.extremum < x);
	}

	/** 
	 *  @brief Check if this interval is below interval @a x.
	 *  
	 *  @param x Is that larger?
	 *
	 *  This returns true, if *this contains values smaller than
	 *  the values of @a x.
	 */
	bool isBelow(const Interval& x) const
	{
	    return !isEmpty() && !x.isEmpty() && (b.extremum < x.a.extremum);
	}

	/**
	 *  @brief Check if the value @a x is smaller than all values of the interval.
	 *
	 *  @param x Is that value smaller?
	 */
	bool isAbove(const ValueType& x) const
	{
	    return !isEmpty() && (x < a.extremum);
	}

	/**
	 *  @brief Check if this interval is above interval @a x.
	 *  
	 *  @param x Is that smaller?
	 *
	 *  This returns true, if *this contains values larger than
	 *  the values of @a x.
	 */
	bool isAbove(const Interval& x) const
	{
	    return !isEmpty() && !x.isEmpty() && (a.extremum > x.b.extremum);
	}

	/**
	 *  @brief Check if this interval is empty.
	 *
	 *  @return true if empty.
	 */
	bool isEmpty() const
	{
	    return a.extremum > b.extremum;
	}

	/**
	 *  @brief Return minimal value that is contained in this interval.
	 *
	 *  @return minimal value.
	 */
	ValueType min() const
	{
	    assure(!isEmpty(), "Error: Attempt to get minimum of an empty interval.");
	    return a.extremum;
	}

	/**
	 *  @brief Return maximal value that is contained in this interval.
	 *
	 *  @return maximal value.
	 */
	ValueType max() const 
	{ 
	    assure(!isEmpty(), "Error: Attempt to get maximum of an empty interval.");
	    return b.extremum; 
	}

	/**
	 *  @brief Check if value @a x is in the interval.
	 *
	 *  @param x Is that value contained?
	 */
	bool contains(const ValueType& x) const
	{
	    return !isEmpty() && (a.extremum <= x) && (b.extremum >= x);
	}

	/**
	 *  @brief Check if the interval @a x is totally contained in *this.
	 *
	 *  @param x Is that interval contained?
	 */
	bool contains(const Interval& x) const
	{
	    return x.isEmpty() || (!isEmpty() && (a.extremum <= x.a.extremum) && (b.extremum >= x.b.extremum));
	}

	/**
	 *  @brief Check if this interval overlaps with the interval @a x.
	 *
	 *  @param x Are we overlapping with that?
	 */
	bool overlaps(const Interval& x) const
	{
	    return !isEmpty() && !x.isEmpty()
		&& (((x.a.extremum <= a.extremum) && (a.extremum <= x.b.extremum)) ||
		    ((a.extremum <= x.a.extremum) && (x.a.extremum <= b.extremum)));
	}

	/**
	 *  @brief Check if this interval can be joined with interval @a x.
	 *
	 *  @param x Can we be joined with that?
	 *
	 *  Intervals are joinable, if a new Interval can be created from them,
	 *  that contains all values from the original intervals. Thus they
	 *  must be at least directly adjacent.
	 */
	bool joinableWith(const Interval& x) const
	{
	    return isEmpty() || x.isEmpty() ||
		overlaps(Interval(Endpoint(--makeEnumerable(x.a.bound), --makeEnumerable(x.a.extremum)), x.b)) ||
		x.overlaps(Interval(Endpoint(--makeEnumerable(a.bound), --makeEnumerable(a.extremum)), b));
	}

	/**
	 *  @brief Create a new interval by joining *this with @a x.
	 *
	 *  @param x Join with that.
	 *
	 *  @return joined Interval.
	 *
	 *  Return a new Interval that ranges from the smallest Endpoint
	 *  to the largest Endpoint. *this and @a x need to be joinable.
	 *
	 *  @see joinable(const Interval&)
	 */
	Interval join(const Interval& x) const
	{
	    assure(joinableWith(x), "Error: Attempt to join unjoinable intervals (must be at least adjacent).");
	    if (isEmpty()) return Interval(x);
	    else if (x.isEmpty()) return Interval(*this);
	    else {
		const Endpoint a(std::min(this->a, x.a));
		const Endpoint b(std::max(this->b, x.b));
		return Interval(a, b);
	    }
	}

	/**
	 *  @brief Create a new interval by intersecting *this with @a x.
	 *
	 *  @param x Intersect with that.
	 *
	 *  @return intersection Interval.
	 *
	 *  Return a new Interval that is the intersection of *this and @a x.
	 *  The returned Interval may be empty.
	 */
	Interval intersect(const Interval& x) const
	{
	    const Endpoint a(std::max(this->a, x.a));
	    const Endpoint b(std::min(this->b, x.b));
	    return Interval(a, b);
	}

    private:

	Interval(const Endpoint& a, const Endpoint& b)
	    : a(a), b(b)
	{}

	Endpoint a;
	Endpoint b;
    };

}

#endif // _INTERVAL_HPP
