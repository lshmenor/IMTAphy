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

#ifndef WNS_OBSERVERINTERFACE_HPP
#define WNS_OBSERVERINTERFACE_HPP

#include <WNS/SubjectInterface.hpp>

namespace wns {

    // We need to make Subject a friend of ObserverInterface
    template <class NOTIFICATIONINTERFACE>
    class Subject;

    /**
     * @brief An observer's Interface to a subject.
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @author Swen Kuehnlein
     *
     * @internal
     *
     * @param NOTIFICATIONINTERFACE The interface that contains the notification
     * function. ObserverInterface will derive from that.
     *
     * @note
     * You should not need to instantiate this template yourself, as you can
     * refer to the corresponding instantiation via
     * Subject<NOTIFICATIONINTERFACE>::ObserverInterface and
     * Observer<NOTIFICATIONINTERFACE>::ObserverInterface.
     *
     * @remarks
     * To resolve the mutual dependency of ObserverInterface and SubjectInterface,
     * ObserverInterface includes SubjectInterface.hpp, whereas SubjectInterface
     * gets the ObserverInterface as template parameter.
     *
     * @see SubjectInterface, Observer, Subject.
     *
     * @ingroup MISC
     */
    template <class NOTIFICATIONINTERFACE>
    class ObserverInterface :
        virtual public NOTIFICATIONINTERFACE
    {
        /**
         * @brief Only Subject should use this class.
         *
         * Subject is the only class with access to addSubject() and removeSubject().
         */
        friend class Subject<NOTIFICATIONINTERFACE>;
    public:
        /**
         * @brief The NotificationInterface used by ObserverInterface and @link ObserverInterface::SubjectInterface SubjectInterface@endlink.
         *
         * This provides the NotificationInterface that is used by both this ObserverInterface
         * and @link ObserverInterface::SubjectInterface SubjectInterface@endlink.
         */
        typedef NOTIFICATIONINTERFACE NotificationType;

        /**
         * @brief The SubjectInterface corresponding to this ObserverInterface.
         *
         * This always refers to the SubjectInterface that uses the same NotificationInterface.
         */
        typedef SubjectInterface<ObserverInterface> SubjectType;

        virtual
        ~ObserverInterface()
        {
        }

    private:
        /**
         * @brief Add a subject we are attached to.
         *
         * @param subject A subject we are attached to.
         *
         * @pre The ObserverInterface instance must be attached to @a subject.
         *
         * @post The ObserverInterface instance will detach itself from @a subject
         * on destruction.
         */
        virtual void
        addSubject(SubjectType* subject) = 0;

        /**
         * @brief Remove a subject we were attached to.
         *
         * @param subject The subject to remove.
         *
         * @pre @a subject must have been previously added with addSubject().
         *
         * @post The ObserverInterface install won't access @a subject in any way
         * after the call is finished.
         */
        virtual void
        removeSubject(SubjectType* subject) = 0;
    };
}

#endif
