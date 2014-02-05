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

#ifndef WNS_SIMULATOR_IAPPLICATION_HPP
#define WNS_SIMULATOR_IAPPLICATION_HPP

#include <WNS/NonCopyable.hpp>

namespace wns { namespace simulator {

    class IApplication :
        private NonCopyable
    {
    public:
        virtual
        ~IApplication();

        void
        readCommandLine(int argc, char* argv[]);

        void
        init();

        void
        run();

        void
        shutdown();

        int
        status() const;

    private:
        virtual void
        doReadCommandLine(int argc, char* argv[]) = 0;

        virtual void
        doInit() = 0;

        virtual void
        doRun() = 0;

        virtual void
        doShutdown() = 0;

        virtual int
        doStatus() const = 0;
    };


} // simulator
} // wns

#endif // NOT defined WNS_SIMULATOR_IAPPLICATION_HPP

