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

#include <WNS/scheduler/metascheduler/GreedyMetaScheduler.hpp>
#include <WNS/scheduler/metascheduler/MetaScheduler.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>

#include <boost/algorithm/string.hpp>

using namespace wns::scheduler::strategy;
using namespace wns::scheduler::metascheduler;


STATIC_FACTORY_REGISTER_WITH_CREATOR(GreedyMetaScheduler,
                                     IMetaScheduler,
                                     "GreedyMetaScheduler",
                                     wns::PyConfigViewCreator);




GreedyMetaScheduler::GreedyMetaScheduler(const wns::pyconfig::View& _config):
    MetaScheduler(_config)  
{

}


void GreedyMetaScheduler::optimize(const UtilityMatrix& throughputMatrix, 
                                   std::vector< std::vector<int> >& vBestCombinations)
{
  
  // matrix parameter
  int iBaseStations = throughputMatrix.getDimensions().first;
  int iNumberUTperBS = throughputMatrix.getDimensions().second[0];
  int iMatrixSize = throughputMatrix.getMatrixSize();

  
  // variables

  
  std::vector<int> vBaseStationsCounter;
  std::vector<int> vBaseStationsSize;
  std::vector< std::vector<bool> > vValidIndices (iBaseStations);
   
  
  // Setup data
  for (int i = 0; i < iBaseStations; i++)
  {
    vBaseStationsSize.push_back(iNumberUTperBS);
    vValidIndices[i].resize (iNumberUTperBS , true);
  }
  
  
  //greedy
  for (int b=0; b < iNumberUTperBS; ++b)
  {
    double dAccumValue = 0;
       
    std::vector<int> vCurrentBest (iBaseStations, 0);
    double currentBestValue = 0;
    
    vBaseStationsCounter.clear();
    vBaseStationsCounter.resize(iBaseStations, 0);
  
    for (int i=0; i < iMatrixSize; ++i)
    {
      //Walk over matrix
      for (int j=0; j < iBaseStations; ++j)
      {
        vBaseStationsCounter[j]++;
        
        if (vBaseStationsCounter[j] == iNumberUTperBS)
        {
          vBaseStationsCounter[j] = 0;
          continue;
        }
        else
          break;
      }
      
    
      //test if a previously used line is present in the counter and skip it
      bool bBlocked = false;
      for (int j=0; j < iBaseStations; ++j)
      {
        if (!vValidIndices[j][vBaseStationsCounter[j]])
        {
          bBlocked = true;
          break;
        }
      }
      if (bBlocked)
      {
        continue;
      }
      
      double dValue = throughputMatrix.getValue(vBaseStationsCounter);
      
      if (dValue > currentBestValue)
      {
        currentBestValue = dValue;
        vCurrentBest = vBaseStationsCounter;
      }
    }
    
    dAccumValue += currentBestValue;  
    
    
    for (int iBS=0; iBS < iBaseStations; ++iBS)
    {
      vBestCombinations[iBS][b] = vCurrentBest[iBS];
      vValidIndices[iBS][vCurrentBest[iBS]] = false;
    } 
  } 
}
