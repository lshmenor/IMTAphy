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


#ifndef _POWER_RATIO_HPP
#define _POWER_RATIO_HPP


#include "Exception.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdint.h>
#include <WNS/Assure.hpp>

namespace wns {
	class Ratio;
	class Power;

// ------------------- R A T I O --------------------------------------
// Declaration
//! Helps calculating with dB
/** @ingroup MISC */
	class Ratio
	{
		friend class Power;
	public:

		// Default constructor
		inline Ratio();
		inline Ratio(const Ratio &r);

		// Destructor
		inline ~Ratio();

		static inline Ratio from_factor(double aValue);
		static inline Ratio from_dB(double aValue);

		inline void   set_factor (double aValue);
		inline void   set_dB     (double aValue);
		inline double get_factor () const;
		inline double get_dB     () const;

		inline Ratio  operator - (const Ratio& r) const;
		inline void   operator -=(const Ratio& r);
		inline Ratio  operator + (const Ratio& r) const;
		inline void   operator +=(const Ratio& r);
		inline bool   operator < (const Ratio& r) const;
		inline bool   operator > (const Ratio& r) const;
		inline bool   operator >=(const Ratio& r) const;
		inline bool   operator <=(const Ratio& r) const;
		inline bool   operator ==(const Ratio& r) const;
		inline bool   operator !=(const Ratio& r) const;
		inline Ratio& operator = (const Ratio& r);

		inline long double
		convertForAveraging() const;

		static inline Ratio
		convertForAveraged(long double d);

		friend std::ostream& operator <<(std::ostream &str, const wns::Ratio& r)
		{
			str << r.get_dB() << " dB";
			return str;
		}

		friend std::istream& operator >>(std::istream &str, Ratio& r) {
			double tmp;
			str >> tmp;
			if(str.eof()) {
				std::cout << "Warning: No unit provided (dB)!"
					  << "Assuming ratio to be in dB."
					  << std::endl;
				r.set_dB(tmp);
				return str;
			}
			std::string sTmp;
			str >> sTmp;
			if (sTmp=="dB") {
				r.set_dB(tmp);
				return str;
			}
			if (sTmp=="dBi") {
				r.set_dB(tmp);
				return str;
			}
			if (sTmp=="dBd") {
				r.set_dB(tmp + 2.15);
				return str;
			}
			throw(Exception("Warning: No valid unit provided for ratio (dB)! Got: " + sTmp));
		}

	protected:
		double factor;

	private:
		inline Ratio(double aValue);
	};

//----------------------- P O W E R ----------------------------------
// Declaration
//! Helps calculating with dBm and mWatt
/** @ingroup MISC */
	class Power
	{
		friend class Ratio;
	public:

		// Default constructor
		inline Power();
		inline Power(const Power& aPowerRef);

		// Destructor
		inline ~Power();

		static inline Power from_dBm(double aValue);
		static inline Power from_mW(double aValue);

		inline void   set_dBm    (double aValue);
		inline void   set_mW     (double aValue);
		inline double get_dBm    () const;
		inline double get_mW     () const;

		inline void   operator +=(const Power& p);
		inline void   operator -=(const Power& p);
		inline Power  operator + (const Power& p) const;
		inline Power  operator - (const Power& p) const;
		inline void   operator +=(const Ratio& r);
		inline void   operator -=(const Ratio& r);
		inline void   operator *=(double d);
		inline void   operator *=(float f);
		inline void   operator *=(long int i);
		inline void   operator *=(unsigned long int i);
		inline void   operator /=(double d);
		inline void   operator /=(float f);
		inline void   operator /=(long int i);
		inline void   operator /=(unsigned long int i);
		inline Power  operator * (double d) const;
		inline Power  operator * (float f) const;
		inline Power  operator * (long int i) const;
		inline Power  operator * (unsigned long int i) const;
		inline Power  operator * (const Ratio& r) const;
		inline Power  operator / (double d) const;
		inline Power  operator / (float f) const;
		inline Power  operator / (long int i) const;
		inline Power  operator / (unsigned long int i) const;
		inline Power  operator / (const Ratio& r) const;
		inline Ratio  operator / (const Power& p) const;
		inline bool   operator < (const Power& p) const;
		inline bool   operator > (const Power& p) const;
		inline bool   operator <=(const Power& p) const;
		inline bool   operator >=(const Power& p) const;
		inline bool   operator ==(const Power& p) const;
		inline bool   operator !=(const Power& p) const;
		inline Power& operator = (const Power& p);
		inline long double convertForAveraging() const {
			return mW;
		}
		static inline Power convertForAveraged(long double d) {
			return Power::from_mW(d);
		}

