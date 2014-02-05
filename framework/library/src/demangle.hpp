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

#ifndef WNS_DEMANGLE_HPP
#define WNS_DEMANGLE_HPP

#include <string>

namespace wns {

    /**
     * @brief Try to demangle "symbol".
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * If symbol is not a vaild name under the rules of the C++ ABI mangling
     * rules, symbol is returned unmodified. If demangling is not available on
     * your system at all this method will also return the symbol name as
     * fallback.
     *
     * Currently demangling is supported for system where _GNU_EXTENSIONS is
     * defined.
     */
    std::string
    demangle(const std::string& symbol);
}

#endif // WNS_DEMANGLE_HPP
