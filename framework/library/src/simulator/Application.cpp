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

#include <WNS/simulator/Application.hpp>
#include <WNS/simulator/SignalHandler.hpp>

#include <WNS/testing/DetailedListener.hpp>
#include <WNS/logger/Master.hpp>
#include <WNS/Assure.hpp>
#include <WNS/TypeInfo.hpp>
#include <WNS/TestFixture.hpp>
#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/simulator/Simulator.hpp>
#include <WNS/simulator/UnitTests.hpp>
#include <WNS/simulator/OutputPreparation.hpp>
#include <WNS/events/MemberFunction.hpp>

//#include <WNS/module/CurrentVersion.hpp>
#include <WNS/module/Base.hpp>

#include <WNS/simulator/AbortHandler.hpp>
#include <WNS/simulator/InterruptHandler.hpp>
#include <WNS/simulator/SegmentationViolationHandler.hpp>
#include <WNS/simulator/SignalHandler.hpp>
#include <WNS/simulator/CPUTimeExhaustedHandler.hpp>
#ifdef WNS_SMARTPTR_DEBUGGING
#  include <WNS/SmartPtrBase.hpp>
#endif

#ifdef WNS_SMARTPTR_DEBUGGING
#include <WNS/SmartPtrBase.hpp>
#endif

#ifdef WNS_SMARTPTR_DEBUGGING
#include <WNS/SmartPtrBase.hpp>
#endif

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/CompilerOutputter.h>

#include <boost/program_options/value_semantic.hpp>

#include <sys/times.h>

#include <csignal>
#include <dlfcn.h>
#include <iomanip>
#include <memory>
#include <fstream>
#include <climits>
#ifdef CALLGRIND
// For callgrind flavour, to turn on instrumentalisation
#include <valgrind/callgrind.h>
#endif

using namespace wns::simulator;

ModuleDependencyMismatchException::ModuleDependencyMismatchException() :
	wns::Exception("Module dependency not met!\n")
{
}

Application::Application() :
    status_(0),
    configFile_("config.py"),
    configuration_(),
    verbose_(false),
    testing_(false),
    compilerTestingOutput_(false),
    testNames_(),
    pyConfigPatches_(),
    options_(),
    arguments_(),
    programName_("openwns"),
    debuggerName_("gdb"),
    attachDebugger_(false),
    interactiveConfig_(false),
	logger_("WNS", "Application", NULL),
    extendedPrecision_(false),
    moduleViews_(),
	listLoadedModules_(false),
	loadedModules_(),
	lazyBinding_(false),
	absolutePath_(false),
    statusReport(),
    probeWriter()
{
    options_.add_options()

        ("help,?",
         "display this help and exit")

        ("config-file,f",
         boost::program_options::value<std::string>(&configFile_)->default_value("config.py"),
         "load config from configuration file")

        ("attach-debugger-on-segfault,s",
         boost::program_options::value<std::string>(&debuggerName_),
         "fire up gdb on segfault, arg = command for debugger")

        ("stop-in-debugger-on-assure,d",
         boost::program_options::bool_switch(&wns::Assure::useSIGTRAP),
         "stop in debugger if an 'assure' fired (no exception will be thrown)")

        ("interactive-configuration,i",
         boost::program_options::bool_switch(&interactiveConfig_),
         "after reading config start an interactive shell which allows modification of the configuration, use 'continue' to exit shell and run openWNS")

        ("unit-tests,t",
         boost::program_options::bool_switch(&testing_),
         "test mode: run unit tests specified with -T or default suite if no tests with -T given")

        ("compiler-unit-tests,c",
         boost::program_options::bool_switch(&compilerTestingOutput_),
         "the test output is made compiler compatible to improve interworking with your IDE, use with -t")

        ("named-unit-tests,T",
         boost::program_options::value<TestNameContainer>(&testNames_),
         "run named unit test (defined multipple time for multiple tests), e.g. wns::pyconfig::tests::ParserTest, use with -t")

        ("python-path,P",
         "print Python path and exit")

        ("verbose,v",
         boost::program_options::bool_switch(&verbose_),
         "verbose mode (version information and verbose tests)")

        ("patch-config,y",
         boost::program_options::value<PyConfigPatchContainer>(&pyConfigPatches_),
         "patch the configuration with the given Python expression")

        ("extended-precision",
         boost::program_options::bool_switch(&extendedPrecision_),
         "enabled arithmetic operations with extended precision (80 bit) in x87 (disables strict IEEE754 compatibility)")

		("show-modules,M",
         boost::program_options::bool_switch(&listLoadedModules_),
		 "show modules that have been loaded")

		("lazy-linking,l",
		 boost::program_options::bool_switch(&lazyBinding_),
		 "be lazy and link when needed (not at start-up)")
        ;
}