		friend std::ostream& operator <<(std::ostream &str, const wns::Power& p)
		{
			str << p.get_dBm() << " dBm";
			return str;
		}

		friend std::istream& operator >>(std::istream &str, Power& p) {
			double tmp;
			str >> tmp;
			if(str.eof()) {
				std::cout << "Warning: No unit provided (dBm, mW, W)!"
					  << "Assuming power to be in dBm."
					  << std::endl;
				p.set_dBm(tmp);
				return str;
			}
			std::string sTmp;
			str >> sTmp;
			if (sTmp=="dBm") {
				p.set_dBm(tmp);
				return str;
			}
			if (sTmp=="mW") {
				p.set_mW(tmp);
				return str;
			}
			if (sTmp=="W") {
				p.set_mW(tmp*1000.0);
				return str;
			}
			throw(Exception("Warning: No valid unit provided for power (dBm, mW, W)"));
		}

	protected:
		double mW;
	private:
		inline Power(double aValue);
	};


// ------------------- R A T I O --------------------------------------
// Implementation
//! Sets the Ration to 0.0 dB
	inline  Ratio::Ratio()
    : factor(1.0) {}

//! Sets the Ration to the value of r
	inline  Ratio::Ratio(const Ratio &r)
		: factor(r.factor) {}

	inline  Ratio::Ratio(double aValue)
    : factor(aValue) {
		assert(aValue>=0);
	}

	inline  Ratio::~Ratio() {}

	inline Ratio&
	Ratio::operator =(const Ratio& r)
	{
		assert(r.factor>=0);
		factor = r.factor;
		return *this;
	}

	inline Ratio Ratio::from_factor(double aValue) {
		assert(aValue>=0);
		Ratio r;
		r.set_factor(aValue);
		return r;
	}

	inline Ratio Ratio::from_dB(double aValue) {
		Ratio r;
		r.set_dB(aValue);
		return r;
	}

	inline void Ratio::set_factor(double aValue) {
		assert(aValue>=0);
		factor = aValue;
	}

	inline void Ratio::set_dB(double aValue) {
		factor = pow(10.0,aValue/10.0);
	}

	inline double Ratio::get_factor() const {
		assert(factor>=0);
		return factor;
	}

	inline double Ratio::get_dB() const {
		assert(factor>=0);
		return 10.0*log10(factor);
	}

	inline Ratio Ratio::operator -(const Ratio& r) const {
		assert(factor>=0);
		assert(r.factor>=0);
        Ratio re(factor/r.factor);
		return re;
	}

	inline void Ratio::operator -=(const Ratio& r) {
		assert(factor>=0);
		assert(r.factor>=0);

        factor /= r.factor;
	}

	inline Ratio Ratio::operator +(const Ratio& r) const {
		assert(factor>=0);
		assert(r.factor>=0);

        Ratio re(factor*r.factor);
        return re;
	}

	inline void Ratio::operator +=(const Ratio& r) {
		assert(factor>=0);
		assert(r.factor>=0);
		factor *= r.factor;
	}

	inline bool Ratio::operator <(const Ratio& r) const  {
		assert(factor>=0);
		assert(r.factor>=0);
		return (factor < r.factor);
	}

	inline bool Ratio::operator >(const Ratio& r) const {
		assert(factor>=0);
		assert(r.factor>=0);
		return (factor > r.factor);
	}

	inline bool Ratio::operator >=(const Ratio& r) const {
		assert(factor>=0);
		assert(r.factor>=0);
		return (factor >= r.factor);
	}

	inline bool Ratio::operator <=(const Ratio& r) const {
		assert(factor>=0);
		assert(r.factor>=0);
		return (factor <= r.factor);
	}

	inline bool Ratio::operator ==(const Ratio& r) const {
		assert(factor>=0);
		assert(r.factor>=0);
		return (factor == r.factor);
	}

	inline bool Ratio::operator !=(const Ratio& r) const {
		assert(factor>=0);
		assert(r.factor>=0);
		return (factor != r.factor);
	}

	inline long double Ratio::convertForAveraging() const {
		return get_dB();
	}

	inline Ratio Ratio::convertForAveraged(long double d) {
		return Ratio::from_dB(d);
	}

//----------------------- P O W E R ----------------------------------
// Implementation

// Default and only constructor, sets the power to 0.0 mW
	inline Power::Power()
		: mW(0.0) {}

