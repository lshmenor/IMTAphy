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

#include <WNS/demangle.hpp>

# if defined(__GNUC__) && __GNUC__ >= 3
// Demangle using cxxabi.h
#include <malloc.h>
#include <cxxabi.h>
std::string
wns::demangle(const std::string& symbol)
{
    // demangle the symbol name
    // mangeld symbol names are always longer than the demangled versions
    int status = 0;
    char* demangledName = abi::__cxa_demangle(symbol.c_str(), NULL, NULL, &status);

    // Mangling did not succeed. Return the symbol name instead.
    if (status != 0)
    {
        // possible reasons are:
        //    -1: A memory allocation failure occurred
        //    -2: "symbol" is no valid name under the C++ ABI mangling rules
        //    -3: One of the arguments is invalid.
        return symbol;
    }

    // Mangling succeeded.
    // Copy into string since ...
    std::string result (demangledName);
    // ... we need to free the memory before returning
    free(demangledName);
    return result;
}

#else
// Can't demangle
std::string
wns::demangle(const std::string& symbol)
{
    return symbol;
}
#endif // _GNU_SOURCE
