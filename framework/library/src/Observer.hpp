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

#ifndef WNS_OBSERVER_HPP
#define WNS_OBSERVER_HPP

#include <WNS/Assure.hpp>
#include <WNS/SubjectInterface.hpp>
#include <WNS/ObserverInterface.hpp>

#include <list>
#include <algorithm>
namespace wns {

    /**
     * @brief An observer to a subject.
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @author Swen Kuehnlein
     *
     * @param NOTIFICATIONINTERFACE The notification interface of the observer.
     *
     * This implementation of ObserverInterface detaches itself from all
     * subjects it is attached to on its destruction. It will receive notifications
     * from the subject via the @a NOTIFICATIONINTERFACE it (indirectly) derives from.
     *
     * @see Subject, ObserverInterface, SubjectInterface.
     */
    template <typename NOTIFICATIONINTERFACE>
    class Observer :
        virtual public ObserverInterface<NOTIFICATIONINTERFACE>
    {
    public:
        /**
         * @brief The NotificationInterface used by Observer.
         *
         * The NotificationInterface used by Observer.
         */
        typedef NOTIFICATIONINTERFACE NotificationInterface;

        /**
         * @brief Our interface.
         *
         * The interface that this Observer implements.
         */
        typedef ObserverInterface<NotificationInterface> ObserverType;

        /**
         * @brief The SubjectInterface using the same NotificationInterface.
         *
         * The corresponding SubjectInterface that uses the same NotificationInterface.
         */
        typedef SubjectInterface<ObserverType> SubjectType;

        /**
         * @brief Default constructor
         */
        Observer() :
            subjects()
        {
        }

        /**
         * @brief The copy obseves the same subjects as the original
         */
        Observer(const Observer& other) :
            NotificationInterface(other),
            ObserverInterface<NOTIFICATIONINTERFACE>(other),
            subjects(other.subjects)
        {
            this->forEachSubject(
                std::bind2nd(
                    std::mem_fun(&SubjectType::addObserver),
                    this));
        }

        /**
         * @brief Remove own subjects, obseve new subjects
         */
        Observer&
        operator=(const Observer& other)
        {
            // remove own observers
            this->forEachSubject(
                std::bind2nd(
                    std::mem_fun(&SubjectType::removeObserver),
                    this));

            // copy subjects from other
            subjects = other.subjects;

            // observe these subjects
            this->forEachSubject(
                std::bind2nd(
                    std::mem_fun(&SubjectType::addObserver),
                    this));

            return *this;
        }

        /**
         * @brief Detaches itself from every Subject it is attached to.
         *
         * Every Subject that has informed us, that we are attached to it,
         * will receive a detach.
         */
        virtual
        ~Observer()
        {
            this->forEachSubject(
                std::bind2nd(
                    std::mem_fun(&SubjectType::removeObserver),
                    this));
        }

        /**
         * @brief Nicer to read: oberserver->startObserving(subject);
         */
        void
        startObserving(SubjectType* subject)
        {
            subject->addObserver(this);
            this->addSubject(subject);
        }

        /**
         * @brief Nicer to read: oberserver->stopObserving(subject);
         */
        void
        stopObserving(SubjectType* subject)
        {
            subject->removeObserver(this);
            this->removeSubject(subject);
        }

    private:
        /**
         * @copydoc ObserverInterface::addSubject().
         *
         * All subjects that register themselves via this function will
         * receive a detach when this Observer instance is destroyed.
         */
        virtual void
        addSubject(SubjectType* subject)
        {
            assureNotNull(subject);
            assure(
                find(this->subjects.begin(), this->subjects.end(), subject) == this->subjects.end(),
                "Already registered");
            this->subjects.push_back(subject);
        }

        /**
         * @copydoc ObserverInterface::removeSubject().
         */
        virtual void
        removeSubject(SubjectType* subject)
        {
            assureNotNull(subject);
            assure(
                find(this->subjects.begin(), this->subjects.end(), subject) != this->subjects.end(),
                "Not registered");

            this->subjects.erase(find(this->subjects.begin(), this->subjects.end(), subject));
        }

        /**
         * @brief Execute @a functor on each subject.
         *
         * @param functor The function to perform on each subject.
         *
         * @returns The functor object passed as @a functor in the
         * state after it has been applied to all subjects.
         */
        template <typename FUNCTOR>
        FUNCTOR
        forEachSubject(const FUNCTOR& functor)
        {
            // copy, so the container gets not corrupted during for_each ...
            SubjectContainer tmp = this->subjects;
            return std::for_each(tmp.begin(), tmp.end(), functor);
        }

        typedef std::list<SubjectType*> SubjectContainer;

        /**
         * @brief Collection of registered subjects.
         *
         * Collection of registered subjects.
         */
        SubjectContainer subjects;
    };
} // wns

#endif