	inline Power::Power(double aValue)
		: mW(aValue) {}

	inline Power::Power(const Power& aPowerRef)
		: mW(aPowerRef.mW) {}

	inline Power::~Power() {}

	inline Power Power::from_dBm(double aValue) {
		Power p;
		p.set_dBm(aValue);
		return p;
	}

	inline Power Power::from_mW(double aValue) {
		assert(aValue>=0);
		Power p;
		p.set_mW(aValue);
		return p;
	}

	inline void  Power::set_dBm(double aValue) {
		mW = pow(10.0, aValue/10.0);
	}

	inline void Power::set_mW(double aValue) {
		assert(aValue>=0);
		mW = aValue;
	}

	inline double Power::get_dBm() const  {
		return 10.0*log10(mW);
	}

	inline double Power::get_mW() const {
		return mW;
	}

	inline Power&
	Power::operator = (const Power& p)
	{
		mW = p.mW;
		return *this;
	}

	inline void Power::operator += (const Power& p) {
		mW += p.mW;
	}

	inline void Power::operator -= (const Power& p) {
		mW -= p.mW;
		assert(mW>=0);
	}

	inline Power Power::operator +(const Power& p) const {
		return Power(mW+p.mW);
	}

	inline Power  Power::operator -(const Power& p) const {
		return Power(mW-p.mW);
	}

	inline void Power::operator += (const Ratio& r) {
		assert(r.factor>=0);
		mW *= r.factor;
	}

	inline void Power::operator -= (const Ratio& r) {
		assert(r.factor>=0);
		mW /= r.factor;
	}

	inline Power Power::operator *(double d) const {
		return Power(mW*d);
	}

	inline Power Power::operator *(float f) const {
		return Power(mW*(double)f);
	}

	inline Power Power::operator *(long int i) const {
		return Power(mW*(double)i);
	}

	inline Power Power::operator *(unsigned long int u) const {
		return Power(mW*(double)u);
	}

	inline Power Power::operator *(const Ratio& r) const {
		return Power(mW*r.get_factor());
	}

	inline void Power::operator *=(double d) {
		mW*=d;
		assert(mW>=0);
	}

	inline void Power::operator *=(float f) {
		mW*=f;
		assert(mW>=0);
	}

	inline void Power::operator *=(long int i) {
		mW*=i;
		assert(mW>=0);
	}

	inline void Power::operator *=(unsigned long int u) {
		mW*=u;
	}

	inline Power Power::operator /(double d) const {
		return Power(mW/d);
	}

	inline Power Power::operator /(float f) const {
		return Power(mW/(double)f);
	}

	inline Power Power::operator /(long int i) const {
		return Power(mW/(double)i);
	}

	inline Power Power::operator /(unsigned long int u) const {
		return Power(mW/(double)u);
	}

	inline Power Power::operator /(const Ratio& r) const {
		return Power(mW/r.get_factor());
	}

	inline void Power::operator /=(double d) {
		mW/=d;
		assert(mW>=0);
	}

	inline void Power::operator /=(float f) {
		mW/=(double)f;
		assert(mW>=0);
	}

	inline void Power::operator /=(long int i) {
		mW/=(double)i;
		assert(mW>=0);
	}

	inline void Power::operator /=(unsigned long int u) {
		mW/=(double)u;
	}

	inline Ratio Power::operator /(const Power& p) const {
		return Ratio(mW/p.mW);
	}

	inline bool Power::operator <(const Power& p) const {
		return (mW < p.mW);
	}

	inline bool Power::operator >(const Power& p) const {
		return (mW > p.mW);
	}

	inline bool Power::operator <=(const Power& p) const {
		return (mW <= p.mW);
	}

	inline bool Power::operator >=(const Power& p) const {
		return (mW >= p.mW);
	}

	inline bool Power::operator ==(const Power& p) const {
		return (mW == p.mW);
	}

	inline bool Power::operator !=(const Power& p) const {
		return (mW != p.mW);
	}

}

inline std::string operator +(std::string &str, const wns::Power& p)
{
	std::ostringstream str2;
	str2 << str << p.get_dBm();
	return str2.str();
}

inline std::string operator +(std::string &str, const wns::Ratio& r)
{
	std::ostringstream str2;
	str2 << str << r.get_dB();
	return str2.str();
}

#endif  // _POWER_RATIO_HPP

/**
 * @file
 */


