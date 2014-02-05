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
#include <WNS/simulator/Main.hpp>
#include <WNS/simulator/Application.hpp>

namespace wns { namespace simulator { namespace tests {

    /**
     * @brief This class is just a compile test (it will not be called or instantiated)
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    class MainTest
    {
        // begin example "wns.simulator.Main.example"
        int main(int argc, char* argv[])
        {
            // create an instance of the wns::simulator::Application
            wns::simulator::Main<wns::simulator::Application> app;

            // parse the command line
            app.readCommandLine(argc, argv);

            // after this the EventScheduler and MasterLogger are available and can be used
            app.init();

            // this starts the main loop (of the EventScheduler) until no more events
            // are available or a stop event is queued
            app.run();

            // shutdown the simulator (EventScheduler, MasterLogger, ...)
            app.shutdown();

            // Finally return the status
            return app.status();
        }
        // end example

    private:
        MainTest();
        ~MainTest();
    };

} // tests
} // simulator
} // wns