Application::~Application()
{
    std::cout << "\nwns::simulator::Application: shutdown complete\n";
}

void
Application::doReadCommandLine(int argc, char* argv[])
{
    programName_ = std::string(argv[0]);

    boost::program_options::store(
        boost::program_options::parse_command_line(argc, argv, options_),
        arguments_);
    boost::program_options::notify(arguments_);
}

void
Application::doInit()
{
    if (!extendedPrecision_)
    {
        Application::disableX87ExtendedFloatingPointPrecision();
    }

    if (arguments_.count("help") > 0)
    {
        std::cout << options_ << "\n";
        status_ = 0;
        exit(0);
    }

    if (arguments_.count("python-path") > 0)
    {
        std::cout << getPathToPyConfig() << std::endl;
        status_ = 0;
        exit(0);
    }

    // if -s is specified we need to attach the debugger
    attachDebugger_ = arguments_.count("attach-debugger-on-segfault") > 0;

#ifdef __x86_32__
    // Patch path for 32bit Python
    configuration_.patch("import sys\n"
                         "import os\n"
                         "import os.path\n"
                         "newPathOK = os.path.exists('/usr/lib32/python2.6')\n"
                         "oldPathOK = os.path.exists('/usr/lib32/python2.5')\n"
                         "assert newPathOK or oldPathOK, '32bit Python not installed'\n"
                         "if newPathOK:\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.6')\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.6/lib-dynload')\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.6/dist-packages/Numeric')\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.6/dist-packages')\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.6/dist-packages/psycopg2')\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.6/dist-packages/numpy/core')\n"
                         "else:\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.5')\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.5/lib-dynload')\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.5/site-packages/Numeric')\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.5/site-packages')\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.5/site-packages/psycopg2')\n"
                         "    sys.path.insert(0, '/usr/lib32/python2.5/site-packages/numpy/core')\n");
#endif
    
    // patch pyconfig (sys.path, command line patches ...)
    configuration_.appendPath(getPathToPyConfig());
    configuration_.appendPath(".");
    configuration_.patch("__file__ = \""+configFile_+"\"\n"); // Python script should know its own name
    configuration_.load(configFile_);

    // backward compatibility: patch "WNS" variable into the configuration
    configuration_.patch("import openwns\n"
                         "WNS = openwns.getSimulator()\n");

    for(PyConfigPatchContainer::const_iterator it = pyConfigPatches_.begin();
        it != pyConfigPatches_.end();
        ++it)
    {
        configuration_.patch(*it);
    }

    if(interactiveConfig_)
    {
        configuration_.patch("import pdb\n"
                             "import rlcompleter\n"
                             "pdb.Pdb.complete = rlcompleter.Completer(locals()).complete\n"
                             "pdb.set_trace()\n");
    }

    // after pyconfig is patched, bring up Simulator singelton
    if (testing_)
    {
        wns::simulator::getSingleton().setInstance(new wns::simulator::UnitTests(getWNSView()));
    }
    else
    {
        wns::simulator::getSingleton().setInstance(new wns::simulator::Simulator(getWNSView()));
    }

    // reset logger, now set with correct MasterLogger
	pyconfig::View wnsView = getWNSView();
	logger_ = logger::Logger(wnsView.get<wns::pyconfig::View>("logger"));

    // after this we can install the signal handlers as well as the
    // unexpected handlers, because both need the SimulatorSingleton

    // for the backtrace to work we need to set out own unexpected handler
    // for exceptions
    std::set_unexpected(wns::simulator::Application::unexpectedHandler);

    // register signal handlers
    wns::simulator::SignalHandler& handler = wns::simulator::GlobalSignalHandler::Instance();

    // Abort
    handler.addSignalHandler(
        SIGABRT,
        wns::simulator::AbortHandler());

    // Segfault
    handler.addSignalHandler(
        SIGSEGV,
        wns::simulator::SegmentationViolationHandler(
            attachDebugger_,
            debuggerName_,
            programName_));

    // Interrupt (Ctrl+c)
    handler.addSignalHandler(
        SIGINT,
        wns::simulator::InterruptHandler());

    // bring up the Monitor for the event scheduler, may only exists until the
    // master logger is shut down, since it has a logger
    if (!wnsView.isNone("eventSchedulerMonitor"))
    {
        wns::pyconfig::View monitorConfig = wnsView.getView("eventSchedulerMonitor");
        eventSchedulerMonitor_.reset(new wns::events::scheduler::Monitor(monitorConfig));
        eventSchedulerMonitor_->startObserving(wns::simulator::getEventScheduler());
    }

	if (listLoadedModules_)
	{
		std::cout << "The following Modules are available before dynamic loading:" << std::endl;
		for(module::Factory::CreateMap::iterator i = module::Factory::getMap()->begin();
		    i != module::Factory::getMap()->end();
		    ++i)
		{
			std::cout << "\t"<< i->first << std::endl;
		}
	}

	long int nModules = wnsView.len("modules");
	for(int i=0; i<nModules; ++i)
	{
		moduleViews_.push_back(wnsView.getView("modules", i));
	}

	loadModules();

    // prepare probes output directory according to the configured strategy

    pyconfig::View outputStrategyView = wnsView.getView("outputStrategy");

    std::auto_ptr<OutputPreparationStrategy> outputPreparationStrategy(
        OutputPreparationStrategyFactory::creator(
            outputStrategyView.get<std::string>("__plugin__"))->create());

    outputPreparationStrategy->prepare(wnsView.get<std::string>("outputDir"));

    // if we're not in testing mode, we need some special preparation ...
    if (!testing_)
    {
        writeFingerprint();
        MESSAGE_SINGLE(NORMAL, logger_, "Start StatusReport");
        this->statusReport.start(configuration_.getView("WNS"));
        this->statusReport.writeStatus(false);

    }
    // register CPU cycles probe
    cpuCyclesProbe_ = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(
            wnsView.get<std::string>("cpuCyclesProbeBusName")));
}

