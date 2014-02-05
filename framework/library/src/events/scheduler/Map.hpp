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

#ifndef WNS_EVENTS_SCHEDULER_MAP_HPP
#define WNS_EVENTS_SCHEDULER_MAP_HPP

#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/Subject.hpp>
#include <WNS/container/FastList.hpp>
#include <WNS/container/FastListEnabler.hpp>
#include <WNS/events/scheduler/CommandQueue.hpp>

#include <map>
#include <list>

namespace wns { namespace events { namespace scheduler {
    /**
     * @brief Implementation based on std::map
     */
    class Map :
        public Interface,
        public Subject<INotification>
    {
    public:

        // Default constructor
        Map();

        // Destructor
        virtual
        ~Map();

    protected:
        class Event :
            public virtual IEvent,
            public wns::container::SingleFastListEnabler< wns::SmartPtr<Event> >
        {
        public:
            Event(const Callable& callable) :
                callable_(callable),
                scheduled_(0),
                issued_(0),
                scheduler_(NULL),
                state_(NotSubmitted)
            {
            }

            enum State
            {
                NotSubmitted,
                Queued,
                Running,
                Finished,
                Canceled
            };

            virtual void
            cancel()
            {
                scheduler_->doCancelMapEventCalledFromMapEvent(EventPtr(this));
            }

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

            void
            operator()()
            {
                callable_();
            }

            wns::simulator::Time
            getScheduled() const
            {
                return scheduled_;
            }

            wns::simulator::Time
            getIssued() const
            {
                return issued_;
            }

            Callable callable_;

            wns::simulator::Time scheduled_;

            wns::simulator::Time issued_;

            Map* scheduler_;

            State state_;
        };

        typedef wns::SmartPtr<Event> EventPtr;

        /**
         * @name NVI implementation
         */
        //{@
        virtual void
        doReset();

        virtual wns::simulator::Time
        doGetTime() const;

        virtual void
        doStop();

        virtual void
        doStart();

        virtual void
        doCancelEvent(const IEventPtr& event);

        virtual IEventPtr
        doScheduleNow(const Callable& callable);

        virtual IEventPtr
        doSchedule(const Callable& callable, wns::simulator::Time at);

        virtual size_t
        doSize() const;

        virtual bool
        doProcessOneEvent();

        virtual ICommandPtr
        doQueueCommand(const Callable& callable);

        virtual void
        doDequeueCommand(const ICommandPtr& command);
        //@}

        /**
         * @name Scheduler observation
         */
        //{@
        virtual void
        sendProcessOneEventNotification();

        virtual void
        sendCancelEventNotification();

        virtual void
        sendScheduleNotification();

        virtual void
        sendScheduleDelayNotification();

        virtual void
        sendScheduleNowNotification();

        virtual void
        sendAddEventNotification();
        //@}

        /**
         * @name Internal helpers
         */
        //{@
        void
        doCancelMapEventCalledFromMapEvent(const EventPtr& event);

        void
        doCancelMapEvent(const EventPtr& event);
        //@}

        /**
         * @brief This is called if the sim time will change
         */
        virtual void
        onNewSimTime(const wns::simulator::Time& /*next Time*/)
        {
        }

        // MEMBER
        wns::simulator::Time simTime_;

        typedef wns::container::FastList<EventPtr> DiscreteTimeContainer;
        // we must use a pointer to the discrete time container, otherwise it
        // will be copied around and since it is SingleFastListEnable this will
        // result in a run-time error. It would be nice, if this could be turned
        // into a compile-time error.
        typedef std::map<wns::simulator::Time, DiscreteTimeContainer*> EventContainer;

        EventContainer events_;

        // Iterators of map don't get invalidiated (therefor we can store the
        // iterator here)
        EventContainer::iterator nowItr_;

        bool stop_;

        CommandQueue commandQueue_;
    };

} // scheduler
} // events
} // wns

#endif  // NOT defined WNS_EVENTS_SCHEDULER_MAP_HPP
