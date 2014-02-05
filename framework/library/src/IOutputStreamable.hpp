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

#ifndef WNS_IOUTPUTSTREAMABLE_HPP
#define WNS_IOUTPUTSTREAMABLE_HPP

#include <string>
#include <sstream>

namespace wns {

    /**
     * @brief Enable usage in output streams by deriving from this class (NVI)
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @ingroup group_main_classes
     */
    class IOutputStreamable
    {
    public:
        /**
         * @brief Virtual d'tor
         */
        virtual
        ~IOutputStreamable()
        {}

        /**
         * @brief Called to generate the output
         *
         * This is a NVI. Overload doToString() to customize the output
         */
        std::string
        toString() const;

    private:
        /**
         * @brief Overload to customize the representation of your class
         */
        virtual std::string
        doToString() const = 0;
    };

    /**
     * @brief Ouput stream operator for IOutputSteamable
     */
    std::ostream&
    operator<<(std::ostream& other, const wns::IOutputStreamable& outputStreamable);
} // wns



#endif // NOT defined WNS_IOUTPUTSTREAMABLE_HPP
