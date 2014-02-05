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

#ifndef WNS_EVENTS_SCHEDULER_INTERFACE_HPP
#define WNS_EVENTS_SCHEDULER_INTERFACE_HPP

#include <WNS/events/scheduler/ICommand.hpp>
#include <WNS/events/scheduler/IEvent.hpp>
#include <WNS/events/scheduler/Callable.hpp>
#include <WNS/events/scheduler/INotification.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/NonCopyable.hpp>
#include <WNS/SubjectInterface.hpp>

namespace wns { namespace events { namespace scheduler {

    /**
     * @brief Schedules and excutes Events in simulation. See
     * @ref schedulerBestPractices for examples.
     * @ingroup group_main_classes
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @author Karsten Klagges <kks@comnets.rwth-aachen.de>
     * @author Daniel Bültmann <dbn@comnets.rwth-aachen.de>
     *
     * The event scheduler is the "clock" of a simulation. If somethings needs to be
     * done in the "future" you have to create an Event and tell the event scheduler
     * when to execute the Event (scheduler->scheduleAt(event, 22.3)).
     *
     * The methods of the event scheduler have been logically grouped. The most
     * important groups are: Schedule Events and Delete Events.
     *
     * This is an NVI (non-virtual interface) for the implementation of
     * different event scheduler.
     */
    class Interface :
        virtual public SubjectInterface<IObserver>,
        private NonCopyable
    {
    public:
        /**
         * @name Current time
         */
        //@{
        /**
         * @brief Return current global simulation time
         *
         * @todo Marc Schinnenburg <marc@schinnenburg.com>: will probably move
         * into simulator ...
         */
        wns::simulator::Time
        getTime() const;
        //@}

        /**
         * @name Schedule Events
         * @brief Schedule Events for later execution
         */
        //@{

        /**
         * @brief Schedule a callable object for immediate execution.
         *
         * @note T must have implemented the call operator (
         * operator()() ).
         */
        IEventPtr
        scheduleNow(const Callable& callable)
        {
            this->sendScheduleNowNotification();
            return this->doScheduleNow(callable);
        }

        /**
         * @brief Schedule a callable object with a certain delay.
         *

         * @note T must have implemented the call operator (
         * operator()() ).
         */
        IEventPtr
        scheduleDelay(const Callable& callable, wns::simulator::Time delay)
        {
            this->sendScheduleDelayNotification();
            return this->doSchedule(callable, this->getTime() + delay);
        }

        /**
         * @brief Schedule a callable object for a given time.
         *
         * @note T must have implemented the call operator (
         * operator()() ).
         */
        IEventPtr
        schedule(const Callable& callable, wns::simulator::Time at)
        {
            this->sendScheduleNotification();
            return this->doSchedule(callable, at);
        }

         /**
         * @brief Queue a command to be executed as very next event
         */
        ICommandPtr
        queueCommand(const Callable& callable)
        {
            return this->doQueueCommand(callable);
        }

        /**
         * @brief Remove a command from the Scheduler
         */
        void
        dequeueCommand(const ICommandPtr& command);
        //@}

        /**
         * @name Delete Events
         * @brief Remove already scheduled Events (e.g. stop a Timer)
         */
        //@{

        /**
         * @brief Cancel the given event.
         */
        void
        cancelEvent(const IEventPtr& event);
        //@}

        /**
         * @name For testing purpose
         * @brief Used in unit tests
         */
        //@{
        /**
         * @brief Reset the scheduler to its initial state (for testing)
         */
        void
        reset();

        /**
         * @brief Process exactly one event
         */
        bool
        processOneEvent();
        //@}

        /**
         * @name Administrative
         * @brief Used by simulation environment
         */
        //@{
        /**
         * @brief Start scheduler (runs until no events left)
         */
        void
        start();

        /**
         * @brief Stop scheduler
         */
        void
        stop();

        /**
         * @brief Stop scheduler at this simulation time
         */
        void
        stopAt(const wns::simulator::Time& time);

        /**
         * @brief Returns number of events currently in the queue
         */
        size_t
        size() const;
        //@}

        /**
         * @brief Destructor
         */
        virtual
        ~Interface();

    protected:

        /**
         * @brief Constructor
         *
         * Protected since it is an interface and can only be called from
         * derived classes.
         */
        Interface();

    private:
        // NVI (non-virtual interface) - For explanation of the methods see
        // above
        virtual wns::simulator::Time
        doGetTime() const = 0;

        virtual IEventPtr
        doSchedule(const Callable& callable, wns::simulator::Time at) = 0;

        virtual IEventPtr
        doScheduleNow(const Callable& callable) = 0;

        virtual ICommandPtr
        doQueueCommand(const Callable& callable) = 0;

        virtual void
        doDequeueCommand(const ICommandPtr& command) = 0;

        virtual void
        doCancelEvent(const IEventPtr& event) = 0;

        virtual void
        doReset() = 0;

        virtual bool
        doProcessOneEvent() = 0;

        virtual void
        doStart() = 0;

        virtual void
        doStop() = 0;

        // this implementation works for all schedulers
        // a sub-class might override this method for custom-/optimization
        virtual void
        doStopAt(const wns::simulator::Time& time);

        virtual size_t
        doSize() const = 0;

        // end NVI


        /**
         * @name Notification
         */
        //{@
        virtual void
        sendProcessOneEventNotification() = 0;

        virtual void
        sendCancelEventNotification() = 0;

        virtual void
        sendScheduleNotification() = 0;

        virtual void
        sendScheduleDelayNotification() = 0;

        virtual void
        sendScheduleNowNotification() = 0;
        //@}
    };