void
Application::doRun()
{


	// StartUp:
	std::list<module::Base*>::iterator itr;
	std::list<module::Base*>::iterator itrEnd = loadedModules_.end();

	MESSAGE_SINGLE(NORMAL, logger_, "Start up modules");
	for (itr = loadedModules_.begin(); itr != itrEnd; ++itr)
	{
		(*itr)->configure();
	}

    // Unit tests are processed here
    if(testing_)
    {
        CppUnit::TestFactoryRegistry& defaultRegistry = CppUnit::TestFactoryRegistry::getRegistry(wns::testsuite::Default());
        CppUnit::TestSuite* masterSuite = new CppUnit::TestSuite("AllTests");
        CppUnit::TestSuite* defaultSuite = new CppUnit::TestSuite("DefaultTests");
        defaultSuite->addTest(defaultRegistry.makeTest());
        defaultSuite->addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
        masterSuite->addTest(defaultSuite);

        // register disabled tests
        CppUnit::TestFactoryRegistry& disabledRegistry =
            CppUnit::TestFactoryRegistry::getRegistry(wns::testsuite::Disabled());
        masterSuite->addTest(disabledRegistry.makeTest());

        // register performance tests
        CppUnit::TestFactoryRegistry& performanceRegistry =
            CppUnit::TestFactoryRegistry::getRegistry(wns::testsuite::Performance());
        masterSuite->addTest(performanceRegistry.makeTest());

        // register spikes
        CppUnit::TestFactoryRegistry& spikeRegistry =
            CppUnit::TestFactoryRegistry::getRegistry(wns::testsuite::Spike());
        masterSuite->addTest(spikeRegistry.makeTest());

        // setup testrunner
        CppUnit::TextTestRunner runner;
        std::auto_ptr<CppUnit::TestListener> listener;
        if(verbose_)
        {
            listener.reset(new wns::testing::DetailedListener());
        }
        else
        {
            listener.reset(new CppUnit::TextTestProgressListener());
        }
        runner.eventManager().addListener(listener.get());
        runner.addTest(masterSuite);

        if(compilerTestingOutput_)
        {
            runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));
        }

        // Built tests (either all, or only specific ones given on the
        // command line)
        if(!testNames_.empty())
        {

            for(TestNameContainer::const_iterator ii = testNames_.begin();
                ii != testNames_.end();
                ++ii)
            {
                status_ = runner.run(*ii, false, true, false) ? status_ : 1;
            }
        }
        else
        {
            status_ = runner.run("DefaultTests", false, true, false) ? 0 : 1;
        }

    }
    // normal simulation
    else
    {
		// install signal handler for graceful shutdown
		wns::simulator::SignalHandler& handler = wns::simulator::GlobalSignalHandler::Instance();
		handler.addSignalHandler(
			SIGXCPU,
			wns::simulator::CPUTimeExhaustedHandler(wns::simulator::getEventScheduler(), SIGXCPU));

        double period =
            this->getWNSView().get<double>("probesWriteInterval");

        if(period != 0.0)
        {
            MESSAGE_SINGLE(NORMAL, logger_, "Starting periodic writing of probes every " << period << "s");
            this->probeWriter.startPeriodicTimeout(period, period);
        }

        // create and configure simulation model
        if (!getWNSView().isNone("simulationModel"))
        {
            wns::pyconfig::View simModelConfig =
                getWNSView().get("simulationModel");

            std::string modelName = simModelConfig.get<std::string>("nameInFactory");
            ISimulationModelCreator* creator =
                ISimulationModelFactory::creator(modelName);

            MESSAGE_SINGLE(NORMAL, logger_, "Creating simulation model: " << modelName);
            simulationModel_.reset(creator->create(simModelConfig));
            MESSAGE_SINGLE(NORMAL, logger_, "Startup simulation model: " << modelName);
            simulationModel_->startup();
        }

        // The ProbeBus must be configured AFTER all modules have been loaded (the
        // modules may add implementations to static factories which can be accessed
        // by the ProbeBusRegistry)
        wns::simulator::getProbeBusRegistry()->startup();

        // queue event for end of simulation
        Time maxSimTime = getWNSView().get<wns::simulator::Time>("maxSimTime");
        if (maxSimTime > 0.0)
        {
	    wns::simulator::getEventScheduler()->stopAt(maxSimTime);
        }
    
        this->statusReport.writeStatus(false, "WNSStatusBeforeEventSchedulerStart.dat");

	    MESSAGE_SINGLE(NORMAL, logger_, "Start Scheduler");
#ifdef CALLGRIND
// If we run in flavour callgrind we are only interested in collecting
// profiling traces of the main event loop. So we start instrumentalization
// here. Startup and shutdown are of no interest to us.
CALLGRIND_START_INSTRUMENTATION;
#endif

        struct tms start, stop;
        times(&start);

    	wns::simulator::getEventScheduler()->start();

        times(&stop);
        cpuCyclesProbe_->put(stop.tms_utime - start.tms_utime);


#ifdef CALLGRIND
// Stop tracing profiling information
CALLGRIND_STOP_INSTRUMENTATION;
#endif

        // shutdown the simulation if it has been created before
        if (simulationModel_.get() != NULL)
        {
            simulationModel_->shutdown();
            simulationModel_.reset();
        }

		// uninstall signal handler for graceful shutdown, can only be
		// used in event loop
		handler.removeSignalHandler(SIGXCPU);

        MESSAGE_SINGLE(NORMAL, logger_, "Sending shutdown signal");
        (*wns::simulator::getShutdownSignal())();

		MESSAGE_SINGLE(NORMAL, logger_, "Simulation finished");
    }
}

