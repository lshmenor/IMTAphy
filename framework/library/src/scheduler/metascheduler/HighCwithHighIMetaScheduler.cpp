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

#include <string>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits.h>
#include <math.h>
#include <sys/times.h>
#include <sys/time.h> 
#include <time.h>

#include <WNS/scheduler/metascheduler/HighCwithHighIMetaScheduler.hpp>
#include <WNS/scheduler/metascheduler/MetaScheduler.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>

#include <boost/algorithm/string.hpp>

using namespace wns::scheduler::strategy;
using namespace wns::scheduler::metascheduler;


STATIC_FACTORY_REGISTER_WITH_CREATOR(HighCwithHighIMetaScheduler,
                                     IMetaScheduler,
                                     "HighCwithHighIMetaScheduler",
                                     wns::PyConfigViewCreator);




HighCwithHighIMetaScheduler::HighCwithHighIMetaScheduler(const wns::pyconfig::View& _config):
    MetaScheduler(_config)  
{

}


void HighCwithHighIMetaScheduler::optimize(const UtilityMatrix& throughputMatrix, 
                                   std::vector< std::vector<int> >& vBestCombinations)
{
  
  
    // matrix parameter
  int iBaseStations = throughputMatrix.getDimensions().first;
  int iNumberUTperBS = throughputMatrix.getDimensions().second[0];
  int iMatrixSize = throughputMatrix.getMatrixSize();

  
  // variables
  double dAccumValue = 0;
  std::vector<int> vBaseStationsCounter;
  std::vector<int> vBaseStationsSize;
  std::vector< std::vector<bool> > vValidIndices (iBaseStations);
   
  // Setup data
  for (int i = 0; i < iBaseStations; i++)
  {
    int iSize = baseStations[i]->vActiveUsers.size(); 
    vBaseStationsSize.push_back(iSize);
    vValidIndices[i].resize (vBaseStationsSize[i], true);
    iMatrixSize *= iSize;
  }
  
    
  for (int iBS=0; iBS < iBaseStations; iBS++)
  {
    // Set first BS with decreasing carriers to TBs
    if(iBS==0)
    {
      std::vector<int> vCurrentBest;
      
      std::multimap<wns::Power, wns::scheduler::UserID>::reverse_iterator revIt;
      
      assure(iNumberUTperBS==baseStations[iBS]->vActiveUsers.size(), "Sizes do not match!!");
      
      
      for(revIt = baseStations[iBS]->carrierMultimap.rbegin(); revIt != baseStations[iBS]->carrierMultimap.rend(); revIt++)
      {
         vCurrentBest.push_back(this->getPositionOfUTinBSactiveUserSet(baseStations[iBS],revIt->second));
      }
      
      vBestCombinations[iBS]= vCurrentBest;
     
      continue;
    }
       
    
    // Compute and order Interference caused by the active UTs of the previous neighbouring BSs
    std::multimap<wns::Power, int> interferenceAtTB;
    std::vector<int> vCurrentBest (iNumberUTperBS, 0);
    
     
    for (int iTBPos=0; iTBPos < iNumberUTperBS; iTBPos++)
    {
      
      wns::Power interference ;
      interference.set_dBm(-116.440);
      
      for (int iIn=0; iIn < iBS; iIn++)
      {   
          interference += (baseStations[iBS]->interferenceMap.find(
            baseStations[iIn]->vActiveUsers[vBestCombinations[iIn][iTBPos]].getNodeID())
              ->second.carrier); 
      }
     
      interferenceAtTB.insert( 
          std::pair<wns::Power, int>(interference, iTBPos) );
    }
   
  
    std::multimap<wns::Power, wns::scheduler::UserID>::const_iterator ita;
    std::multimap<wns::Power, int>::const_iterator itb;
      
 
    for(itb = interferenceAtTB.begin(), ita = baseStations[iBS]->carrierMultimap.begin(); itb !=interferenceAtTB.end(); itb++,ita++)
    {
       vCurrentBest[itb->second]= this->getPositionOfUTinBSactiveUserSet(baseStations[iBS],ita->second);    
    }
    
   vBestCombinations[iBS]= vCurrentBest; 
   
  }
  
  
 // Print Best Combination
 /*
 std::cout<<"HCHI"<<std::endl;
  for (int b=0; b < iBaseStations; ++b)
  {
    std::cout  << "BS: " << baseStations[b]->BSID.getName() << " ";
    for (int i=0; i < baseStations[b]->bestCombination.size(); ++i)
    {
      std::cout  << (vBestCombinations)[b][i]<< ", ";//baseStations[b]->bestCombination[i] <<"-"<<(vBestCombinations)[b][i]<< ", ";
    }
    std::cout << std::endl;
  }
*/
}
