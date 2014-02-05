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

#ifndef DLL_SERVICES_MANAGEMENT_INTERFERENCECACHE_HPP
#define DLL_SERVICES_MANAGEMENT_INTERFERENCECACHE_HPP

#include <DLL/services/management/ESMFunc.hpp>

#include <map>
#include <set>
#include <WNS/ldk/ldk.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/ldk/ManagementServiceInterface.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/ldk/PyConfigCreator.hpp>
#include <WNS/service/phy/power/PowerMeasurement.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>

namespace wns
{
	namespace ldk
    {
		class ManagementServiceRegistry;
	}
}

namespace dll { namespace services { namespace management {


	/// The interference cache provides averaged carrier and interference values.
	class InterferenceCache :
	public wns::ldk::ManagementService
	{
	    public:
	    	/**
	    	 * @brief Strategy that specifies the behaviour if the requested
	    	 * value is not found.
	    	 *
	    	 * Whenever a value of the InterferenceCache is requested and
	    	 * the value is not available, the NotFoundStrategy is called to
	    	 * handle the request.
	    	 */

		class NotFoundStrategy
		{
	    	public:
    		virtual ~NotFoundStrategy()
            {
            }

	   		/**
	   		 * @brief Handles unavailable average carrier values.
	    	 */
	    	virtual wns::Power notFoundAverageCarrier() = 0;

			/**
			 * @brief Handles unavailable average interference values.
			 */
			virtual wns::Power notFoundAverageInterference() = 0;

			/**
			 * @brief Handles unavailable average pathloss values.
			 */
			virtual wns::Ratio notFoundAveragePathloss() = 0;

			/**
			 * @brief Handles unavailable deviation carrier values.
			 */
			virtual wns::Power notFoundDeviationCarrier() = 0;

			/**
			 * @brief Handles unavailable deviation interference values.
			 */
			virtual wns::Power notFoundDeviationInterference() = 0;
		};

		typedef wns::ldk::PyConfigCreator<NotFoundStrategy> NotFoundStrategyCreator;
		typedef wns::StaticFactory<NotFoundStrategyCreator> NotFoundStrategyFactory;


		/**
		 * @brief A NotFoundStrategy that returns constant values.
		 */
		class ConstantValue :
		public NotFoundStrategy
		{
		    public:

		    ConstantValue( const wns::pyconfig::View& );

		   	wns::Power notFoundAverageCarrier()
		   	{
		   		return averageCarrier;
	    	}

	    	wns::Power notFoundAverageInterference()
	    	{
	    		return averageInterference;
	    	}

			wns::Ratio notFoundAveragePathloss()
			{
				return averagePathloss;
			}

			wns::Power notFoundDeviationCarrier()
			{
				return deviationCarrier;
			}

			wns::Power notFoundDeviationInterference()
			{
				return deviationInterference;
			}

		    private:
		    	wns::Power averageCarrier;
		       	wns::Power averageInterference;
	    		wns::Power deviationCarrier;
	    		wns::Power deviationInterference;
	    		wns::Ratio averagePathloss;
		};

		/**
		 * @brief A NotFoundStrategy that throws an exception.
		 */
		class Complain :
		public virtual NotFoundStrategy
		{
    		public:
    		Complain(const wns::pyconfig::View&)
   			{
            }

			wns::Power notFoundAverageCarrier()
			{
				throw wns::Exception("Average Carrier value not found");
				return wns::Power();
			}

			wns::Power notFoundAverageInterference()
			{
				throw wns::Exception("Average Interference value not found");
				return wns::Power();
			}

			wns::Ratio notFoundAveragePathloss()
			{
				throw wns::Exception("Average Pathloss value not found");
				return wns::Ratio();
			}

			wns::Power notFoundDeviationCarrier()
			{
				throw wns::Exception("Carrier value deviation not found");
				return wns::Power();
			}

			wns::Power notFoundDeviationInterference()
			{
				throw wns::Exception("Interference calue deviation not found");
				return wns::Power();
			}
		};


		InterferenceCache( wns::ldk::ManagementServiceRegistry*, const wns::pyconfig::View& config );

		virtual ~InterferenceCache()
        {
        }

        virtual void
        onMSRCreated();

		enum ValueOrigin
        {
			Local,
			Remote
		};

		/**
		 * @brief Store the received power measurements of node in the InterferenceCache.
		 *
		 * The received power values of node is stored in the
		 * InterferenceCache. The ValueOrigin tells whether the local node
		 * or a remote node writes into the cache.
		 *
		 * @sa storeCarrier()
		 * @sa storeInterference()
		 * @sa storePathloss()
		 */
		void storeMeasurements( wns::node::Interface* node, 
            wns::service::phy::power::PowerMeasurementPtr rxPowerMeasurement, ValueOrigin origin, 
            int subBand = 0);

		/**
		 * @brief Store the received carrier power of node in the InterferenceCache.
		 *
		 * The received carrier power of node is stored in the
		 * InterferenceCache. The ValueOrigin tells whether the local node
		 * or a remote node writes into the cache.
		 *
		 * Obsolete for new code. But still used in unitTests. Therefore not removed.
		 *
		 * @sa storeMeasurements()
		 */
		void storeCarrier( wns::node::Interface* node, const wns::Power& carrier, ValueOrigin origin, int subBand = 0);


		/**
		 * @brief Store the received interference power of node in the InterferenceCache.
		 *
		 * The received interference power of node is stored in the
		 * InterferenceCache. The ValueOrigin tells whether the local node
		 * or a remote node writes into the cache.
		 *
		 * Obsolete for new code. But still used in unitTests. Therefore not removed.
		 *
		 * @sa storeMeasurements()
		 */
		void storeInterference( wns::node::Interface* node, 
            const wns::Power& interference, ValueOrigin origin, int subBand = 0, int timeSlot = ANYTIME);

		/**
		 * @brief Store the estimated pathloss (including antenna gains) to a node in the InterferenceCache.
		 *
		 * Obsolete for new code. But still used in unitTests. Therefore not removed.
		 * @sa storeMeasurements()
		 */
		void storePathloss( wns::node::Interface* node, const wns::Ratio& pathloss, ValueOrigin origin, int subBand = 0);

		/// Returns the average carrier power measured so far.
		wns::Power getAveragedCarrier( wns::node::Interface* node, int subBand = 0, int timeSlot = ANYTIME ) const;

		/// Returns the average interference measured so far.
		wns::Power getAveragedInterference( wns::node::Interface* node, int subBand = 0, int timeSlot = ANYTIME ) const;

		/// Returns the average pathloss measured so far.
		wns::Ratio getAveragedPathloss( wns::node::Interface* node, int subBand = 0, int timeSlot = ANYTIME ) const;

		/// Returns the average carrier power measured so far on all subchannels the node transmitted on.
		wns::Power getPerSCAveragedCarrier( wns::node::Interface* node) const;

		/// Returns the average interference measured so far on all subchannels the node transmitted on.
		wns::Power getPerSCAveragedInterference( wns::node::Interface* node) const;

		/// Returns the average pathloss measured so far on all subchannels the node transmitted on.
		wns::Ratio getPerSCAveragedPathloss( wns::node::Interface* node) const;
        /**
        * @brief Get the average uplink interference pathloss from this node to all but this BS
        */
        wns::Ratio 
        getAverageEmittedInterferencePathloss(wns::node::Interface* node) const;

        /**
        * @brief Calculate Effective SINR using an Effective SINR Mapping (ESM).
        * if no interference map is provided values from the local cache are used
        */
        wns::Ratio
        getEffectiveSINR(wns::node::Interface* node, 
            const std::set<unsigned int>& subchannels,
            const wns::Power& txPower,
            const std::map<unsigned int, wns::Power>&);
        
        /**
        * @brief Updates the evaluated subchannels of a user.
        */    
        void 
        updateUserSubchannels (const wns::node::Interface* node, const std::set<int>& channels);

		/// Returns the deviation of the measured carrier power.
		wns::Power getCarrierDeviation( wns::node::Interface*, int subBand = 0 ) const;

		/// Returns the deviation of the measured interference power.
		wns::Power getInterferenceDeviation( wns::node::Interface*, int subBand = 0 ) const;


	private:
		/**
		 * @brief A key for the internal representation of the cache.
		 */
		class InterferenceCacheKey :
		public std::binary_function<const InterferenceCacheKey, const InterferenceCacheKey, bool>
		{
		    public:
			InterferenceCacheKey(wns::node::Interface* node, int subBand, int timeSlot = ANYTIME) :
			node_(node),
			subBand_(subBand),
            timeSlot_(timeSlot)
			{
            }

			bool operator<(const InterferenceCacheKey& rhs) const
			{
				if ( node_->getNodeID() == rhs.node_->getNodeID() )
                {
                    if(subBand_ == rhs.subBand_)
                    return timeSlot_ < rhs.timeSlot_;
					return subBand_ < rhs.subBand_;
                }
				return node_->getNodeID() < rhs.node_->getNodeID();
			}
		private:
			wns::node::Interface* node_;
			int subBand_;
            int timeSlot_;
		};

		typedef std::map<InterferenceCacheKey, wns::Power> Node2Power;
		typedef std::map<InterferenceCacheKey, double> Node2Double;

		Node2Power node2CarrierAverage_;
		Node2Power node2InterferenceAverage_;
		Node2Double node2CarrierSqExp_;
		Node2Double node2InterferenceSqExp_;
		Node2Double node2pathloss;
		double alphaLocal_;
		double alphaRemote_;
        mutable bool initialized_;
        mutable std::list<InterferenceCache*> remoteBSCaches_;
        std::map<int, std::set<int> > userSubbands_;
        std::string serviceName_;

		wns::logger::Logger logger;

		std::auto_ptr<NotFoundStrategy> notFoundStrategy;
        IESMFunc* esmFunc_;
	};
}}}
#endif // NOT DEFINED DLL_SERVICES_MANAGEMENT_INTERFERENCECACHE_HPP

