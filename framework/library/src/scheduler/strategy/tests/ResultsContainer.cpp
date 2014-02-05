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

#include <WNS/scheduler/strategy/tests/ResultsContainer.hpp>

using namespace wns::scheduler::strategy::tests;

ResultsContainer::ResultsContainer()
	: framePlotting(false) {}

void
ResultsContainer::callBack(MapInfoEntryPtr mapInfoEntry)
{
  simTimeType startTime = mapInfoEntry->start;
  simTimeType endTime = mapInfoEntry->end;
  int fSlot = mapInfoEntry->subBand;
  int spatialLayer = mapInfoEntry->spatialLayer;
  wns::scheduler::UserID user = mapInfoEntry->user;
  int userID = user.getNodeID();
  std::list<wns::ldk::CompoundPtr> compounds = mapInfoEntry->compounds;
	if (framePlotting) {
		*plotFiles[fSlot] << startTime << "\t" << endTime << "\t"
				  << float(spatialLayer) << "\t" << float(userID)/100.0 << "\t\"";
		*plotFiles[fSlot] << userID << "\"\n";
	}
	// only take first one (assume that it only contains one pdu)
	//assure(compounds.size()==1,"compounds.size()="<<compounds.size()<<" must be one here");
	//scheduledPDUs.insert(compounds.front());
	for (wns::scheduler::CompoundList::iterator iter=compounds.begin(); iter!=compounds.end(); ++iter)
	{
	  wns::ldk::CompoundPtr myPDU = *iter;
	  scheduledPDUs.insert(myPDU);
	}
}

void ResultsContainer::callBack(unsigned int fSlot,
				simTimeType startTime,
				simTimeType endTime,
				wns::scheduler::UserID /* user */,
				const wns::ldk::CompoundPtr& pdu,
				float cidColor,
				unsigned int spatialLayer,
				wns::service::phy::ofdma::PatternPtr /* pattern */,
				wns::scheduler::MapInfoEntryPtr /* burst */,
				const wns::service::phy::phymode::PhyModeInterface& /*phyMode*/,
				bool  measureInterference,
				wns::Power /*txPowerPerStream*/,
				wns::CandI /*estimatedCandI*/)
{
	if (framePlotting) {
		*plotFiles[fSlot] << startTime << "\t" << endTime << "\t"
				  << float(spatialLayer) << "\t" << cidColor << "\t\"";
		if (measureInterference)
			*plotFiles[fSlot] << int(nearbyint(cidColor*100.0)) << "\"\n";
		else
			*plotFiles[fSlot] << "\"\n";
		// FIXME: provide a better user identification (last field)
	}
	scheduledPDUs.insert(pdu);
}


bool ResultsContainer::hasBeenScheduled(const wns::ldk::CompoundPtr& pdu)
{
	return (scheduledPDUs.find(pdu) != scheduledPDUs.end());;
}

void ResultsContainer::reset()
{
	scheduledPDUs.clear();
}

void ResultsContainer::plotNextFrame(int fChannels,
				     int maxSpatialLayers,
				     simTimeType slotDuration,
				     std::string name)
{
	framePlotting = true;
	plotChannels = fChannels;

	std::ofstream conf;
	std::stringstream ssConf;
	std::stringstream ssPlot;

	ssConf << "output/" << name << ".conf";
	conf.open (ssConf.str().c_str());

	conf << "#use bin/plotFrame.py from the WNS root directory to plot this:\n"
	     << "# \"../../../bin/plotFrame.py "
	     << name << "\"\n";
	conf << "[main]\n"
	     << "FreqChannels=" << plotChannels << "\n"
	     << "Beams=" << maxSpatialLayers << "\n"
	     << "StartTime=0.0\n"
	     << "EndTime=" << slotDuration << "\n";
	conf.close();

	plotFiles.clear();
	for (int i = 0; i < plotChannels; ++i) {
		plotFiles.push_back(new std::ofstream());
		ssPlot.str("");
		ssPlot << "output/" << name << ".plot." << i;
		plotFiles[i]->open(ssPlot.str().c_str());
		// format flags for the timestamps: always print 9 digits for float,
		plotFiles[i]->flags( std::ios_base::fixed );
		plotFiles[i]->precision(9);
	}
}

void ResultsContainer::finishPlotting()
{
	assure(framePlotting, "Only possible after call to plotNextFrame");

	framePlotting = false;

	for (int i = 0; i < plotChannels; ++i) {
		plotFiles[i]->close();
		delete plotFiles[i];
	}
}


