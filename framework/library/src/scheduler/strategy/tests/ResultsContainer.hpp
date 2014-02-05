/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
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

#ifndef WNS_SCHEDULER_STRATEGY_TEST_RESULTSCONTAINER_HPP
#define WNS_SCHEDULER_STRATEGY_TEST_RESULTSCONTAINER_HPP

#include <WNS/scheduler/CallBackInterface.hpp>
#include <set>
#include <vector>
#include <string>
#include <fstream>

namespace wns { namespace scheduler { namespace strategy { namespace tests {

	class ResultsContainer :
		public wns::scheduler::CallBackInterface
	{
	public:
		ResultsContainer();
		~ResultsContainer() {};

		// CallBackInterface
		virtual void
		callBack(MapInfoEntryPtr mapInfoEntry);
	  virtual void
	  callBack(SchedulingMapPtr schedulingMapPtr){};

		// old CallBackInterface
		void callBack(unsigned int fSlot, simTimeType startTime, simTimeType endTime, wns::scheduler::UserID user,
			      const wns::ldk::CompoundPtr& pdu, float cidColor, unsigned int spatialLayer,
			      wns::service::phy::ofdma::PatternPtr pattern, wns::scheduler::MapInfoEntryPtr burst,
			      const wns::service::phy::phymode::PhyModeInterface& phyMode,
			      bool measureInterference, wns::Power txPowerPerStream,
			      wns::CandI estimatedCandI);

		bool hasBeenScheduled(const wns::ldk::CompoundPtr& pdu);
		/**
		 * Configures whether and how the scheduled pdus should be
		 * plotted. fChannels is the number of frequency channels made
		 * available to the scheduler. name is the name prefix used for
		 * the outputfiles. If fChannels == 0 plotting of next frame is
		 * disabled.
		 */
		void plotNextFrame(int fChannels,
				   int maxSpatialLayers,
				   simTimeType slotDuration,
				   std::string name);

		/**
		 * Finishes the plotting process. Has to be called after all
		 * pdus have been scheduled. Disables future plotting so it will
		 * have to be enabled by plotNextFrame again.
		 */
		void finishPlotting();

		void reset();

	private:
		std::set<wns::ldk::CompoundPtr> scheduledPDUs;
		std::vector<std::ofstream*> plotFiles;
		bool framePlotting;
		int plotChannels;
				   };

} // tests
} // strategy
} // scheduler
} // wns
#endif // NOT defined WNS_SCHEDULER_STRATEGY_TEST_RESULTSCONTAINER_HPP


