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

#ifndef WNS_EXCEPTION_HPP
#define WNS_EXCEPTION_HPP

#include <WNS/IOutputStreamable.hpp>
#include <WNS/Backtrace.hpp>

#include <sstream>
#include <exception>

namespace wns {
    /**
     * @brief Base class of all WNS exceptions
     * @author Marc Schinnenburg <marc@schinnenburg.net>
     * @ingroup group_main_classes
     *
     * This Exception also contains a backtrace of the program stack as it was
     * at creation time of the Exception. The backtrace can be accesed with
     * exception.getBacktrace(). If NDEBUG is defined the backtrace will be
     * empty (the overhead to generate the backtrace vanishes).
     */
    class Exception :
        virtual public wns::IOutputStreamable,
        virtual public std::exception
    {
    public:
        /**
         * @brief Creates empty Exception (contains no reason)
         */
        Exception();

        /**
         * @brief String s is set as reason
         */
        explicit
        Exception(const std::string& s);

        // Default c'tor is ok

        /**
         * @brief Destructor
         */
        virtual
        ~Exception() throw();

        /**
         * @brief Return the reason as C-style character string
         */
        virtual const char*
        what() const throw();

        /**
         * @brief Return the backtrace of the stack at creation time of this
         * Exception
         *
         * Can be used in streams like this:
         * @code
         * std::cerr << exception.getBacktrace() << std::endl;
         * @endcode
         */
        const Backtrace&
        getBacktrace() const;

        /**
         * @brief Allows using Exceptions like streams
         *
         * Usgae: excp << "Reason: invalid argument";
         */
        template <typename T>
        friend Exception&
        operator <<(Exception& e, const T& t)
        {
            std::stringstream ss;
            ss << t;
            e.reason_.append(ss.str());
            return e;
        }

    protected:
        /**
         * @brief Represent the Exception as std::string
         */
        virtual std::string
        doToString() const;

    private:
        /**
         * @brief String containing the reason for the exception
         */
        std::string reason_;

        /**
         * @brief Backtrace of the current stack, taken at creation of time of
         * the Exception
         */
        Backtrace backtrace_;
    };
}
#endif  // NOT defined WNS_EXCEPTION_HPP
