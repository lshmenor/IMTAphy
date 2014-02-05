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

#ifndef WNS_SIMULATOR_APPLICATION_HPP
#define WNS_SIMULATOR_APPLICATION_HPP

#include <WNS/simulator/IApplication.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/events/scheduler/Monitor.hpp>
#include <WNS/simulator/ISimulationModel.hpp>
//#include <WNS/module/VersionInformation.hpp>
#include <WNS/probe/bus/ProbeBusRegistry.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/simulator/StatusReport.hpp>
#include <WNS/simulator/ProbeWriter.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <vector>
#include <string>
#include <memory>

namespace wns {

	namespace module {
		class Base;
	}
}

namespace wns {  namespace simulator {

	class ModuleDependencyMismatchException :
		public wns::Exception
	{
	public:
		ModuleDependencyMismatchException();
                ~ModuleDependencyMismatchException() throw() {}
	};


    /**
     * @brief Run-time environment of openWNS
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * The following mehtods are called in this order:
     *  - Application()
     *  - readCommandLine()
     *  - init()
     *  - run()
     *  - shutdown()
     *  - status()
     *  - ~Application()
     */
    class Application :
        public IApplication
    {
        // needs to be vector to work with boost
        typedef std::vector<std::string> TestNameContainer;
        typedef std::vector<std::string> PyConfigPatchContainer;

    public:
        /**
         * @brief Default Constructor
         */
        Application();

        /**
         * @brief Default Destructor
         */
        virtual
        ~Application();

    protected:
        /**
         * @brief Reads the command line parameters
         */
        virtual void
        doReadCommandLine(int argc, char* argv[]);

        /**
         * @brief Configure stuff that belongs to WNS
         */
        virtual void
        doInit();

        /**
         * @brief Run the simulation
         */
        virtual void
        doRun();

        /**
         * @brief Shutdown everything
         */
        virtual void
        doShutdown();

        /**
         * @brief Return status code
         */
        virtual int
        doStatus() const;

        /**
         * @brief Handle unexpected exceptions
         */
        static void
        unexpectedHandler();

        /**
         * @brief Returns the path to pyconfig
         */
        static std::string
        getPathToPyConfig();

        /**
         * @brief Returns the view to root configuration
         */
	    pyconfig::View
        getWNSView() const;

        /**
         * @brief Disable usage of 80 bit extensions in x87
         *
         * From: http://www.network-theory.co.uk/docs/gccintro/gccintro_70.html
         *
         * The IEEE-754 standard defines the bit-level behavior of
         * floating-point arithmetic operations on all modern processors. This
         * allows numerical programs to be ported between different platforms
         * with identical results, in principle. In practice, there are often
         * minor variations caused by differences in the order of operations
         * (depending on the compiler and optimization level) but these are
         * generally not significant.
         *
         * However, more noticeable discrepancies can be seen when porting
         * numerical programs between x86 systems and other platforms, because
         * the the x87 floating point unit (FPU) on x86 processors computes
         * results using extended precision internally (the values being
         * converted to double precision only when they are stored to
         * memory). In contrast, processors such as SPARC, PA-RISC, Alpha, MIPS
         * and POWER/PowerPC work with native double-precision values
         * throughout. The differences between these implementations lead to
         * changes in rounding and underflow/overflow behavior, because
         * intermediate values have a greater relative precision and exponent
         * range when computed in extended precision. In particular,
         * comparisons involving extended precision values may fail where the
         * equivalent double precision values would compare equal.
         *
         * To avoid these incompatibilities, the x87 FPU also offers a hardware
         * double-precision rounding mode. In this mode the results of each
         * extended-precision floating-point operation are rounded to double
         * precision in the floating-point registers by the FPU. It is important
         * to note that the rounding only affects the precision, not the
         * exponent range, so the result is a hybrid double-precision format
         * with an extended range of exponents.
         *
         * On BSD systems such as FreeBSD, NetBSD and OpenBSD, the hardware
         * double-precision rounding mode is the default, giving the greatest
         * compatibility with native double precision platforms. On x86
         * GNU/Linux systems the default mode is extended precision (with the
         * aim of providing increased accuracy). To enable the double-precision
         * rounding mode it is necessary to override the default setting on
         * per-process basis using the FLDCW "floating-point load control-word"
         * machine instruction, which will be performed by this method.
         */
        static void
        disableX87ExtendedFloatingPointPrecision();


        /**
         * @brief Retrieve an instance of Module from each library
         */
        void
        loadModules();


        /**
         * @brief Write a fingerprint file to the output directory
         *
         */
        void
        writeFingerprint();

        /**
         * @brief Stop Probes
         */
        void
        stopProbes();


        /**
         * @brief The status code of openWNS
         *
         * 0 - everything fine
         *
         * 1 - an error occured
         */
        int status_;

        /**
         * @brief Name of the configuration file
         */
        std::string configFile_;

        /**
         * @brief Configuration as parsed from configFile_
         */
        pyconfig::Parser configuration_;

        /**
         * @brief Be chatty
         */
        bool verbose_;

        /**
         * @brief Is wns in testing mode?
         */
        bool testing_;

        /**
         * @brief Is the output in testing mode compatible to that of a compiler?
         */
        bool compilerTestingOutput_;

        /**
         * @brief If special tests have been defined they are stored here
         */
        TestNameContainer testNames_;

        /**
         * @brief Holds all PyConfig patches
         */
        PyConfigPatchContainer pyConfigPatches_;

        /**
         * @brief Defines the valid options for the command line
         */
        boost::program_options::options_description options_;

        /**
         * brief Holds the command line parameters
         */
        boost::program_options::variables_map arguments_;

        /**
         * @brief Our own name (as from argv[0])
         */
        std::string programName_;

        /**
         * @brief In case we should launch a debugger, this is the name of it
         */
        std::string debuggerName_;

        /**
         * @brief Are we expected to launch a debugger on segfault?
         */
        bool attachDebugger_;

        /**
         * @brief Interactive config
         */
        bool interactiveConfig_;

        /**
         * @brief The logger of the Application
         *
         * @note The logger can only be used after init and before shutdown
         */
		wns::logger::Logger logger_;

        /**
         * @brief A monitor for the event scheduler
         */
        std::auto_ptr<wns::events::scheduler::Monitor> eventSchedulerMonitor_;

        /**
         * @brief If true use 80 bit extensions of x87 (false by default)
         */
        bool extendedPrecision_;

        /**
         * @brief Keeps the SimulationModel which is constructed by a StaticFactory
         */
        std::auto_ptr<wns::simulator::ISimulationModel> simulationModel_;

        /**
         * @brief List with the configuration of all modules
         */
		std::list<pyconfig::View> moduleViews_;

        /**
         * @brief If true, a list of loaded modules will be shown (set by "-m")
         */
		bool listLoadedModules_;

        /**
         * @brief List with all loaded modules
         */
		std::list<wns::module::Base*> loadedModules_;

		bool lazyBinding_;

		bool absolutePath_;

        /**
         * @brief Anchor for all probe busses
         */
        std::auto_ptr<wns::probe::bus::ProbeBusRegistry> probeBusRegistry;

        StatusReport statusReport;

        ProbeWriter probeWriter;

        /**
        * @brief Probe CPU cycles spent in main event loop
        */
        wns::probe::bus::ContextCollectorPtr cpuCyclesProbe_;
    };

} // simulator
} // wns

#endif // NOT defined WNS_SIMULATOR_APPLICATION_HPP