void
Application::doShutdown()
{

#ifdef WNS_SMARTPTR_DEBUGGING
    MESSAGE_SINGLE(NORMAL, logger_, "SmartPtr debugging:");
    wns::SmartPtrBase::printAllExistingPointers();
#endif

    if (!testing_)
    {
        stopProbes();
    }

    // delete ProbeBusRegisty (auto_ptr)
    probeBusRegistry.reset();

    wns::simulator::getEventScheduler()->reset();

#ifdef WNS_SMARTPTR_DEBUGGING
    wns::SmartPtrBase::printAllExistingPointers();
#endif
	MESSAGE_SINGLE(NORMAL, logger_, "Calling shutDown for all modules");
	for(std::list<module::Base*>::iterator itr = loadedModules_.begin();
	    itr != loadedModules_.end();
	    ++itr)
	{
		(*itr)->shutDown();
	}

	MESSAGE_SINGLE(NORMAL, logger_, "Destroying all modules");
	while(loadedModules_.empty() == false)
	{
		wns::module::Base* mm = *(loadedModules_.begin());
		MESSAGE_SINGLE(NORMAL, logger_, "Destroying module " << wns::TypeInfo::create(*mm));
		delete mm;
		loadedModules_.erase(loadedModules_.begin());
	}

    // print final stats and shut down the event scheduler monitor
    if (eventSchedulerMonitor_.get() != NULL)
    {
        eventSchedulerMonitor_->logStatistics();
        eventSchedulerMonitor_.reset();
    }

    // deregister all signal handler before shutting down master logger
    wns::simulator::GlobalSignalHandler::Instance().removeAllSignalHandlers();

    // restore default state
    std::set_unexpected(std::terminate);

    // Reset the logger, no MasterLogger is set, no output will be available
    logger_ = logger::Logger("WNS", "Application", NULL);

    // This is the very last thing to shut down! Keep the MasterLogger up as
    // long as possible
    wns::simulator::getSingleton().shutdownInstance();

    // Run post processing hook in Python
    if (getWNSView().get<bool>("postProcessing()") == false)
    {
	    throw wns::Exception("postProcessing() failed!");
    }

}


