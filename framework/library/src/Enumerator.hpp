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

#ifndef _ENUMERATOR_HPP
#define _ENUMERATOR_HPP

#include <limits>
#include <cmath>
#include <stdexcept>

#include <WNS/Types.hpp>

namespace wns
{

    /**
     *  @brief Totally static class for enumeration of especially FP values.
     *
     *  With this class one can enumerate over all values of a type with @c succ(v) and
     *  @c pred(v). For all but floating point data types this is a wrapper for @c ++v
     *  and @c --v.
     */
    class Enumerator
    {
    public:

	/**
	 *  @brief Return successor of @a v.
	 *
	 *  @param v (in) return the successor of this value.
	 */
	template<typename T>
	static inline T succ(const T& v) { T temp = v; return ++temp; }
    
	static inline float succ(const float& v) { return fp_succ(v); }

	static inline double succ(const double& v) { return fp_succ(v); }

	static inline long double succ(const long double& v) { return fp_succ(v); }

	/**
	 *  @brief Return predecessor of @a v.
	 *
	 *  @param v (in) return the predecessor of this value.
	 */
	template<typename T>
	static inline T pred(const T& v) { T temp = v; return --temp; }

	static inline float pred(const float& v) { return fp_pred(v); }

	static inline double pred(const double& v) { return fp_pred(v); }

	static inline long double pred(const long double& v) { return fp_pred(v); }

    private:

	// For IEEE 754/IEC 559 compatible representations of
	// floating point numbers enumeration could be done
	// by simply incrementing/decrementing the binary
	// representation (and take care of the bounds 0, +/-inf).
	// But as IEEE 754/IEC 559 compatibility is not required
	// by C++ (otherwise we wouldn't need much of numeric_limits),
	// the enumeration is implemented more generic
	// (which also means less low-level); especially since I do not
	// expect these functions to be used in time critical
	// situations (inner loops etc.).
	// If they ever are, one could consider bloating these functions/
	// this class with specialised increment/decrement for IEEE/IEC FP.
	// (if (numeric_limits<T>::is_iec559) ...)

	template<typename T>
	static inline T fp_succ(const T& v)
	{
	    // NaNs don't have successors
	    if ((std::numeric_limits<T>::has_quiet_NaN
		 && (v == std::numeric_limits<T>::quiet_NaN()))
		|| (std::numeric_limits<T>::has_signaling_NaN
		    && (v == std::numeric_limits<T>::signaling_NaN())))
		throw std::invalid_argument("No successor for a NaN");
	    // take care of upper bound (inf/max)
	    if (std::numeric_limits<T>::has_infinity) {
		// type has infinity
		if (v == std::numeric_limits<T>::max())
		    return std::numeric_limits<T>::infinity();
		else if (v == -std::numeric_limits<T>::infinity())
		    return -std::numeric_limits<T>::max();
		else if (v == std::numeric_limits<T>::infinity())
		    throw std::out_of_range("No successor of infinity");
	    }
	    else if (v == std::numeric_limits<T>::max())
		// type does not have infinity and thus no larger "value"
		throw std::out_of_range("No successor of largest value");

	    // take care of "-" -> "0" -> "+" transition
	    if (v == -std::numeric_limits<T>::denorm_min()) return 0.0;
	    else if (v == 0.0) return std::numeric_limits<T>::denorm_min();
	    else {
		// let's increase the least significant digit of the mantissa

		const SizeType digitsSign = 1;
		const SizeType digitsMantissa = std::numeric_limits<T>::digits - digitsSign;
		const SizeType radix = std::numeric_limits<T>::radix;

		// get the exponent (a silly C function which returns the exp through call-by-reference...)
		int exp; (void)std::frexp(v, &exp);
		// calculate the increment
		const T increment = std::max(std::pow((T)radix, (T)exp - (T)digitsMantissa - 1),
					     std::numeric_limits<T>::denorm_min());
		return v + increment;
	    }
	}

