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
#include <sys/time.h> 


#include <WNS/scheduler/metascheduler/MaxRegretMetaScheduler.hpp>
#include <WNS/scheduler/metascheduler/MetaScheduler.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>

#include <boost/algorithm/string.hpp>

#include <sys/times.h>
#include <time.h>

using namespace wns::scheduler::strategy;
using namespace wns::scheduler::metascheduler;


STATIC_FACTORY_REGISTER_WITH_CREATOR(MaxRegretMetaScheduler,
                                     IMetaScheduler,
                                     "MaxRegretMetaScheduler",
                                     wns::PyConfigViewCreator);




MaxRegretMetaScheduler::MaxRegretMetaScheduler(const wns::pyconfig::View& _config):MetaScheduler(_config)
{

}

  
void MaxRegretMetaScheduler::optimize(const UtilityMatrix& throughputMatrix, std::vector< std::vector<int> >& vBestCombinations)
{
  
  // matrix parameter
  int iBaseStations = throughputMatrix.getDimensions().first;
  int iNumberUTperBS = throughputMatrix.getDimensions().second[0];
  int iMatrixSize = throughputMatrix.getMatrixSize();

    
  // variables
  double dAccumValue = 0;  
  double bestDataRate = 0.0;
  static bool bComputed = false;
  
  std::vector<int> vBaseStationsCounter;
  std::vector<int> vBaseStationsSize;
  std::vector< std::vector<bool> > vValidIndices (iBaseStations);
  std::vector< std::vector<double> > MaxValues (iBaseStations); 
  
  // Setup data
  for (int i = 0; i < iBaseStations; i++)
  {
    std::vector<int> vCombination;
    int iSize = iNumberUTperBS; 
    
    vBaseStationsSize.push_back(iSize);
    vValidIndices[i].resize (vBaseStationsSize[i], true);
    MaxValues[i].resize (vBaseStationsSize[i], 0.0);
  }
  
  vBaseStationsCounter.clear();
  vBaseStationsCounter.resize(iBaseStations, 0);
  
    

  ////Max Regret Greedy
  
 // Over the number of final assignments
  for (int b=0; b < iNumberUTperBS; ++b)
  {
    std::vector<int> vCurrentBest (iBaseStations, 0);
    double currentBestRegret = -1.0;
    double currentDataRate = 0.0;
    
    //Over all BSs 
    for (int b2=0; b2 < iBaseStations; ++b2)
    {
      //Over all UTs of each BS
      for (int ut=0; ut < iNumberUTperBS; ++ut)
      {

        std::set<wns::scheduler::metascheduler::WeightTuple> twoBestVectors;
        
        if (!vValidIndices[b2][ut])
        {
          continue;
        }
        
        // prepare the counter
        vBaseStationsCounter.clear();
        vBaseStationsCounter.resize(iBaseStations, 0);
        // set the observed UT in the counter
        vBaseStationsCounter[b2] = ut;
        
        // seach for two biggest vectors
        for (int i=0; i < (iMatrixSize/iNumberUTperBS); ++i)
        {    
          //Walk over matrix
          for (int m1=0; m1 < iBaseStations; ++m1)
          {
            //fixed
            if (m1 == b2)
            { 
              continue;
            }
            
            vBaseStationsCounter[m1]++;
            
            // if counter has reached the maximum number of UTs, it is resetted
            if (vBaseStationsCounter[m1] == iNumberUTperBS)
            {
              vBaseStationsCounter[m1] = 0;
              continue;
            }
            else
              break;
          }
          
     
          //Test for a valid vector
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
          
          //Get Value
          double dValue = throughputMatrix.getValue(vBaseStationsCounter);
          
          twoBestVectors.insert(wns::scheduler::metascheduler::WeightTuple(dValue, vBaseStationsCounter));
          
          if (twoBestVectors.size() > 2)
          {
            twoBestVectors.erase(twoBestVectors.begin());
          }
        }
        

        
        
        if (twoBestVectors.size() == 0)
        {
          continue;
        }
        else if (twoBestVectors.size() == 1)
        {
          if (0.0 > currentBestRegret)
          {
            vCurrentBest = twoBestVectors.rbegin()->_pos;
            currentBestRegret = 0.0;
            currentDataRate = twoBestVectors.rbegin()->_weight;
          }
        }
        else if (twoBestVectors.size() == 2)
        {
          double regret = (twoBestVectors.rbegin()->_weight - twoBestVectors.begin()->_weight);
          
          if (regret > currentBestRegret)
          {
            vCurrentBest = twoBestVectors.rbegin()->_pos;
            currentBestRegret = regret;
            currentDataRate = twoBestVectors.rbegin()->_weight;
          }
        }
      }
    }
    
    dAccumValue += currentDataRate;
    
    //Apply greedy result to best Combination vectors

    for (int j=0; j < iBaseStations; ++j)
    {
      (vBestCombinations)[j][b] = vCurrentBest[j];
      vValidIndices[j][vCurrentBest[j]] = false;
    }
  }
  


  
}




