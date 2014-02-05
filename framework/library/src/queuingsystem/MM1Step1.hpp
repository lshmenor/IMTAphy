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

// begin example "wns.queuingsystem.mm1step1.hpp.example"
#ifndef WNS_QUEUINGSYSTEM_MM1STEP1_HPP
#define WNS_QUEUINGSYSTEM_MM1STEP1_HPP

#include <WNS/rng/RNGen.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/simulator/ISimulationModel.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/IOutputStreamable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/distribution/Distribution.hpp>

#include <boost/bind.hpp>

// In the OpenWNS we use nested namespaces
// which reflect the directory tree
// This file is in ./src/queuingsystem/
// so the convention is to use the same namespace:

namespace wns { namespace queuingsystem {

     // Simulation model for queueing system tutorial (step 1).
     //
     // Here We implement our own simulation model. To allow the openwns
     // application to load our custom simulation model, we must inherit
     // from wns::simulator::ISimulationModel.
     //
	/**
	 * @brief This is a text line parsed by Doxygen
	 * to automatically generate a documentation for class SimpleMM1Step1
	 * The documentation can be read using a web browser.
	 * Ask your openwns guru what URL to use.
	 */
    class SimpleMM1Step1 :
        public IOutputStreamable,
        public wns::simulator::ISimulationModel
    {
    public:
        // All simulation models must be constructible from a
        // wns::pyconfig::View.
		/**
		 * @brief You can (and should!) document each function like this.
		 */
        explicit
        SimpleMM1Step1(const wns::pyconfig::View& configuration);
		// ^ The Python class used in MM1Step1.py
		// is intentionally called the same.
		// The Python view parameter here gets the values set as member
		// in the constructor of the Python class.

    private:
        // Implementation of the ISimulationModel interface.
        // The openwns application will call this method to indicate
        // that construction of the environment (scheduler, random number
        // generator, etc.) has finished.
        //
        // We will place our own startup code here
        virtual void
        doStartup();

        // Implementation of the ISimulationModel interface.
        // The openwns application will call this method to indicate
        // that the simulation ends and the model should shut down.
        //
        // We will place our own shutdown code here
        virtual void
        doShutdown();

        // Implementation of the IOutputStreamable interface.
        // IOutputStreamable implements the stream output operator for
        // us. It will get the contents to output to the stream by
        // calling this method.
        virtual std::string
        doToString() const;

        // Generates a new job that enters the queuing system
        void
        generateNewJob();

        // Excuted after a job has been processed
        void
        onJobProcessed();

        // Starts processing of the next job if one is available
        void
        processNextJob();

        // A random number generator object pointer. 
		/**
		 * @brief You can also document member variables like this
		 */
        wns::distribution::Distribution* jobInterarrivalTime_;

        // A random number generator object that creates exponentially
        // distributed random numbers.
        wns::distribution::Distribution* jobProcessingTime_;

        // Counter for the number of jobs in our system
        int jobsInSystem_;

        // Used to access the logging subsystem
        wns::logger::Logger logger_;
    };
}
}

#endif // NOT defined WNS_QUEUINGSYSTEM_MM1STEP1_HPP
// end example

// The following block is meta data used for tha automatic generation of
// documentation by the "Doxygen" system.