    typedef wns::Creator<wns::events::scheduler::Interface> Creator;
    typedef StaticFactory<wns::events::scheduler::Creator> Factory;

} // scheduler
} // events
} // wns

/**
 * @page schedulerBestPractices The Event Scheduler
 *
 * With the openWNS event scheduler you can schedule anything that is callable
 * in your simulation. A callable can be a function pointer, a function object
 * or anything else that implements the call operator, requires no arguments and
 * has a return value of void. This page shows you how to use boost::bind to
 * create such callables in a uniform way and use the resulting function
 * objects with the scheduler.
 *
 * @section schedulingFreeFunctions Scheduling free functions
 * We will start with a very simple example. Suppose there exists a free
 * function freeFunction within your simulation and you want to schedule calls
 * to this function at later points in time.  The example function we use here
 * looks like this:
 * @include wns.events.scheduler.bestpractices.freeFunction.example
 * Every time it is called it sets a global variable to the value 101. To
 * schedule a call to this function at some later point in (simulation) time
 * you could simply do:
 * @include wns.events.scheduler.bestpractices.bindFreeFunction.example
 * This tells the scheduler to call the function freeFunction after 10 seconds
 * of simulation time have passed. The argument you pass to the scheduleDelay
 * function is a Callable that carries a pointer to freeFunction and the delay
 * after which the call to this function should be made. This is quite simple if you deal with
 * free functions, i.e. functions that do not belong to a class and therefore
 * do not have a context. If you want to schedule calls to member functions
 * (which you probably want to do frequently within an object oriented simulator)
 * you need to be able to tell the scheduler on which object the scheduler should
 * make the call to a member function, you want to bind the function pointer to
 * an instance of an object. This is when you want to use boost::bind. Let us
 * just stick with the last example for a short moment and see how you could use
 * boost::bind to create a Callable for freeFunction.
 * @include wns.events.scheduler.bestpractices.boostBindFreeFunction.example
 * Pretty simple to do that. Simply call boost::bind() and pass the function
 * pointer as the argument. This creates an object that is compatible with
 * wns::events::scheduler::Callable. However, this does not have any immediate
 * benefit for us. We will see later that by using boost::bind we get a
 * consistent syntax for all Callables which greatly improves readability.
 *
 * @section schedulingMemberFunctions Scheduling Member Functions
 * So let us have a look at member functions now. Suppose you have the following
 * class:
 * @include wns.events.scheduler.bestpractices.classWithCallabck.example
 * The behaviour of ClassWithCallback::callback is pretty much the same as the
 * freeFunction of our last example, but now the scope is not global but limited
 * to the scope of the class instance.
 * @section schedulingMemberFunctionsByPointer Scheduling Member Functions by Pointer
 * When we pass an instance of some type to a boost::bind expression it is copied.
 * This is fine and yields the expected results if we work with pointers, but it
 * does not necessarily yield the expected results if you pass your arguments
 * by value (see next section). To schedule a member function of an object where
 * you have a pointer to can be done like this:
 * @include wns.events.scheduler.bestpractices.boostBindMemberFunction.example
 * @section schedulingMemberFunctionsByValue Scheduling Member Functions by Value
 * The code below illustrates the usage of boost::bind if you have references to
 * your callback objects. To avoid copying of your callback object when you pass
 * it as an argument to boost::bind, just wrap in an boost::ref or boost::cref
 * (wrapper for const references) object. Thereby, only the reference wrapper is
 * copied but not the reference itself. You could also have simply taken the
 * address (& operator) of your callback object and pass it to boost::bind to
 * enforce pointer semantics.
 * @include wns.events.scheduler.bestpractices.boostBindMemberFunctionRef.example
 * @section schedulingMemberFunctionsArguments Scheduling functions that take arguments
 * The examples above lack a very important feature. None of the above callbacks
 * can take an argument. Most often within your simulation you want to schedule
 * calls that take parameters. Boost::bind offers partial binding of parameters,
 * too. If you bind all parameters in an boost::bind expression, the result is
 * a nullary function object that you can the pass the the scheduler. The follwing
 * example illustrates this:
 * @include wns.events.scheduler.bestpractices.boostBindMemberFunctionParam.example
 * This concludes the scheduler best practices lesson.
 *
 * @section wns_events_scheduler_interface_cancel_events Cancelling Events
 *
 * Whenever you schedule an event by using either the @c schedule or @c scheduleDelay
 * method of the eventschdeduler these methods return an instance of @c IEventPtr . This
 * is a handle for your scheduled event and can be used to remove it from the scheduler
 * queue again. This is demonstrated in the next example.
 * 
 * @include wns.events.scheduler.bestpractices.unittest.cancel.example
 */

#endif // NOT defined WNS_EVENTS_SCHEDULER_INTERFACE_HPP
