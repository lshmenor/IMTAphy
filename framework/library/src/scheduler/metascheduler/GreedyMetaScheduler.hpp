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

#ifndef WNS_SCHEDULER_METASCHEDULER_GREEDYMETASCHEDULER_HPP
#define WNS_SCHEDULER_METASCHEDULER_GREEDYMETASCHEDULER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>

#include <WNS/container/Registry.hpp>
#include <WNS/scheduler/strategy/StrategyInterface.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/scheduler/metascheduler/IMetaScheduler.hpp>
#include <WNS/scheduler/metascheduler/MetaScheduler.hpp>
#include <WNS/Singleton.hpp>
#include <WNS/scheduler/strategy/Strategy.hpp>




namespace wns { namespace scheduler{ namespace metascheduler{
	
	
	class GreedyMetaScheduler:public MetaScheduler
	//, public wns::scheduler::strategy::Strategy 
	{
	  
	  public:
	    
		GreedyMetaScheduler(const wns::pyconfig::View& _config);
		~GreedyMetaScheduler(){};
				
		/**
		 * @brief Applies a Greedy Algorithm to the ThroughputMatrix.
		 *
		 */			
		void optimize(const UtilityMatrix& throughputMatrix, std::vector< std::vector<int> >& vBestCombinations);	
		
	private:

	};
		  
}
}
}




#endif // WNS_SCHEDULER_METASCHEDULER_GREEDYMETASCHEDULER_HPP

