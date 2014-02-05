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

#ifndef WNS_ASSURE_HPP
#define WNS_ASSURE_HPP

#include <WNS/Exception.hpp>
#include <WNS/TypeInfo.hpp>

#include <cassert>
#include <sstream>
#include <iostream>
#include <string>
#include <csignal>

/**
 * @def assure
 * @brief An extended assert
 *
 * assure is very much like assert except for two differences:
 *
 * 1.) Additionally to the expression to be checked a reason in case the
 * expression evaluates to "false" can be provided. Example:
 *
 * @code
 * assure(container.find(x) != container.end(), "x is not contained in container");
 * @endcode
 *
 * 2.) The assure is configurable at three levels:
 *
 * @li Throw on error: It throws an wns::Assure::Exception with the reason as
 * content. If wns::Assure::useSIGTRAP is set to "true", the debugger will stop
 * exactly at the point where the exception is thrown.
 *
 * @li Abort on error: It behaves like assert, the reason will NOT be visible
 *
 * @li Don't check: The expression will not be evaluated and therefor not
 * checked. This can e.g. be used when compiling optimized. Note, that it is
 * therefor dangereous to use non-const (and even const) method of an object
 * inside the assure expression as these may change the behaviour of the
 * program in case you have the checks disabled.
 *
 * The behavior of the macro must be configured at compile time. To throw on
 * error you don't need to define anything - this is the defaul. To make assure
 * behave like assert (abort on error) define WNS_ASSERT. To disable the
 * checking define WNS_NDEBUG.
 *
 * @note The WNS build system defines nothing if "dbg" is used as build flavour
 * and defines WNS_NDEBUG if "opt" is used as build flavour.
 *
 * @note Use WNS_ASSERT_ASSURE_EXCEPTION and
 * WNS_ASSERT_ASSURE_NOT_NULL_EXCEPTION instead of CPPUNIT_ASSERT_THROW to
 * prevent, that your test fails in opt flavours.
 *
 * @note If you have other code, that depends on an exception thrown by
 * an assure, enclose this code with an ifdef:
 * @code
 * #ifdef WNS_ASSURE_THROWS_EXCEPTION
 *         yourCode;
 * #endif.
 * @endcode
 */

/**
 * @def assureWithExceptionType
 * @brief This assure allows to specifiy the exception type to be thrown
 */

// WNS_NDEBUG means: assures are disabled
#if defined WNS_NDEBUG
#define assure(expr, reason) \
((void)0)

#define assureWithExceptionType(expr, reason, type) \
((void)0)

// WNS_ASSERT means: assure behaves like assert -> abort if expression is not true
#elif defined WNS_ASSERT
#define assure(expr, reason) \
if (true) \
{ \
    assert(expr); \
} \
else \
    void()

#define assureWithExceptionType(expr, reason, type) \
if (true) \
{ \
    assert(expr); \
} \
else \
    void()

// if nothing is defined: assure throws an exception if expression is not true
#else
#define assure(expr, reason) \
if (!(expr)) \
{ \
    std::stringstream s; \
    s.precision(10); \
    s<<reason; \
    wns::Assure::expression<wns::Assure::Exception>(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__, (s.str())); \
} \
else \
    void()

#define assureWithExceptionType(expr, reason, type) \
if (!(expr)) \
{ \
    std::stringstream s; \
    s.precision(10); \
    s<<reason; wns::Assure::expression<type>(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__, (s.str())); \
} else \
    void()

#define WNS_ASSURE_THROWS_EXCEPTION 1

#endif // WNS_NDEBUG


/**
 * @brief Assure "pointer" is of type T
 *
 * @author Marc Schinnenburg <marc@schinnenburg.com>
 *
 * @code
 * Baz* foo = getBaz();
 * assureType(foo, Bar*);
 * @endcode
 *
 * Provides the following error message if pointer is NOT of type T:
 * @verbatim foo is not of type wns::Bar* but of type wns::Baz* @endverbatim
 */
#define assureType(pointer, T) \
assure(dynamic_cast<T>(pointer), \
       std::string(#pointer) + " is not of type " + wns::TypeInfo::create<T>().toString() + \
                   " but of Type: " + wns::TypeInfo::create(*pointer).toString()+"*")

/**
 * @brief Assure pointer is not NULL
 *
 * @author Marc Schinnenburg <marc@schinnenburg.com>
 *
 * This should be prefered over:
 * @code
 * assure (NULL != pointer);
 * @endcode
 * since it adds additional information about the type of the pointer automatically.
 */
#define assureNotNull(pointer) \
assureWithExceptionType(NULL != pointer, \
               "Pointer " + std::string(#pointer) + " invalid (NULL)", wns::NullException)

namespace wns {

    /**
     * @brief Thrown by @ref assureNotNull
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    class NullException :
        public wns::Exception
    {
    public:
        /**
         * @brief @ref assureNotNull provides additional information about the
         * pointer via this c'tor
         */
        explicit
        NullException(const std::string& s);
        ~NullException() throw() {}
    };


    /**
     * @brief Pseudo class grouping the assure functionality
     *
     * @internal This is not a namespace to not clutter the top level
     * namespace with it.
     */
    class Assure
    {
    public:
        /**
         * @brief If enabled, cerr and SIGTRAP will be used instead of
         * throwing an error
         *
         * This allows to stop the debugger on an exception.
         */
        static bool useSIGTRAP;

        /**
         * @brief Thrown by @ref assure
         *
         * @author Marc Schinnenburg <marc@schinnenburg.com>
         */
        class Exception :
            public wns::Exception
        {
        public:
            /**
             * @brief @ref assure provides additional information about the
             * pointer via this c'tor
             */
            explicit
            Exception(const std::string& s);

	  ~Exception() throw() {};
        };

        /**
         * @brief Used by @ref assure(bool anExpression, string reason)
         *
         * @author Marc Schinnenburg <marc@schinnenburg.com>
         * @author Swen Kuehnlein
         *
         * @throws wns::Assure::Exception Indicates a failed assertion. This is
         * more verbose than assert() as you can provide any information you
         * like as message why the assure failed.
         */
        template<typename TYPE>
        static void
        expression(
            const std::string& expression,
            const std::string& fileName,
            int line,
            const std::string& functionName,
            const std::string& reason)
        {
            std::stringstream ss;
            ss << "In '"
               << functionName
               << "' ("
               << fileName
               << ":"
               << line
               << "):\nfailed assertion (" << expression << ")." << std::endl
               << "Reason: " << reason << std::endl;

            if (useSIGTRAP)
            {
                std::cerr << ss.str();
                raise(SIGTRAP);
            }
            else
            {
                TYPE e(ss.str());
                throw(e);
            }
        }

    };
}

#endif // WNS_ASSURE_HPP