std::string
Application::getPathToPyConfig()
{
    // if this thing here fails you the user can set PYCONFIGPATH ...
    char path[PATH_MAX];
    // /proc/self/exe is a link to the executable (openwns)
    ssize_t length = readlink( "/proc/self/exe", path, sizeof(path)-1 );
    if (length <= 0)
    {
        std::cerr << "Warning: could not determine path for PyConfig (readlink('/proc/self/exe') failed.";
        return "./PyConfig";
    }

    path[length] = '\0';

    std::string fullPath(path);
    // find the last of occurence of '/' and replace with '\0' (terminates the
    // string there). this strips the executable name
    size_t pos = fullPath.find_last_of('/');
    if(pos == std::string::npos)
    {
        std::cerr << "Warning: could not determine path for PyConfig from " << fullPath;
        return "./PyConfig";
    }

    std::string pathToOpenWNSExe = fullPath.substr(0, pos);
    // next to bin we can find the lib dir and below that the PyConfigs
    std::stringstream ss;
    ss << pathToOpenWNSExe << "/../lib/PyConfig/";
    return ss.str();
}

wns::pyconfig::View
Application::getWNSView() const
{
	return configuration_.get("openwns.simulator.config");
}

int
Application::doStatus() const
{
    return status_;
}

void
Application::unexpectedHandler()
{
    std::cerr << "openWNS: caught an unexpected exception!\n";
    wns::simulator::getMasterLogger()->outputBacktrace();
    exit(1);
}

void
Application::disableX87ExtendedFloatingPointPrecision()
{
    unsigned int mode = 0x27F;
	asm("fldcw %0" : : "m" (*&mode));
}


