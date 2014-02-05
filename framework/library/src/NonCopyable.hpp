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

#ifndef WNS_NONCOPYABLE_HPP
#define WNS_NONCOPYABLE_HPP

namespace wns {

    /**
     * @brief Prohibits copy construction and copy assignment for a class "A" if you
     * derive "A" from this class
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * This has been taken from the documentation of boost::noncopyable:
     *
     * Some objects, particularly those which hold complex resources like files
     * or network connections, have no sensible copy semantics.  Sometimes there
     * are possible copy semantics, but these would be of very limited
     * usefulness and be very difficult to implement correctly.  Sometimes
     * you're implementing a class that doesn't need to be copied just yet and
     * you don't want to take the time to write the appropriate functions.
     * Deriving from NonCopyable will prevent the otherwise implicitly-generated
     * functions (which don't have the proper semantics) from becoming a trap
     * for other programmers.
     *
     * The traditional way to deal with these is to declare a private copy
     * constructor and copy assignment, and then document why this is done.  But
     * deriving from NonCopyable is simpler and clearer, and doesn't require
     * additional documentation.
     */
    class NonCopyable
    {
    protected:
        /**
         * @brief Avoid instantiation of NonCopyable object (useless)
         */
        NonCopyable()
        {
        }

        /**
         * @brief Avoid deletion of NonCopyable object (symmetric with c'tor)
         */
        virtual
        ~NonCopyable()
        {
        }

    private:
        /**
         * @brief Private and not implemented to forbid copy construction
         */
        NonCopyable(const NonCopyable&);

        /**
         * @brief Private and not implemented to forbid assignment
         */
        NonCopyable&
        operator =(const NonCopyable&);
    };
}

#endif // NOT defined WNS_NONCOPYABLE_HPP