/**
 * @defgroup group_wns_queuingsystem_tutorial Beginners Tutorial
 * @ingroup group_tutorials
 * @brief Simulating a M/M/1 queuing system
 * @section intro Introduction
 *
 * Welcome to the openWNS tutorial. This tutorial consists of several
 * incremental parts that will guide you through all the major parts of
 * the openWNS simulator. We will start with a very simple implementation
 * of a M/M/1 queuing system and will step by step expand it. The following
 * list shows the contents of this tutorial.
 *
 *   -# @subpage wns.queuingsystem.mm1step1
 *   -# @subpage wns.queuingsystem.mm1step2
 *   -# @subpage wns.queuingsystem.mm1step3
 *   -# @subpage wns.queuingsystem.mm1step4
 *   -# @subpage wns.queuingsystem.mm1step5
 *
 * @section mm1queueingsystem The M/M/1 Queuing System
 * The M/M/1 system consists of a queue with unlimited size
 * and a worker (server) with* negative-exponentially distributed job processing time.
 * The jobs arrive at the system with an inter arrival time
 * that is negative-exponentially distributed.
 * This is illustrated below
 * @dot
 * digraph overview {
 * rankdir=LR
 * node [shape=record, fontname=Helvetica, fontsize=10, color=black, height=0.2,width=0.4];
 * edge [style="solid", arrowhead="open", fontsize=10, fontname=Helvetica ]
 * ArrivalProcess [shape=ellipse]
 * Worker [shape=ellipse]
 * ArrivalProcess -> Queue
 * Queue -> Worker
 * Worker -> Exit
 * }
 * @enddot
 *
 * @section directory The OpenWNS directory layout
 *     - You should have some UNIX knowledge at this point.
 *     - The directory layout you will experience is as follows:
 *     - Your current working directory should be the base directory
 *       where you installed your copy of openwns into your home directory.
 *     - From here you see (with "ls -lag") the following directories, among others:
 *     <PRE>
 *     ./bin/           This must be in your PATH environment variable
 *     ./documentation/ Here you find text, PDF and PPT files that may help you
 *     ./examples/      This is where you start the MM1 tutorial. Change dir to here and look
 *     ./framework/     Here the main source code (C++ and Python) is located
 *     ./modules/       Here is the source code for blocks of specific ISO-OSI layers
 *     ./sandbox/       This is where your compilation results (and header and Python files) will be copied into when you run "./playground.py install"
 *     ./tests/         Here are the files for UnitTests and SystemTests
 *     ./playground.py  This is the main script for handling the openwns
 *     </PRE>
 *
 * @section build Building of the Examples
 *   - Call "./playground.py install" in the OpenWNS base directory
 *   - This compiles all C++ files, and copies the result into the ./sandbox/ directory
 *   - If there are compiler errors, try to find the file where the compiler stopped
 *     and edit it with emacs. Then go to the line number shown and use your C++ skills.
 *   - After compilation, there is now an executable called "openwns" (formerly wns-core)
 *   - In many directories a symbolic link is made to point to this executable file.
 *
 * @section execution Execution of the Examples
 *   - To execute the individual tutorial steps follow this:
 *   - change directory to ./examples/
 *   - open and read the file README for newest notes
 *   - call "./openwns -f MM1Step1.py" to start the first tutorial step.
 *   - You will now see approximately 500 lines of output written to STDOUT
 *   - The output lines are formatted into four columns:
 *     <ol>
 *       <li> The current simulated time
 *       <li> The module name where the code that runs is implemented
 *       <li> The name of the "logger" object; usually a hierarchically given name
 *       <li> The output of MESSAGE_SINGLE() calls in the code
 *     </ol>
 *   - Which files are involved?
 *     - the called executable "./openwns" has access to every simulator implementation
 *     - If MM1Step1.py is given as parameter, it is read by the embedded Python interpreter of "./openwns"
 *     - Have a look into MM1Step1.py to familiarize yourself with Python
 *     - Try to modify these files. Unless you commit something you cannot do any harm.
 *     - For example, change the simulation time from 10 to 100 seconds
 *     - Try to find out what the load factor is and change it to 50 percent.
 *     - Open the files included in MM1Step1.py to understand more about the Python use in OpenWNS.
 *     - The C++ code that corresponds to the MM1Step1 Python file is located in
 *     <PRE>
 *       ./framework/library/src/queuingsystem/MM1Step1.hpp
 *       ./framework/library/src/queuingsystem/MM1Step1.cpp
 *     </PRE>
 *     - Have a look into MM1Step1.hpp and cpp to familiarize yourself with the C++ use in OpenWNS.
 *
 *
 * @page wns.queuingsystem.mm1step1 Implementing a SimulationModel
 * @ref group_wns_queuingsystem_tutorial
 * @section simulationmodel Implementing a SimulationModel
 *
 *   - The simulator core implements the main() function, you don't need to know this.
 *   - The application program will be ./openwns, the result of the compilation step.
 *   - Your simulation model will be implemented in a class which inherits from ISimulationModel.
 *   - By inheritance from ISimulationModel you get interface functions which you overload as <tt>virtual</tt>.
 *   <PRE>
 *     doStartup()
 *        The openwns application will call this method to indicate
 *        that construction of the environment has finished.
 *        You will put your initialization code into it
 *        and trigger one or more discrete events for the future.
 *        In this example we simply call oue own function <tt>generateNewJob()</tt>
 *     doShutdown()
 *        The openwns application will call this method to indicate
 *        that the simulation ends and the model should shut down.
 *        There is currently no need to fill code in here.
 *   </PRE>
 *   - By inheritance from IOutputStreamable you get this function:
 *   <PRE>
 *     doToString()
 *        IOutputStreamable implements the stream output operator for us
 *        It will get the contents to output to the stream by calling this method.
 *   </PRE>
 *   - When doStartup is called, the environment is promised to exist:
 *     - discrete event scheduler
 *     - random number generators
 *     - Logger
 *
 * @section simplemm1 A very simple M/M/1 queuing system
 *   - The implementation is done with the functions
 *   <PRE>
 *     generateNewJob():
 *                       Here we count the number of jobs generated,
 *                       determine the interarrival time until the next job will arrive
 *                       and schedule an event for exactly this time in the future
 *                       using the discrete event scheduler function scheduleDelay()
 *                       If this is a new job and there is no other job in the system
 *                       the function processNextJob() is called.
 *     processNextJob():
 *                       Called either from generateNewJob() if the queue was empty
 *                       or immediately after the predecessor job was finished
 *                       Here the random number for the processing time is drawn
 *                       and an event at this time in the future is scheduled
 *                       to call the function onJobProcessed()
 *     onJobProcessed():
 *                       Called by the discrete event scheduler.
 *                       Unless there are no more jobs in the queue
 *                       it starts processing another job.
 *                       The number of jobs in the system is decremented again.
 *   </PRE>
 *   - Logging means writing informational output lines to STDOUT during the simulation
 *     - you can either use MESSAGE_BEGIN and MESSAGE_END macros or
 *     - <tt>MESSAGE_SINGLE(NORMAL, logger_, "Your output string here");</tt>
 *     - see the documentation of the logging system to learn more.
 *   - Drawing random numbers is easy:
 *     - There are two objects of the <tt>Exponential</tt> class
 *     - This class has the operator() implemented
 *     - We simply call is with the object like in
 *     <PRE>
 *       wns::simulator::Time delayToNextJob = jobInterarrivalTime_();
 *     </PRE>
 *   - Scheduling: The discrete event scheduler is accessed with:
 *     <PRE>
 *        wns::simulator::getEventScheduler()->scheduleDelay(
 *            boost::bind(&SimpleMM1Step1::generateNewJob, this),
 *            delayToNextJob);
 *     </PRE>
 *     This looks weird, but it only binds our function <tt>generateNewJob()</tt>
 *     to an event scheduled after <tt>delayToNextJob</tt> seconds.
 *
 * @section configuration Configuration of your system
 *   - PyConfig:
 *     - The constructor get an wns::pyconfig::View object
 *     - This object knows the parameters exactly for our object to be configured
 *     - We get the values by <tt>config.get<ReturnType>("parameterName")</tt>
 *     - e.g. by <tt>config.get<wns::simulator::Time>("meanJobInterArrivalTime")</tt>
 *
 * @section loadingsimulationmodel How to tell openWNS which Simulation Model to load
 *   - Registration and use of StaticFactory:
 *     - There is a line <tt>STATIC_FACTORY_REGISTER_WITH_CREATOR</tt> in the cpp file
 *     - It registers this class under the name "openwns.queuingsystem.SimpleMM1Step1"
 *     - In MM1Step1.py the SimpleMM1Step1 Python class is used.
 *     - See the class definition in <tt>PyConfig/openwns/queuingsystem.py</tt>
 *     - You will find the member <tt>self.nameInFactory = 'openwns.queuingsystem.SimpleMM1Step1'</tt>
 *     - This is the link between Python and C++ classes.
 *     - The instantiation of objects of this class is done by the OpenWNS core.
 *   - In the toplevel PyConfig <tt>openwns.setSimulator(sim)</tt> sets the configuration for this simulation
 *   - How do I get my config (delegation through the application):
 *     - The OpenWNS core automatically cares for the object to get its correct Python parameters
 *
 * @section staticfactory A look behind the scenes (StaticFactory)
 *   - StaticFactory is a concept to dynamically generate (instanciate) many objects
 *     of the same class, which is known by a string, during runtime.
 *     - The call of new and delete is hidden from the programmer
 *     - Each object gets a different PyConfig View,
 *       so each object can have its own personality.
 *   - Connects the registration process with the configuration process
 *
 * @section output Expected Output
 * <PRE>
(  0.0000000) [  WNS] Application    Start up modules
(  0.0000000) [  WNS] Application    Creating simulation model: openwns.queuingsystem.SimpleMM1Step1
(  0.0000000) [  WNS] Application    Startup simulation model: openwns.queuingsystem.SimpleMM1Step1
(  0.0000000) [  WNS] SimpleMM1Step1    MM1Step1 started, generating first job
                                        Jobs in system: 0
(  0.0000000) [  WNS] SimpleMM1Step1    Generated new job, next job in 0.168591s
                                        Jobs in system: 1
(  0.0000000) [  WNS] SimpleMM1Step1    Processing next job, processing time: 0.0144122s
                                        Jobs in system: 1
(  0.0000000) [  WNS] Application       Start Scheduler
(  0.0144122) [  WNS] SimpleMM1Step1    Finished a job
                                        Jobs in system: 0
(  0.1685907) [  WNS] SimpleMM1Step1    Generated new job, next job in 0.236225s
                                        Jobs in system: 1
(  0.1685907) [  WNS] SimpleMM1Step1    Processing next job, processing time: 0.178384s
                                        Jobs in system: 1
...
( 10.0000000) [  WNS] Application       Simulation finished
( 10.0000000) [  WNS] Application       Calling shutDown for all modules
( 10.0000000) [  WNS] Application       Destroying all modules
 * </PRE>
 *
 * This is the contents of the file MM1Step1.hpp
 * @include "wns.queuingsystem.mm1step1.hpp.example"
 * This is the contents of the file MM1Step1.cpp
 * @include "wns.queuingsystem.mm1step1.cpp.example"
 * This is the contents of the file MM1Step1.py
 * @include "wns.queuingsystem.mm1step1.py.example"
 */
