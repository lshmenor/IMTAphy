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

#include <WNS/scheduler/metascheduler/IMetaScheduler.hpp>
#include <WNS/StaticFactoryBroker.hpp>
#include <WNS/Singleton.hpp>

using namespace wns::scheduler::metascheduler;

wns::scheduler::UserID BSInfo::getUserInFrequency (int frequency, const std::vector<int>& userMapping)
{
  int currentFreq = 0;
  for (int i=0; i < resourceBlockSizes.size(); ++i)
  {
    currentFreq += resourceBlockSizes[i];
    
    if (frequency < currentFreq)
      return vActiveUsers[userMapping[i]];
  }
}


int BSInfo::getPhyModeIndexInFrequency (int frequency, const std::vector<int>& phyModeMapping)
{
  int currentFreq = 0;
  for (int i=0; i < resourceBlockSizes.size(); ++i)
  {
    currentFreq += resourceBlockSizes[i];
    
    if (frequency < currentFreq)
      return phyModeMapping[i];
  }
}



bool PeerResource::operator<(const PeerResource& b) const
{
  if (this->timeSlot != b.timeSlot)
  {
    return (this->timeSlot < b.timeSlot);
  }
  else
  {
    return (this->subChannel < b.subChannel);
  }
}
/*
bool operator<(const PeerResource &a, const PeerResource& b)
{
  if (a.timeSlot != b.timeSlot)
  {
    return (a.timeSlot < b.timeSlot);
  }
  else
  {
    return (a.subChannel < b.subChannel);
  }
}
*/

IMetaScheduler::IMetaScheduler():metaSchedulerCPUCycles_("MetaScheduler.cpuCycles"),metaSchedulerTime_("MetaScheduler.Time")
{


}


IMetaScheduler*
IMetaScheduler::getMetaScheduler(const wns::pyconfig::View& config)
{
	//Debugging
	////std::cout<<"Welcome to getMetaScheduler!!!"<<std::endl;
	
	std::string name = config.get<std::string>("plugin");
	
	
	if (metaScheduler)
	  return metaScheduler;
	//Debugging
	////std::cout<<"IMetaScheduler: "<<name<<std::endl;
	
	// Define shorthand for the staticfactorybroker typename
	typedef wns::StaticFactoryBroker<IMetaScheduler, MetaSchedulerCreator > BrokerType;
	
	// Obtain the broker from its singleton
	BrokerType* broker = wns::SingletonHolder<BrokerType>::getInstance();
	metaScheduler = broker->procure(name, config);
	
	//std::cout << "*****getMetaScheduler Ptr: " << config << std::endl;
	return metaScheduler;
}

IMetaScheduler::~IMetaScheduler() {};

IMetaScheduler* IMetaScheduler::metaScheduler = NULL;