void
Application::loadModules()
{
	//std::list<wns::module::VersionInformation> moduleVersions;

	if(verbose_)
	{
		//std::cout << wns::module::CurrentVersion.getNiceString() << std::endl;
		std::cout << "Loading..." << std::endl;
	}

	//moduleVersions.push_back(wns::module::CurrentVersion);

	if (!moduleViews_.empty())
	{
		std::list<pyconfig::View> moduleViewsTmp = moduleViews_;

		std::list<pyconfig::View>::iterator itr = moduleViewsTmp.begin();
		size_t numberOfErrors = 0;
		std::string errorStr;

		// walk through the list, every time a module can be successfully loaded it
		// is removed from the list and we start from the beginning of the list.
		// This will be done until the list is empty non of the remaining modules
		// can be loaded.It can happen that module can't be loaded the first
		// time. This is because the module might have dependencies on other
		// modules which are not loaded yet.
		while (!moduleViewsTmp.empty())
		{
			if (numberOfErrors == moduleViewsTmp.size())
			{
				std::cout << "ModuleFactory contains the following modules:" << std::endl;
				for(wns::module::Factory::CreateMap::iterator i = wns::module::Factory::getMap()->begin();
				    i != wns::module::Factory::getMap()->end();
				    ++i)
				{
					std::cout << "\t"<< i->first << std::endl;
				}
				throw wns::Exception(
					"Can't load all specified modules. Reason:\n" +
					errorStr);
			}

			std::string libName = itr->get<std::string>("libname");
			std::string moduleName = itr->get<std::string>("__plugin__");

			if(verbose_)
			{
				std::cout << std::setw(8) << "Library: " << libName << "\n"
					  << "Module: " << moduleName << "\n";
			}
			// If the ModuleFactory knows the Module the library has been opened
			// before (may be due to static linkage). Then we don't need to load the
			// library by hand.
			if (wns::module::Factory::knows(moduleName) == false)
			{
				bool success = wns::module::Base::load(libName, absolutePath_, verbose_, lazyBinding_);
				if(success == false)
				{
					// continue with next candidate
					errorStr += std::string(dlerror());
					numberOfErrors += 1;
					++itr;
					continue;
				}
			}
			// it seems we found a loadable module -> append to module list
			wns::module::Creator* moduleCreator = wns::module::Factory::creator(moduleName);
			wns::module::Base* m = moduleCreator->create(*itr);
			loadedModules_.push_back(m);
			//wns::module::VersionInformation v = m->getVersionInformation();
			//moduleVersions.push_back(v);
			if(verbose_)
			{
				//std::cout << v.getNiceString() << std::endl;
				std::cout << std::endl;
			}
			moduleViewsTmp.erase(itr);
			// After a loadable module has been found and an error happened
			// before (another module could not be loaded), the first module
			// in the list is the one that could not be loaded. Thus we put
			// it to the end.
			if(numberOfErrors > 0)
			{
				moduleViewsTmp.push_back((*moduleViewsTmp.begin()));
				moduleViewsTmp.erase(moduleViewsTmp.begin());
			}
			itr = moduleViewsTmp.begin();
			numberOfErrors = 0;
		}

		if(listLoadedModules_)
		{
			std::cout << "The following Modules are available after dynamic loading:" << std::endl;
			for(wns::module::Factory::CreateMap::iterator i = wns::module::Factory::getMap()->begin();
			    i != wns::module::Factory::getMap()->end();
			    ++i)
			{
				std::cout << "\t"<< i->first << std::endl;
			}
		}

	}
}

void
Application::stopProbes()
{
    probeWriter.cancelPeriodicRealTimeout();

    MESSAGE_BEGIN(NORMAL, logger_, m, "Writing Probes");
    MESSAGE_END();

        try {
                statusReport.writeStatus(true);
        }
        catch(...) {
                std::cerr << "couldn't write status file.\n";
        }
    statusReport.stop();

    // Trigger output for ProbeBusses
    wns::simulator::getProbeBusRegistry()->forwardOutput();
}



void Application::writeFingerprint()
{
    std::string fingerprintName = getWNSView().get<std::string>("outputDir") + "/WNSFingerprint.dat";
    std::ofstream fingerprint(fingerprintName.c_str());
    if(!fingerprint.good())
        throw wns::Exception("Couldn't create fingerprint file.");

    fingerprint << "Version information not supported any more" << std::endl;
    fingerprint.close();
}

