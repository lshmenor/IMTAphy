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

#ifndef WNS_ENUM_HPP
#define WNS_ENUM_HPP

#include <WNS/TypeInfo.hpp>
#include <WNS/Assure.hpp>

#include <iostream>
#include <string>
#include <map>
#include <stdint.h>

// Start your enum section with this macro
#define ENUM_BEGIN(NAMESPACE) namespace NAMESPACE {\
template <int T, typename dummy> class EnumItem {};\
class _Enums {\
public:\
	typedef std::map<std::string, int> S2I;\
	typedef std::map<int, std::string> I2S;\
\
	static S2I& getS2I() {\
		static S2I s2i;\
		return s2i;\
	}\
\
	static I2S& getI2S() {\
		static I2S i2s;\
		return i2s;\
	}\
};\
\
struct EnumRegister : private _Enums {\
	 EnumRegister(const std::string& s, int i) {\
		 _Enums::getS2I()[s]=i;\
		 _Enums::getI2S()[i]=s;\
	 }\
};\
inline int fromString(const std::string& s) {\
        _Enums::S2I& s2i = _Enums::getS2I();\
        _Enums::S2I::iterator itr = s2i.find(s);\
	assure(itr != s2i.end(), "No Enum with this name (\""<<s<<"\")!");	\
        return itr->second;\
}\
inline int fromString(const char* c) {\
        return fromString(std::string(c));\
}\
\
inline std::string toString(int i) {\
        _Enums::I2S& i2s = _Enums::getI2S();\
        _Enums::I2S::iterator itr = i2s.find(i);\
	assure(itr != i2s.end(), "No Enum with this integer value ("<<i<<")!"); \
        return itr->second;\
} \
class ThisClassDoesNotExist /* to allow semicolon */

// End your enum section with this macro
#define ENUM_END() } \
class ThisClassDoesNotExist /* to allow semicolon */

// Define your enums with this macro
#define ENUM(NAME, NUMBER)\
/* The dummy template parameter type allows definition of the static member in
   the header-file*/\
template <typename dummy> class EnumItem<NUMBER, dummy> : public _Enums \
{\
public:\
    operator int() {return NUMBER;}\
private:\
    static EnumRegister reg;\
};\
typedef EnumItem<NUMBER, wns::NullType> NAME;\
/* explicit template instantiation to call the EnumRegister c'tor*/\
template <typename dummy> EnumRegister EnumItem<NUMBER, dummy>::reg = EnumRegister(#NAME, NUMBER);\
template class EnumItem<NUMBER, wns::NullType>


#endif //_ENUM_HPP

/**
 * @file
 */


