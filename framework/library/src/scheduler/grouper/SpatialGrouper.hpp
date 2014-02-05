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

#ifndef WNS_SCHEDULER_GROUPER_SPATIALGROUPER_HPP
#define WNS_SCHEDULER_GROUPER_SPATIALGROUPER_HPP

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/service/phy/ofdma/Pattern.hpp>
#include <WNS/service/phy/ofdma/DataTransmission.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/node/Interface.hpp>

namespace wns { namespace scheduler { namespace grouper {

	/// GroupingProviderInterface defines the functions to be provided by every
	/// spatial grouper.

	class GroupingProviderInterface {
	public:
		virtual ~GroupingProviderInterface() {};
		virtual Grouping getTxGrouping(UserSet activeUsers, int maxBeams) = 0;
		virtual Grouping getRxGrouping(UserSet activeUsers, int maxBeams) = 0;
		virtual wns::scheduler::GroupingPtr getTxGroupingPtr(UserSet activeUsers, int maxBeams);
		virtual wns::scheduler::GroupingPtr getRxGroupingPtr(UserSet activeUsers, int maxBeams);

		virtual void setColleagues(RegistryProxyInterface*  registry) = 0;
		virtual void setFriends(wns::service::phy::ofdma::BFInterface*) = 0;
	};

	/// Base class for all spatial groupers that provides some basic
	/// functionality used by every grouper
	class SpatialGrouper
		: public GroupingProviderInterface {
	public:

		SpatialGrouper(const wns::pyconfig::View& config);
		virtual ~SpatialGrouper(){}

		virtual void setColleagues(RegistryProxyInterface* _registry);
		virtual void setFriends(wns::service::phy::ofdma::BFInterface* _ofdmaProvider);

	protected:
        template<typename T>
        std::map<UserID, T>
        convertMap(std::map<wns::node::Interface*, T> r)
        {
            typename std::map<wns::node::Interface*, T>::iterator it;
            typename std::map<UserID, T> converted;
            for (it=r.begin(); it!=r.end();++it)
            {
                converted.insert(std::pair<UserID, T>(UserID(it->first), it->second) );
            }
            return converted;
        }

        int setupProbe(float minX, float maxX, std::string name, int bins);

		struct {
			RegistryProxyInterface* registry;
			const wns::service::phy::phymode::PhyModeMapperInterface* phyModeMapper;
		} colleagues;

		struct {
			wns::service::phy::ofdma::BFInterface* ofdmaProvider;
		} friends;

		wns::Power x_friendliness;
		wns::Power txPower;
		bool eirpLimited;
		bool MonteCarloSim;
		bool beamforming;
		bool uplink;

		wns::probe::bus::ContextCollectorPtr groupingGainProbeBus;

		wns::logger::Logger logger;
	};

	// StaticFactory typedefs to allow creation of Groupers adhering to the GroupingProviderInterface
	typedef wns::PyConfigViewCreator<GroupingProviderInterface, GroupingProviderInterface> SpatialGrouperCreator;
	typedef wns::StaticFactory<SpatialGrouperCreator> SpatialGrouperFactory;

}}} // namespace wns::scheduler
#endif // WNS_SCHEDULER_GROUPER_SPATIALGROUPER_HPP


