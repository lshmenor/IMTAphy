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


#include <WNS/scheduler/grouper/SpatialGrouper.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

using namespace wns::scheduler::grouper;

wns::scheduler::GroupingPtr
GroupingProviderInterface::getTxGroupingPtr(UserSet activeUsers, int maxBeams)
{
	return GroupingPtr(new Grouping(getTxGrouping(activeUsers,maxBeams)));
}

wns::scheduler::GroupingPtr
GroupingProviderInterface::getRxGroupingPtr(UserSet activeUsers, int maxBeams)
{
	return GroupingPtr(new Grouping(getRxGrouping(activeUsers,maxBeams)));
}

SpatialGrouper::SpatialGrouper(const wns::pyconfig::View& config)
	: x_friendliness(wns::Power::from_dBm(config.get<double>("friendliness_dBm"))),
	  txPower(wns::Power::from_mW(0)), // to be initialised once the
	  eirpLimited(false),              // ofdmaProvider is known
	  MonteCarloSim(config.get<bool>("MonteCarloSim")),
	  beamforming(config.get<bool>("beamforming")),
	  uplink(config.get<bool>("uplink")),
	  logger(config.get("logger"))
{
	// initialize them to NULL, so assure can work
	colleagues.registry = NULL;
	friends.ofdmaProvider = NULL;
}

void
SpatialGrouper::setColleagues(RegistryProxyInterface* _registry)
{
	colleagues.registry = _registry;
	colleagues.phyModeMapper = colleagues.registry->getPhyModeMapper();
        txPower = colleagues.registry->getPowerCapabilities().nominalPerSubband;
	if (!MonteCarloSim){
		if (!uplink){
			groupingGainProbeBus = wns::probe::bus::ContextCollectorPtr(
				new wns::probe::bus::ContextCollector(
					wns::probe::bus::ContextProviderCollection(
						&colleagues.registry->getMyUserID().getContextProviderCollection()),
					"groupingGain"));
		}
		else{
			groupingGainProbeBus = wns::probe::bus::ContextCollectorPtr(
				new wns::probe::bus::ContextCollector(
					wns::probe::bus::ContextProviderCollection(
						&colleagues.registry->getMyUserID().getContextProviderCollection()),
					"groupingGainUL"));
		}
	}
}

void
SpatialGrouper::setFriends(wns::service::phy::ofdma::BFInterface* _ofdmaProvider)
{
	friends.ofdmaProvider = _ofdmaProvider;
	//txPower = friends.ofdmaProvider->getMaxPowerPerSubband();
	eirpLimited = friends.ofdmaProvider->isEIRPLimited();
}

