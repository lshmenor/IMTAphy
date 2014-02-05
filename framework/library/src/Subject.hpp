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

#ifndef WNS_SUBJECT_HPP
#define WNS_SUBJECT_HPP

#include <WNS/Assure.hpp>
#include <WNS/ObserverInterface.hpp>
#include <WNS/SubjectInterface.hpp>
#include <WNS/IOutputStreamable.hpp>

#include <functional>
#include <list>
#include <algorithm>

namespace wns {

    /**
     * @brief A Subject to an observer.
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @author Swen Kuehnlein
     *
     * @param NOTIFICATIONINTERFACE The notification interface of the observers.
     *
     * This implementation of SubjectInterface manages observer registrations,
     * provides methods to access all observers and informs all observers
     * on its destruction.
     *
     * Derivers can notify all observers with sendNotifies() and perform
     * operations on all observer with forEachObserver().
     *
     * @attention If your class derives from different instantiations of
     * this template, the compiler will not know which attach() and detach()
     * method to call without explicitly telling him. So you must either require
     * your observers to do this, or provide forwarding functions for each
     * attach() and detach() incarnation.
     *
     * @see Observer, SubjectInterface, ObserverInterface.
     *
     * @ingroup MISC
     */
    template <typename NOTIFICATIONINTERFACE>
    class Subject :
        public virtual SubjectInterface<ObserverInterface<NOTIFICATIONINTERFACE> >
    {
    public:
        /**
         * @brief The NotificationInterface used by Subject.
         *
         * The NotificationInterface used by Subject.
         */
        typedef NOTIFICATIONINTERFACE NotificationInterface;

        /**
         * @brief The ObserverInterface using the same NotificationInterface.
         *
         * The corresponding ObserverInterface that uses the same NotificationInterface.
         */
        typedef ObserverInterface<NotificationInterface> ObserverType;

        /**
         * @brief Our interface.
         *
         * The interface that this Subject implements.
         */
        typedef SubjectInterface<ObserverType> SubjectType;

        /**
         * @brief Default constructor
         */
        Subject() :
            observers(),
            modificationGuard_(false)
        {
        }

        /**
         * @brief copy constructor
         */
        Subject(const Subject& other) :
            SubjectInterface< ObserverInterface<NOTIFICATIONINTERFACE> >(other),
            observers(other.observers),
            modificationGuard_(other.modificationGuard_)
        {
            this->forEachObserver(
                std::bind2nd(
                    std::mem_fun(&ObserverType::addSubject),
                    this));
        }

        /**
         * @brief Remove own observers, connect to others obsevers
         */
        Subject&
        operator=(const Subject& other)
        {
            // remove own observers
            this->forEachObserver(
                std::bind2nd(
                    std::mem_fun(
                        &ObserverType::removeSubject),
                    this));

            // copy subjects from other
            observers = other.observers;

            // observe these subjects
            this->forEachObserver(
                std::bind2nd(
                    std::mem_fun(&ObserverType::addSubject),
                    this));

            return *this;
        }

        /**
         * @brief Removes itself from every attached Observer.
         *
         * Informs every attached observer that it is no longer
         * attached to this Subject instance via ObserverInterface::removeSubject().
         */
        virtual
        ~Subject()
        {
            this->forEachObserver(
                std::bind2nd(
                    std::mem_fun(
                        &ObserverType::removeSubject),
                    this));
        }

        /**
         * @brief Notify all observers via @a notificattionFunctionPtr.
         *
         * @param notificationFunctionPtr A pointer to a member function of
         * NotificationInterface that receives no arguments.
         *
         * Calls the no-argument member function referenced by @a notificationFunctionPtr
         * on each observer.
         */
        template <typename NOTIFICATIONFUNCTIONPTR>
        void
        sendNotifies(const NOTIFICATIONFUNCTIONPTR notificationFunctionPtr)
        {
            this->forEachObserver(std::mem_fun(notificationFunctionPtr));
        }

        /**
         * @brief Notify all observers via @a notificattionFunctionPtr and one argument.
         *
         * @param notificationFunctionPtr A pointer to a member function of
         * NotificationInterface that receives one argument.
         *
         * @param arg The argument to pass to each observer via the call
         * of the member function referenced by @a notificationFunctionPtr.
         *
         * Calls the one-argument member function referenced by @a notificationFunctionPtr
         * on each observer with @a arg.
         */
        template <typename NOTIFICATIONFUNCTIONPTR, typename ARG>
        void
        sendNotifies(const NOTIFICATIONFUNCTIONPTR notificationFunctionPtr, const ARG& arg)
        {
            this->forEachObserver(std::bind2nd(std::mem_fun(notificationFunctionPtr), arg));
        }

        /**
         * @brief Execute @a functor on each observer.
         *
         * @param functor The function to perform an each observer.
         *
         * @returns The functor object passed as @a functor in the
         * state after it has been applied to all observers.
         */
        template <typename FUNCTOR>
        FUNCTOR
        forEachObserver(const FUNCTOR& functor)
        {
            ObserverContainer tmp = this->observers;
            return std::for_each(tmp.begin(),
                                 tmp.end(),
                                 functor);
        }

        /**
         * @brief Execute @a functor on each of the originals observers.
         *
         * @param functor The function to perform an each observer.
         *
         * @returns The functor object passed as @a functor in the
         * state after it has been applied to all observers.
         *
         * @note This version operates on the original list of
         * observers. It therefore ensures via guards that the list is
         * not changed during the process.
         */
        template <typename FUNCTOR>
        FUNCTOR
        forEachObserverNoDetachAllowed(const FUNCTOR& functor)
        {
            this->modificationGuard_ = true;
            FUNCTOR f = std::for_each(this->observers.begin(),
                                      this->observers.end(),
                                      functor);
            this->modificationGuard_ = false;
            return f;
        }

        bool
        hasObservers() const
        {
            return !observers.empty();
        }

    protected:
        /** @brief */
        typedef std::list<ObserverType*> ObserverContainer;

    private:
        /**
         * @copydoc SubjectInterface::addObserver()
         *
         * All attached observers will be addressed by sendNotifies() and forEachObserver(),
         * and they will be informed of the destruction of this Subject instance via
         * ObserverInterface::removeSubject().
         */
        virtual void
        addObserver(ObserverType* observer)
        {
            if (this->modificationGuard_ == true)
            {
                throw wns::Exception("Tried to modify observer list even though in protected operation");
            }
            assureNotNull(observer);
            assure(
                find(this->observers.begin(), this->observers.end(), observer) == this->observers.end(),
                "Already registered");
            this->observers.push_back(observer);
        }

        /**
         * @copydoc SubjectInterface::removeObserver()
         */
        virtual void
        removeObserver(ObserverType* observer)
        {
            if (this->modificationGuard_ == true)
            {
                throw wns::Exception("Tried to modify observer list even though in protected operation");
            }
            assureNotNull(observer);
            assure(
                find(this->observers.begin(), this->observers.end(), observer) != this->observers.end(),
                "Not registered");

            this->observers.erase(find(this->observers.begin(), this->observers.end(), observer));
        }


        /**
         * @brief The observer collection.
         *
         * The observer collection.
         */
        ObserverContainer observers;
        bool modificationGuard_;
    };
}

#endif // NOT defined WNS_SUBJECT_HPP
