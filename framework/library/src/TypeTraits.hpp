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

#ifndef WNS_TYPETRAITS_HPP
#define WNS_TYPETRAITS_HPP

#include <WNS/TypeInfo.hpp>

namespace wns {
    /**
     * @brief A template to check various aspects of a type
     */
    template <typename T>
    class TypeTraits
    {
    private:
        template <class U>
        struct PointerTraits
        {
            enum
            {
                result = 0
            };
            typedef NullType PointeeType;
        };
        template <class U>
        struct PointerTraits<U*>
        {
            enum
            {
                result = 1
            };
            typedef U PointeeType;
        };

        template <class U>
        struct ReferenceTraits
        {
            enum
            {
                result = 0
            };
            typedef NullType ReferenceType;
        };
        template <class U>
        struct ReferenceTraits<U&>
        {
            enum
            {
                result = 1
            };
            typedef U ReferenceType;
        };

        template <class U>
        struct ConstTraits
        {
            enum
            {
                result = 0
            };
            typedef NullType ConstType;
        };
        template <class U>
        struct ConstTraits<const U>
        {
            enum
            {
                result = 1
            };
            typedef U ConstType;
        };

        // The following ConstTraits shouldn't be necessary, but somehow g++
        // doesn't figure out the correct type ...
        template <class U>
        struct ConstTraits<const U*>
        {
            enum
            {
                result = 1
            };
            typedef U ConstType;
        };
        template <class U>
        struct ConstTraits<U&>
        {
            enum
            {
                result = 0
            };
            typedef U ConstType;
        };
        template <class U>
        struct ConstTraits<const U&>
        {
            enum
            {
                result = 1
            };
            typedef U ConstType;
        };

    public:
        /**
         * @brief Is type a pointer?
         */
        enum
        {
            isPointer = PointerTraits<T>::result
        };
        typedef typename PointerTraits<T>::PointeeType PointeeType;

        /**
         * @brief Is type a reference?
         */
        enum
        {
            isReference = ReferenceTraits<T>::result
        };
        typedef typename ReferenceTraits<T>::ReferenceType ReferenceType;

        /**
         * @brief Is type const?
         */
        enum
        {
            isConst = ConstTraits<T>::result
        };
        typedef typename ConstTraits<T>::ConstType ConstType;

        /**
         * @brief Return properties of type as octal
         */
        enum
        {
            propNumber = 4*isPointer + 2*isReference + 1*isConst
        };

        /**
         * @brief Represent types in octal notation
         */
        enum
        {
            Plain = 0,
            Const = 1,
            Reference = 2,
            ConstReference = 3,
            Pointer = 4,
            ConstPointer = 5
        };
    };
}

#endif // NOT defined WNS_TYPETRAITS_HPP
