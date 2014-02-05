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

#ifndef WNS_SUBJECTINTERFACE_HPP
#define WNS_SUBJECTINTERFACE_HPP

namespace wns {

    // We need to make Observer a friend of SubjectInterface
    template <class OBSERVERINTERFACE>
    class Observer;

    /**
     * @brief A subject's Interface to an observer.
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @author Swen Kuehnlein
     *
     * @internal
     *
     * @param OBSERVERINTERFACE The observer interface to use by
     * addObserver/removeObserver.
     *
     * @note
     * You should not need to instantiate this template yourself, as you can
     * refer to the corresponding instantiation via
     * Subject<NOTIFICATIONINTERFACE>::SubjectInterface and
     * Observer<NOTIFICATIONINTERFACE>::SubjectInterface.
     *
     * @remarks
     * To resolve the mutual dependency of ObserverInterface and
     * SubjectInterface, ObserverInterface includes SubjectInterface.hpp,
     * whereas SubjectInterface gets the ObserverInterface as template
     * parameter.
     *
     * @see ObserverInterface, Subject, Observer.
     *
     * @ingroup MISC
     */
    template <class OBSERVERINTERFACE>
    class SubjectInterface
    {
        /**
         * @brief Only Observer should use this class.
         *
         * Observer is the only class with access to addObserver() and
         * removeObserver().
         */
        friend class Observer<typename OBSERVERINTERFACE::NotificationType>;
    public:
        /**
         * @brief The ObserverInterface corresponding to this
         * SubjectInterface.
         *
         * This always refers to the ObserverInterface that uses the
         * same NotificationInterface.
         */
        typedef OBSERVERINTERFACE ObserverType;

        /**
         * @brief The NotificationInterface used by SubjectInterface and
         * OBSERVERINTERFACE.
         *
         * This provides the NotificationInterface that is used by both
         * this SubjectInterface and OBSERVERINTERFACE.
         */
        typedef typename ObserverType::NotificationType NotificationType;

        virtual
        ~SubjectInterface()
        {
        }

    private:
        /**
         * @brief Add an observer to a subject.
         *
         * @param observer The observer to add.
         *
         * @pre @a observer must not have been added before.
         *
         * @post @a observer is attached an will receive notifications
         * according to @link OBSERVERINTERFACE::NotificationInterface
         * NotificationInterface@endlink and OBSERVERINTERFACE.
         */
        virtual void
        addObserver(ObserverType* observer) = 0;

        /**
         * @brief Remove an observer from a subject.
         *
         * @param observer The observer to remove.
         *
         * @pre @a observer must have been added before.
         *
         * @post @a observer is not attached an will not receive any
         * notifications.
         */
        virtual void
        removeObserver(ObserverType* observer) = 0;
    };

}

#endif
