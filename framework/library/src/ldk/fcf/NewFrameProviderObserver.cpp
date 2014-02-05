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

#include "NewFrameProviderObserver.hpp"

#include <WNS/Assure.hpp>


using namespace  wns::ldk::fcf;


/************ NewFrameProvider **********************/

NewFrameProvider::NewFrameProvider(std::string stationName) :
    observersToNotify_(),
    newFrameObservers_(),
    stationName_(stationName),
    logger_("WNS", "NewFrameProvider")
{
} //NewFrameProvider



NewFrameProvider::~NewFrameProvider()
{
    for(NewFrameObservers::iterator iter = newFrameObservers_.begin();
        iter != newFrameObservers_.end(); ++iter)
    {
        (*iter)->setNewFrameProvider(NULL);
    }
}



void
NewFrameProvider::attachObserver(NewFrameObserver* newFrameObserver)
{
    assure(std::find(newFrameObservers_.begin(),
             newFrameObservers_.end(),
             newFrameObserver)
           == newFrameObservers_.end(),
           "NewFrameObserver is already added to NewFrameProvider");

    MESSAGE_BEGIN(NORMAL, logger_, m, stationName_ );
    m << ": Attach observer! "
      << newFrameObserver->getObserverName();
    MESSAGE_END();

    newFrameObservers_.push_back(newFrameObserver);
    newFrameObserver->setNewFrameProvider(this);
} //attachNewFrameObserver



void
NewFrameProvider::detachObserver(NewFrameObserver* newFrameObserver)
{
    assure(std::find(newFrameObservers_.begin(),
             newFrameObservers_.end(),
             newFrameObserver)
           != newFrameObservers_.end(),
           "unknown NewFrameObserver");

    MESSAGE_BEGIN(NORMAL, logger_, m, stationName_ );
    m << ": Detach observer! "
      << newFrameObserver->getObserverName();
    MESSAGE_END();

    newFrameObserver->setNewFrameProvider(NULL);
    newFrameObserver->newFrameProviderDeleted();
    newFrameObservers_.remove(newFrameObserver);

    // remove observer from list observersToNotify_, because it is detached
    if(	std::find(observersToNotify_.begin(), observersToNotify_.end(),
                  newFrameObserver) != newFrameObservers_.end() )
    	observersToNotify_.remove(newFrameObserver);

} //detachNewFrameObserver



void
NewFrameProvider::notifyNewFrameObservers()
{
    // Copy of list is necessary, because elements of List could deleted while
    // iterating throw the list
    observersToNotify_ = newFrameObservers_;

    // Send NewFrame message to observer
    MESSAGE_BEGIN(NORMAL, logger_, m, stationName_ );
    m << ": Notify all observers! newFrameObservers_.size():" << observersToNotify_.size();
    MESSAGE_END();

    while(!observersToNotify_.empty())
    {
        NewFrameObserver* observer = observersToNotify_.front();
        observersToNotify_.pop_front();
        observer->messageNewFrame();
    }
} //notifyNewFrameObservers



/************ NewFrameObserver **********************/

NewFrameObserver::NewFrameObserver(std::string observerName):
    observerName_(observerName),
    newFrameProvider_(NULL)
{
} // NewFrameObserver



NewFrameObserver::~NewFrameObserver()
{
    if (newFrameProvider_)
    {
        newFrameProvider_->detachObserver(this);
        newFrameProvider_ = NULL;
    }

} // ~NewFrameObserver



void
NewFrameObserver::setNewFrameProvider(NewFrameProvider* newFrameProvider)
{
    newFrameProvider_ = newFrameProvider;
} // setNewFrameProvider



void
NewFrameObserver::newFrameProviderDeleted()
{
} //newFrameProviderDeleted


