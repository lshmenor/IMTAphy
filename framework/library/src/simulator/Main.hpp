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

#ifndef WNS_SIMULATOR_MAIN_HPP
#define WNS_SIMULATOR_MAIN_HPP

#include <WNS/simulator/IApplication.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/logger/Master.hpp>

namespace wns { namespace simulator {

    /**
     * @brief A Warpper for any IApplication
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * Creates an instance of APPLICATION, which reads
     * command line parameters and starts the simulation.
     * Additionally Main performs exception handling. Every uncaught
     * wns::Exception or std::exception is caught in run() and will be displayed
     * via "std::cerr".
     *
     * Usage:
     * @include wns.simulator.Main.example
     */
    template <typename APPLICATION>
    class Main :
        public IApplication
    {
    public:
        typedef APPLICATION Application;

        Main();

    private:
        /**
         * @name NVI-Implementation (IApplication)
         */
        //{@
        virtual void
        doReadCommandLine(int argc, char* argv[]);

        virtual void
        doInit();

        virtual void
        doRun();

        virtual void
        doShutdown();

        virtual int
        doStatus() const;
        //@}

        /**
         * @name Exception handling
         */
        //{@
        void
        handleWNSException(bool showLoggerBacktrace, const wns::Exception& exception) const;

        void
        handleSTDException(bool showLoggerBacktrace, const std::exception& exception) const;

        void
        handleAnyException(bool showLoggerBacktrace) const;
        //@}

        /**
         * @brief An instance of the application we're running
         */
        Application application_;
    };


    template <typename APPLICATION>
    Main<APPLICATION>::Main() :
        IApplication(),
        application_()
    {
    }


    template <typename APPLICATION>
    void
    Main<APPLICATION>::doReadCommandLine(int argc, char* argv[])
    {
        try
        {
            return application_.readCommandLine(argc, argv);
        }
        catch (const wns::Exception& exception)
        {
            handleWNSException(false, exception);
        }
        catch (const std::exception& exception)
        {
            handleSTDException(false, exception);
        }
        catch (...)
        {
            handleAnyException(false);
        }
    }

    template <typename APPLICATION>
    void
    Main<APPLICATION>::doInit()
    {
        try
        {
        return application_.init();
        }
        catch (const wns::Exception& exception)
        {
            handleWNSException(false, exception);
        }
        catch (const std::exception& exception)
        {
            handleSTDException(false, exception);
        }
        catch (...)
        {
            handleAnyException(false);
        }
    }

    template <typename APPLICATION>
    void
    Main<APPLICATION>::doRun()
    {
        try
        {
            return application_.run();
        }
        catch (const wns::Exception& exception)
        {
            handleWNSException(true, exception);
        }
        catch (const std::exception& exception)
        {
            handleSTDException(true, exception);
        }
        catch (...)
        {
            handleAnyException(true);
        }
    }

    template <typename APPLICATION>
    void
    Main<APPLICATION>::doShutdown()
    {
        try
        {
            return application_.shutdown();
        }
        catch (const wns::Exception& exception)
        {
            handleWNSException(false, exception);
        }
        catch (const std::exception& exception)
        {
            handleSTDException(false, exception);
        }
        catch (...)
        {
            handleAnyException(false);
        }
    }

    template <typename APPLICATION>
    int
    Main<APPLICATION>::doStatus() const
    {
        try
        {
            return application_.status();
        }
        catch (const wns::Exception& exception)
        {
            handleWNSException(false, exception);
        }
        catch (const std::exception& exception)
        {
            handleSTDException(false, exception);
        }
        catch (...)
        {
            handleAnyException(false);
        }
        // if reach this something went wrong
        return 1;
    }


    template <typename APPLICATION>
    void
    Main<APPLICATION>::handleWNSException(bool showLoggerBacktrace, const wns::Exception& exception) const
    {
        if (showLoggerBacktrace)
        {
            wns::simulator::getMasterLogger()->outputBacktrace();
        }
        std::stringstream message;
        message << exception.getBacktrace()
                << "openWNS: Caught " << wns::TypeInfo::create(exception) << ":\n\n"
                << exception.what();
        std::cerr << message.str() << "\n\n";
        exit(1);
    }


    template <typename APPLICATION>
    void
    Main<APPLICATION>::handleSTDException(bool showLoggerBacktrace, const std::exception& exception) const
    {
        if (showLoggerBacktrace)
        {
            wns::simulator::getMasterLogger()->outputBacktrace();
        }
        std::stringstream message;
        message << "openWNS: Caught " << wns::TypeInfo::create(exception) << ":\n\n"
                << exception.what();
        std::cerr << message.str() << "\n\n";
        exit(1);
    }


    template <typename APPLICATION>
    void
    Main<APPLICATION>::handleAnyException(bool showLoggerBacktrace) const
    {
        if (showLoggerBacktrace)
        {
            wns::simulator::getMasterLogger()->outputBacktrace();
        }
        std::cerr << "openWNS: An unknown exception occurred.\n";
        exit(1);
    }

} // simulator
} // wns

#endif // NOT defined WNS_SIMULATOR_MAIN_HPP
