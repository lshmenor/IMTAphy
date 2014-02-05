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

#ifndef WNS_PYCONFIG_PYOBJECT_HPP
#define WNS_PYCONFIG_PYOBJECT_HPP

#include <string>

#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif

namespace wns { namespace pyconfig {

    class Object
    {
    public:
        Object();

        explicit
        Object(PyObject* obj);

        /**
         * @brief "str(obj)" in Python
         */
        std::string
        toString() const;

        /**
         * @brief decrease refcount
         */
        void
        decref() const;

        /**
         * @brief increase refcount
         */
        void
        incref() const;

        /**
         * @brief True if obj can be converted to a string
         */
        bool
        isConvertibleToString() const;

        /**
         * @brief True if obj is a sequence
         */
        bool
        isSequence() const;

        /**
         * @brief "obj == None" in Python
         */
        bool
        isNone() const;

        /**
         * @brief True if Internal obj is NULL
         */
        bool
        isNull() const;

        /**
         * @brief "obj[n]" in Python
         */
        Object
        getItem(int n) const;

        /**
         * @brief 1 if True, 0 if False, else on error
         */
        int
        isTrue() const;


        /**
         * @brief compare internal pointer
         */
        bool
        operator ==(const Object& other);

        /**
         * @brief compare internal pointer
         */
        bool
        operator !=(const Object& other);

        PyObject* obj_;
    };
}
}

#endif
