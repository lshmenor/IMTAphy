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

#include <WNS/Chamaeleon.hpp>

using namespace wns;

ChamaeleonBase::ChamaeleonBase(const TypeInfo& typeInfo) throw() :
	typeInfo_(typeInfo)
{
}

ChamaeleonBase::~ChamaeleonBase() throw()
{
}

TypeInfo
ChamaeleonBase::getTypeInfo() const throw()
{
	return typeInfo_;
}

ChamaeleonBase::BadCast::BadCast(
    const TypeInfo& expectedType,
    const TypeInfo& actualType) throw() :
    Exception()
{
    (*this) << "Wrong type for conversion specified! The expected (wanted/specified) type is: "
            << expectedType
            << "\nThe actual type of the chamaeleonized data is: " << actualType;
}

ChamaeleonBase::BadCast::~BadCast() throw()
{
}

