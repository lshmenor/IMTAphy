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

#ifndef WNS_SIMULATOR_SEGMENTATIONVIOLATIONHANDLER_HPP
#define WNS_SIMULATOR_SEGMENTATIONVIOLATIONHANDLER_HPP

#include <WNS/simulator/SignalHandlerCallback.hpp>
#include <string>

namespace wns { namespace simulator {

    /**
     * @brief This signal handler will catch segmentation faults try to print
     * the current stack and attach the GNU Debugger 'gdb' if enabled.
     * It exits with -SIGSEGV.
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    class SegmentationViolationHandler :
        public SignalHandlerCallback
    {
    public:
        SegmentationViolationHandler(
            bool attachDebugger,
            const std::string& debuggerName,
            const std::string& programName);

    private:
        virtual void
        doCall();

        bool attachDebugger_;
        std::string debuggerName_;
        std::string programName_;
    };

} // simulator
} // wns

#endif // NOT defined WNS_SIMULATOR_SEGMENTATIONVIOLATIONHANDLER_HPP
