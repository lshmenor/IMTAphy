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

#ifndef WNS_PYTHONICOUTPUT_HPP
#define WNS_PYTHONICOUTPUT_HPP

#include <WNS/IOutputStreamable.hpp>

namespace wns {

    /**
     * @brief Provide default (pythonic) output for C++ objects
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @ingroup group_main_classes
     *
     * This class is named PythonicOuput because the format of the output being
     * generated is very similar to the output an object in Python generates, if
     * it is asked to cast itself to a string but has no special operator for this.
     *
     * If you derive (virtual) from this class your class will show nice and
     * informative output if it is used in a output stream like this:
     * @code
     * mynamespace::MyClass myObject;
     * cout << myObject;
     * @endcode
     * Results in:
     * @verbatim <mynamespace::MyClass instance at 0xb623bfd> @endverbatim
     *
     * If you want something else, just overload "std::string doToString()" which is
     * provided by IOutputStreamable and reimplemented in this class.
     * Examples:
     *
     * To achive the default output:
     * @include wns.PythonicOutput.default.example
     * To achive user defined output:
     * @include wns.PythonicOutput.special.example
     */
    class PythonicOutput :
        virtual public IOutputStreamable
    {
    protected:
        /**
         * @brief Provides pythonic output
         *
         * Provides the following (pythonic) output:
         * @verbatim <mynamespace::MyClass instance at 0xb623bfd> @endverbatim
         */
        virtual std::string
        doToString() const;
    };

} // wns



#endif // NOT defined WNS_PYTHONICOUTPUT_HPP