	template<typename T>
	static inline T fp_pred(const T& v)
	{
	    // NaNs don't have predecessors
	    if ((std::numeric_limits<T>::has_quiet_NaN
		 && (v == std::numeric_limits<T>::quiet_NaN()))
		|| (std::numeric_limits<T>::has_signaling_NaN
		    && (v == std::numeric_limits<T>::signaling_NaN())))
		throw std::invalid_argument("No predecessor for a NaN");
	    // take care of lower bound (-inf/-max)
	    if (std::numeric_limits<T>::has_infinity) {
		// type has infinity
		if (v == -std::numeric_limits<T>::max())
		    return -std::numeric_limits<T>::infinity();
		else if (v == std::numeric_limits<T>::infinity())
		    return std::numeric_limits<T>::max();
		else if (v == -std::numeric_limits<T>::infinity())
		    throw std::out_of_range("No predecessor of -infinity");
	    }
	    else if (v == -std::numeric_limits<T>::max())
		// type does not have infinity and thus no smaller "value"
		throw std::out_of_range("No predecessor of smallest value");

	    // take care of "+" -> "0" -> "-" transition
	    if (v == std::numeric_limits<T>::denorm_min()) return 0.0;
	    else if (v == 0.0) return -std::numeric_limits<T>::denorm_min();
	    else {
		// let's decrease the least significant digit of the mantissa

		const SizeType digitsSign = 1;
		const SizeType digitsMantissa = std::numeric_limits<T>::digits - digitsSign;
		const SizeType radix = std::numeric_limits<T>::radix;

		// get the exponent (a silly C function which returns the exp through call-by-reference...)
		int exp; (void)std::frexp(v, &exp);
		// calculate the increment
		const T decrement = std::max(std::pow((T)radix, (T)exp - (T)digitsMantissa - 1),
					     std::numeric_limits<T>::denorm_min());
		return v - decrement;
	    }
	}
    };

    /**
     *  @ingroup MISC
     *  @brief Make @a T a enumerable type.
     *
     *  @param T base type.
     *
     *  This class makes @a T enumerable by ++ and -- with the help of Enumerator.
     *  This means it does basically nothing except for floating point numbers,
     *  for which ++ and -- really become iterators/enumerators.
     *  Enumerable values are implicitly convertable to their base type, but
     *  base types are only explicitly convertable to their associated enumerable type.
     */
    template<typename T>
    class Enumerable
    {
    public:
	typedef T ValueType;

	/**
	 *  @brief Construct an enumerable version of @a value.
	 *
	 *  @param value Make this enumerable.
	 */
	explicit Enumerable(const ValueType& value)
	    : value(value)
	{}

	/**
	 *  @brief Implicit Conversion Enumerable<T> -> T.
	 *
	 *  @return Value of enumerable type (as base type).
	 */
	operator ValueType() const
	{
	    return value;
	}

	/**
	 *  @brief Pre-increment enumerator.
	 *
	 *  @return Successor of @c *this.
	 *
	 *  Sets the value of @c *this to it's successor
	 *  and returns the new value.
	 */
	ValueType& operator++()
	{
	    return value = Enumerator::succ(value);
	}

	/**
	 *  @brief Post-increment enumerator.
	 *
	 *  @return @c *this.
	 *
	 *  Sets the value of @c *this to it's successor
	 *  and returns the previous value.
	 */
	ValueType operator++(int)
	{
	    ValueType temp = value;
	    ++value;
	    return temp;
	}

	/**
	 *  @brief Pre-decrement enumerator.
	 *
	 *  @return Predecessor of @c *this.
	 *
	 *  Sets the value of @c *this to it's predecessor
	 *  and returns the new value.
	 */
	ValueType& operator--()
	{
	    return value = Enumerator::pred(value);
	}

	/**
	 *  @brief Post-decrement enumerator.
	 *
	 *  @return @c *this.
	 *
	 *  Sets the value of @c *this to it's predecessor
	 *  and returns the previous value.
	 */
	ValueType operator--(int)
	{
	    ValueType temp = value;
	    --value;
	    return temp;
	}

	/**
	 *  @brief Get the current value.
	 *
	 *  @return Value of @c *this.
	 */
	ValueType get() const
	{ 
	    return value; 
	}

	/**
	 *  @brief Set new value.
	 *
	 *  @param value Set to this value.
	 */
	void set(const ValueType& value) 
	{ 
	    this->value = value; 
	}

    private:
	ValueType value;
    };

    /**
     *  @brief Make value @a v enumerable.
     *
     *  @param v Convert this value to it's enumerable version.
     *
     *  This function converts @a v of type @c T to an Enumerable<T>.
     *  It exists primarily to remove the necessity of specifying the
     *  type (as in Enumerable<T>(v)).
     */
    template<typename T>
    Enumerable<T> makeEnumerable(const T& v)
    {
	return Enumerable<T>(v);
    }

}

#endif // _ENUMERATOR_HPP
