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
#ifndef WNS_LDK_FCF_NEWFRAMEOBSERVERINTERFACE_H
#define WNS_LDK_FCF_NEWFRAMEOBSERVERINTERFACE_H

#include <list>
#include <WNS/logger/Logger.hpp>


namespace wns { namespace ldk { namespace fcf {

class NewFrameProvider;

class NewFrameObserver
    {
    public:
        NewFrameObserver(std::string observerName);

        virtual
        ~NewFrameObserver();

        void
        setNewFrameProvider(NewFrameProvider* newFrameProvider);

        virtual void
        newFrameProviderDeleted();

        virtual void
        messageNewFrame() = 0;

        std::string
        getObserverName() const {return observerName_;}

        NewFrameProvider*
        testGetNewFrameProvider_()
            {
                return newFrameProvider_;
            }

    private:
        const std::string observerName_;

        NewFrameProvider* newFrameProvider_;
    };



    class NewFrameProvider
    {
    public:
        typedef std::list<NewFrameObserver*> NewFrameObservers;

        NewFrameProvider(std::string stationName);

        ~NewFrameProvider();

        void
        attachObserver(NewFrameObserver* newFrameObserver);

        void
        detachObserver(NewFrameObserver* newFrameObserver);

        void
        notifyNewFrameObservers();

        NewFrameObservers
        testGetNewFrameObservers_()
        {
            return newFrameObservers_;
        }


    private:

        void
        pl();

        NewFrameObservers observersToNotify_;
        NewFrameObservers newFrameObservers_;

        const std::string stationName_;
        wns::logger::Logger logger_;
    };

}
}
} // wns ldk fcf

#endif //WNS_LDK_FCF_NEWFRAMEOBSERVERINTERFACE_H


