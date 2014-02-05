/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#ifndef DLL_SERVICES_ASSOCIATION_HPP
#define DLL_SERVICES_ASSOCIATION_HPP

#include <WNS/ldk/ControlServiceInterface.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/Subject.hpp>
#include <WNS/Observer.hpp>

#include <vector>
#include <list>

namespace dll {
    class ILayer2;
    class StationManager;
}
namespace dll { namespace services { namespace control {

    class AssociationObserverInterface
    {
    public:
        virtual
        void onAssociated(wns::service::dll::UnicastAddress userAdr,
                  wns::service::dll::UnicastAddress dstAdr) = 0;

        virtual
        void onDisassociated(wns::service::dll::UnicastAddress userAdr,
                     wns::service::dll::UnicastAddress dstAdr) = 0;

        virtual
        ~AssociationObserverInterface(){};
    };


    class AssociationFunctor
    {
        typedef void (AssociationObserverInterface::*fPtr)(wns::service::dll::UnicastAddress, wns::service::dll::UnicastAddress);

        fPtr f;
        wns::service::dll::UnicastAddress user;
        wns::service::dll::UnicastAddress dst;
    public:
        AssociationFunctor(fPtr _f,
                   wns::service::dll::UnicastAddress _user,
                   wns::service::dll::UnicastAddress _dst) :
            f(_f),
            user(_user),
            dst(_dst)
        {
        }

        void
        operator()(AssociationObserverInterface* observer)
        {
            (*observer.*f)(user, dst);
        }
    };

    typedef wns::Subject<AssociationObserverInterface> AssociationInfo;
    typedef wns::Observer<AssociationObserverInterface> AssociationObserver;

    /** @brief Control Plane Service to handle associations */
    class Association :
        public wns::ldk::ControlService,
        public AssociationInfo
    {
    public:
        Association( wns::ldk::ControlServiceRegistry* csr, const wns::pyconfig::View& config);
        virtual ~Association(){}

        /** @name Methods to query about associations between dll::ILayer2 Instances */
        //@{
        /** @brief return this stations association (master station) */
        wns::service::dll::UnicastAddress
        getAssociation();
        /** @brief does this station have an association (master station) ? */
        bool
        hasAssociation();
        /** @brief does this master station have the associated client*/
        bool
        hasAssociated(const wns::service::dll::UnicastAddress& slave) const;
        /** @brief return all my slave associations (i.e., all stations
         * associated to me, may be 0..many) */
        std::vector<wns::service::dll::UnicastAddress>
        getAssociated();
        //@}

        /** @name Association handling */
        //@{
        /** @brief set association between source and a "master station" -- address version*/
        void
        associate(wns::service::dll::UnicastAddress slave, wns::service::dll::UnicastAddress master);
        /** @brief set association between source and a "master station" -- ILayer2* version*/
        void
        associate(dll::ILayer2* slave, dll::ILayer2* master);
        /** @brief release association to my "master station" */
        void
        releaseFromMaster();

        /** @brief release association of a slave station -- ILayer2* version*/
        void
        releaseClient(dll::ILayer2*);
        /** @brief release association of a slave station -- address version*/
        void
        releaseClient(wns::service::dll::UnicastAddress slave);
        //@}

    private:
        /** @brief add an association from a slave station to this */
        void
        registerAssociation(wns::service::dll::UnicastAddress slave, wns::service::dll::UnicastAddress master);

        wns::ldk::ControlServiceRegistry* csr_;

        /** @brief The name this service is registered with */
        std::string serviceName_;

        /** @brief The station we are associated to */
        wns::service::dll::UnicastAddress masterStation_;
        /** @brief vector of all stations that are associated to us. */
        std::list<wns::service::dll::UnicastAddress> slaveStations_;

        ///** @brief list of observers to notify when association changes. */
        //std::list<AssociationObserver*> observers;

        /** @brief Logger for debug output */
        wns::logger::Logger logger_;

        /** @brief Access to station manager for DLL->ILayer2* */
        dll::StationManager* stationManager_;
    };
}
}
}

#endif // NOT defined DLL_SERVICES_ASSOCIATION_HPP



