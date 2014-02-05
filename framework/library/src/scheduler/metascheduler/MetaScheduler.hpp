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

#ifndef WNS_SCHEDULER_METASCHEDULER_METASCHEDULER_HPP
#define WNS_SCHEDULER_METASCHEDULER_METASCHEDULER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>

#include <WNS/container/Registry.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/scheduler/metascheduler/IMetaScheduler.hpp>
#include <WNS/Singleton.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>



//#include <WIMAC/scheduler/Scheduler.hpp>



//using namespace wimac::scheduler;


namespace wns { namespace scheduler{ namespace metascheduler{
	
	class UtilityMatrix: public wns::IOutputStreamable {
	  public:
	    void createMatrix (int baseStations, std::vector<int>& userTerminalsInBaseStations);
	    void setValue (std::vector<int>& userIndices, double value);
	    
        double getValue (std::vector<int>& userIndices) const;	    
        std::pair<int, std::vector<int> >getDimensions() const;
        int getMatrixSize(void) const;
	    void Print (void);
	    //friend std::ostream& operator<<(std::ostream &stream, const InterferenceMatrix& m);
	    
	  private:
        virtual std::string
        doToString() const;
	    
        int _baseStations;
        int _matrixSize;
	    std::vector<int> _userTerminalsInBaseStations;
	    std::vector<int> _indexJumpOfBaseStation;
	    std::vector<double> _data;
	};
	
	/*std::ostream& operator<<(std::ostream &stream, const InterferenceMatrix& m)
	{
	  //We only print the first two dim
	  if (m._baseStations < 2)
	    return stream;
	  
	  for (int y=0; y < m._userTerminalsInBaseStations[1]; ++y)
	  {
	    for (int x=0; x < m._userTerminalsInBaseStations[0]; ++x)
	    {
	      stream << m._data[x + y * m._indexJumpOfBaseStation[1]] << " ";
	    }
	    stream << std::endl;
	  }
	  stream << std::endl;
	  return stream;
	}*/

	class MetaScheduler : public IMetaScheduler
	//, public wns::scheduler::strategy::Strategy 
	{
	  
	  public:
	    
		MetaScheduler(const wns::pyconfig::View& _config);//: availableFrequencyChannels(0), numberBS(0),numberCount(1) {}						
		
		~MetaScheduler()=0;
				
		 
		/**
		 * @brief Attach a Scheduler to the MetaScheduler
		 *
		 * Called by the constructor of the Scheduler, hence there is no
		 * need to call this method explictly.
		 */		
		virtual void 
        attachBS(const wns::pyconfig::View *pyConfig, wns::scheduler::RegistryProxyInterface* registryProxy, bool IamUplinkMaster);
		
		virtual void 
		attachUT(const wns::pyconfig::View *pyConfig, wns::scheduler::RegistryProxyInterface* registryProxy);	

        
		/**
		 * @brief Modifys a SchedulingMap.
		 *
		 */			
		void 
		provideMetaConfiguration(wns::scheduler::UserID UserID, 
                                 wns::scheduler::SchedulingMapPtr schedulingMap, bool bUplink, 
                                 const wns::scheduler::strategy::StrategyInput* strategyInput);
		
		/**
		 * @brief Determines the active UTs of a BS in a frame and returns a change.
		 *
		 */			
		bool 
		setActiveUserSet(BSInfo* p_BSInfo, int frameNr);
		
		
		/**
		 * @brief Determines the active UTs of a BS in a frame.
		 *
		 */
		void 
		computeRessourceBlockSizes (BSInfo* p_BSInfo);				      
						      
		
		/**
		 * @brief Writes the meta schedule into the schedulingMap 
		 *
		 */
		void 
		applyMetaSchedule(void);
		
		/**
		 * @brief Calls an assignment strategy 
		 *
		 */
        virtual void 
        optimize(const UtilityMatrix& throughputMatrix, std::vector< std::vector<int> >& vBestCombinations)=0;
        
		
		
		/**
		 * @brief Removes subchannels from the interference cache that are not used by the UT 
		 *
		 */		
		void 
		updateUserSubchannels(void);
		
        
        /**
         * @brief Returns the Position of an UT in its BS's activeUserSet 
         *
         */     
        int 
        getPositionOfUTinBSactiveUserSet(const BSInfo* pBS ,const wns::scheduler::UserID);
        

        
        
		/**
		 * @brief Sets a Phy-Mode for a UT in a schedulingMap 
		 *
		 */		
		void 
		setPhyModeForPRB(wns::scheduler::UserID userID, wns::scheduler::SchedulingMap & schedulingMap,
                         int subChannel, int timeSlot, int spatialLayer);
		void 
		setPhyModeForPRB(wns::scheduler::UserID userID, wns::scheduler::SchedulingMap & schedulingMap,
                         int subChannel, int timeSlot, int spatialLayer, wns::service::phy::phymode::PhyModeInterfacePtr pm);
				

		 
		 /**
		 * @brief Determines the Interference Map for each BS
		 *
		 */				
        bool 
        computeInterferenceMap (void);
         
        
         /**
         * @brief Computes the Maximum Throughput for an UT with given Interferer
         *
         */             
        double 
        getMaximumThroughputForUser (BSInfo* pBS, wns::scheduler::UserID user, 
                                     std::set<wns::scheduler::UserID>& interferer);
        wns::Ratio 
        getSINR (BSInfo* pBS, wns::scheduler::UserID user, std::set<wns::scheduler::UserID>& interferer);
		 
		 
         /**
         * @brief reserves PRBs according to the parameters 
         *
         */
        void 
        reservePRB(wns::scheduler::PhysicalResourceBlock *prbDescriptor,wns::scheduler::UserID userID);
        void 
        reservePRB(wns::scheduler::UserID userID , wns::scheduler::SchedulingMap & schedulingMap, 
                   int subChannel, int timeSlot, int spatialLayer);
        
        
        
        /**
         * @brief provides interferer on a RB 
         *
         */            
        void 
        getInterfererOnFrequency (BSInfo* pBS, int iFrequency, std::vector<std::vector<int> >& currentCombination,
                                  std::set<wns::scheduler::UserID>& interferer);
        void 
        getInterfererOnFrequency (wns::scheduler::UserID user, int iFrequency, 
                                  std::set<wns::scheduler::UserID>& interferer);
                      
                   
	  private:
		 
	    UtilityMatrix throughputMatrix;
             
		 
	  protected:
        wns::Power defaultCarrier;
		wns::Power defaultInterference;
		wns::Ratio defaultPathloss;
	    

	};
		  
      }
    }

}




#endif // WNS_SCHEDULER_METASCHEDULER_METASCHEDULER_HPP
