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

#ifndef WNS_EVENTS_SCHEDULER_COMMANDQUEUE_HPP
#define WNS_EVENTS_SCHEDULER_COMMANDQUEUE_HPP

#include <WNS/events/scheduler/ICommand.hpp>
#include <WNS/events/scheduler/Callable.hpp>
#include <WNS/container/FastListEnabler.hpp>
#include <WNS/container/FastList.hpp>
#include <WNS/Assure.hpp>

#include <pthread.h>

namespace wns { namespace events { namespace scheduler {
    /**
     * @brief Thread safe command queue
     *
     * Fabian Debus <fabian.debus@illator.de>
     * Marc Schinnenburg <marc@schinnenburg.com>
     *
     * This thread safe command queue is used to insert commands from other
     * threads into the main event loop. The command queue will be paged before
     * a new event is executed and will run all available commands. 
     */
    class CommandQueue
    {
    public:
        /**
         * @brief Default constructor
         */
        CommandQueue();

        /**
         * @brief Destructor
         */
        virtual
        ~CommandQueue();

        /**
         * @brief Remove all commands
         */
        void
        reset();

        /**
         * @brief Queue a command for thread safe execution from within the main loop.
         */
        ICommandPtr
        queueCommand(const Callable& callee);

        /**
         * @brief Remove a command from the queue
         */
        void
        dequeueCommand(const ICommandPtr& command);

        /**
         * @brief This "brings" us back into the main event loop ...
         */
        void
        runCommands();

    private:

        /**
         * @brief internal representation of ICommand
         *
         * All members are public since this is a private class of the CommandQueue
         * and thus cannot be accessed from outside.
         */
        class Command :
            public virtual ICommand,
            public wns::container::SingleFastListEnabler< wns::SmartPtr<Command> >
        {
        public:
            Command(const wns::events::scheduler::Callable& callable) :
                callable_(callable),
                state_(NotSubmitted)
            {
            }

            /**
             * @brief forward call to callable
             */
            void
            operator()()
            {
                callable_();
            }

            enum State
            {
                NotSubmitted,
                Queued,
                Running,
                Finished,
                Canceled
            };

            virtual bool
            isNotSubmitted() const
            {
                return NotSubmitted == state_;
            }

            virtual bool
            isQueued() const
            {
                return Queued == state_;
            }

            virtual bool
            isRunning() const
            {
                return Running == state_;
            }

            virtual bool
            isFinished() const
            {
                return Finished == state_;
            }

            virtual bool
            isCanceled() const
            {
                return Canceled == state_;
            }

            wns::events::scheduler::Callable callable_;

            State state_;
        };

        typedef wns::SmartPtr<Command> CommandPtr;

        typedef wns::container::FastList<CommandPtr> CommandContainer;

        CommandContainer queue_;

        pthread_mutex_t mutex_;

    }; // CommandQueue
} // scheduler
} // events
} // wns

#endif // NOT defined WNS_EVENTS_SCHEDULER_COMMANDQUEUE_HPP
