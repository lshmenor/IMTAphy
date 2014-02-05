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

#ifndef WNS_EVENTS_SCHEDULER_COMMAND_HPP
#define WNS_EVENTS_SCHEDULER_COMMAND_HPP

#include <WNS/SmartPtr.hpp>
#include <WNS/Exception.hpp>

namespace wns { namespace events { namespace scheduler {
    /**
     * @brief Base of commands that may be queued using the
     * EventScheduler::queueCommand interface.
     *
     * A command runs through the following life cycle:
     * @dot
     digraph LifeCycle {
     rankdir=LR
     node [shape=record, fontname=Helvetica, fontsize=10, color=black, height=0.2,width=0.4];
     NotSubmitted -> Queued
     Queued -> Running
     Queued -> Canceled
     Running -> Finished
     }
     * @enddot
     * A command can only be cancled if it is currently Queued. It can't be
     * re-submitted if it was cancled or has finished. The final state is either
     * finished or cancled.
     * The Command that is returned by the queue method will be in state Queued!
     * The user will never see NotSubmitted.
     */
    class ICommand :
        virtual public RefCountable
    {
    public:
        /**
         * @brief If a command can be canceled this exception should be thrown by
         * the command queue
         *
         * @author Marc Schinnenburg <marc@schinnenburg.com>
         *
         * An command may not be canceled if it is
         *
         *  - currently running
         *  - already canceled
         *  - already executed
         *  - not submitted to the command queue at all (which should not happen)
         */
        class CancelException :
            wns::Exception
        {
        public:
            CancelException(const std::string& s) :
                wns::Exception(s)
            {
            }

            ~CancelException() throw() {}
        };

        virtual
        ~ICommand()
        {
        }

        /**
         * @brief Return true, if the Command is not submitted to the queue so
         * far (initial state)
         */
        virtual bool
        isNotSubmitted() const = 0;

        /**
         * @brief Return true if the Command is queued (waiting for execution)
         */
        virtual bool
        isQueued() const = 0;

        /**
         * @brief Return true if the Command is currently being executed
         */
        virtual bool
        isRunning() const = 0;

        /**
         * @brief Return true if the Command was executed (no longer queued, no
         * longer running)
         */
        virtual bool
        isFinished() const = 0;

        /**
         * @brief Return true if the Command was canceled
         */
        virtual bool
        isCanceled() const = 0;
    };

    typedef wns::SmartPtr<ICommand> ICommandPtr;
}
}
}

#endif // NOT defined WNS_EVENTS_SCHEDULER_COMMAND_HPP
