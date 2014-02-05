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

#ifndef _ISCLASS_HPP
#define _ISCLASS_HPP

namespace wns
{

    /** @brief Check if object @a o is an instance of class @a C.
     * 
     *  @param C Class to check.
     *  @param o Object to check.
     *  @return true, if @a o is an instance of class @a C or a subclass,
     *     false otherwise.
     *
     *  @c isClass<aClass>(anObject) will check, if @c anObject
     *  is an instance of class @c aClass or one of its subclasses
     *  and return true, if this is the case. @c aClass must be
     *  polymorphic.
     */
    template<class C, class T>
    inline bool isClass(const T& o)
    {
	return (dynamic_cast<const C *const>(&o) != 0);
    }

}

#endif // _ISCLASS_HPP
