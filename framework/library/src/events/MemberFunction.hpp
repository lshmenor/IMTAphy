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

#ifndef WNS_EVENTS_MEMBERFUNCTION_HPP
#define WNS_EVENTS_MEMBERFUNCTION_HPP

#include <WNS/Functor.hpp>
#include <WNS/PythonicOutput.hpp>
#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/simulator/ISimulator.hpp>

#include <boost/bind.hpp>

namespace wns { namespace events {

    /**
     * @brief Generate an event calling a member function in one line
     *
     * Usage is quite easy:
     * @include wns.events.MemberFunction.sendNow.example
     */
    template <typename T>
    class MemberFunction :
        public TFunctor<T>,
        public virtual PythonicOutput
    {
    public:
        MemberFunction(T* obj, typename TFunctor<T>::functionPointer fptr) :
            wns::TFunctor<T>(obj, fptr)
        {
        }

        /**
         * @brief Destructor
         */
        virtual
        ~MemberFunction()
        {
        }
    };

    /**
     * @brief Similar to wns::events::MemberFunction, but to be used in
     * std::for_each (or similar)
     *
     * Usage:
     * @include wns.events.DelayedMemberFunction.sendDelayed.example
     */
    template <typename T>
    class DelayedMemberFunction
    {
    public:
        /**
         * @brief Takes a member function pointer and the delay
         */
        DelayedMemberFunction(
            typename TFunctor<T>::functionPointer _fptr,
            wns::simulator::Time _delay) :
            fptr(_fptr),
            delay(_delay)
        {
        }

        /**
         * @brief Destructor
         */
        virtual
        ~DelayedMemberFunction()
        {
        }

        /**
         * @brief This is for pointers
         */
        virtual void
        operator()(T* obj)
        {
            wns::simulator::getInstance()->getEventScheduler()->scheduleDelay(boost::bind(fptr, obj), this->delay);
        }

        /**
         * @brief This is for value objects
         */
        virtual void
        operator()(T& obj)
        {
            wns::simulator::getInstance()->getEventScheduler()->scheduleDelay(boost::bind(fptr, &obj), this->delay);
        }
    private:
        typename TFunctor<T>::functionPointer fptr;
        wns::simulator::Time delay;
    };

} // events
} // wns

#endif // NOT defined WNS_EVENTS_MEMBERFUNCTION_HPP
