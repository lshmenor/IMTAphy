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

#ifndef WNS_BACKTRACE_HPP
#define WNS_BACKTRACE_HPP

#include <WNS/IOutputStreamable.hpp>
#include <vector>
#include <list>

namespace wns {

    /**
     * @brief Makes a snapshot of the current stack
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * Usage:
     * @include wns.Backtrace.example
     */
    class Backtrace :
        virtual public IOutputStreamable
    {
        typedef std::list<std::string> Lines;
    public:
        /**
         * @brief Keeps the information an element of the backtrace (for one
         * function call)
         * @author Marc Schinnenburg <marc@schinnenburg.com>
         *
         * @note If the line of a backtrace cannot be parsed completely, the
         * elements which were not found return "unknown" as string.
         */
        class FunctionCall
        {
        public:
            /**
             * @brief Takes a line as provided from
             * char ** backtrace_symbols (void *const *buffer, int size)
             * of execinfo.h
             */
            explicit
            FunctionCall(std::string line);

            /**
             * @brief Origing of the function (library or whatever)
             */
            std::string
            getOrigin() const;

            /**
             * @brief The name of the function called in this frame
             */
            std::string
            getName() const;

            /**
             * @brief Offset into the function
             */
            std::string
            getOffset() const;

            /**
             * @brief Return address of the function
             */
            std::string
            getReturnAddress() const;

        private:
            std::string origin_;
            std::string name_;
            std::string offset_;
            std::string returnAddress_;
        };

        typedef std::vector<FunctionCall> FunctionCalls;

        /**
         * @brief Creates an empty object, use snapshot(), to get a snapshot of
         * the stack
         */
        Backtrace();

        /**
         * @brief Destructor
         */
        virtual
        ~Backtrace();

        /**
         * @brief Make a snapshot of the stack, discarding any old snapshot
         */
        void
        snapshot();

        /**
         * @brief Discard snapshot
         */
        void
        clear();

        /**
         * @brief Returns a list with function calls
         */
        FunctionCalls
        getFunctionCalls() const;

    private:
        /**
         * @brief Retuns a human readable String with demangled symbols
         * @note This is part of an NVI. Call toString() if you want this
         */
        virtual std::string
        doToString() const;

        /**
         * @brief Holds the backtrace
         */
        Lines backtrace_;
    };
}

#endif // NOT defined WNS_BACKTRACE_HPP
