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

#ifndef DLL_LAYER2_HPP
#define DLL_LAYER2_HPP

#include <DLL/services/control/Association.hpp>
#include <DLL/UpperConvergence.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/Enum.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/container/Registry.hpp>

namespace dll {

	class StationManager;
	class UpperConvergence;

        class ILayer2:
            virtual public wns::ldk::ILayer
        {
        public:
            typedef long int StationIDType;

            /** @brief Corresponding with the StationTypes ENUM */
            typedef long int StationType;

            typedef std::vector<dll::services::control::AssociationInfo*> AssociationInfoContainer;

            typedef wns::container::Registry< std::string, AssociationInfoContainer > AssociationInfoRegistry;

            static StationIDType   invalidStationID;
            static StationIDType broadcastStationID;

            /** @brief Access to the internal ID */
            virtual StationIDType
            getID() const = 0;

            /** @brief Access to the DLL Address */
            virtual wns::service::dll::UnicastAddress
            getDLLAddress() const = 0;

            /** @brief Access to the StationManager */
            virtual StationManager*
            getStationManager() = 0;

            /** @brief Access the stationType */
            virtual StationType
            getStationType() const = 0;

            /** @brief Access the internal FUN */
            virtual wns::ldk::fun::FUN*
            getFUN() const = 0;

           /**
            * @brief Access the Station Name. This method is deprecated - it is
            * recommended to use the  node's getName() instead
            */
            virtual std::string
            getName() const = 0;

           /** @brief Deliver pointer to the Node this component belongs to */
            virtual wns::node::Interface*
            getNode() const = 0;

            virtual void
            addAssociationInfoProvider( const std::string& id, dll::services::control::AssociationInfo* provider ) = 0;

            virtual const AssociationInfoContainer&
            getAssociationInfoProvider(const std::string& id) const = 0;

            virtual void
            updateContext(const std::string& key, int value) = 0;

            virtual void
            setContext(const std::string& key, int value) = 0;
        };

	/** @brief The DLL Component Base Class.
	 *
	 * Inherited by the WinProSt and WiMAC Data Link Layers. This class is
	 * mainly the container for the Functional Unit Network (FUN).
	 */
	class Layer2 :
            public ILayer2,
            public wns::ldk::Layer,
            public wns::node::component::Component
	{
	public:

		/** @brief Constructor */
		Layer2(wns::node::Interface*, const wns::pyconfig::View&, StationManager* sm);
		/** @brief Destructor */
		virtual ~Layer2();

                /** @brief Access to the internal ID */
		StationIDType getID() const;

		/** @brief Access to the DLL Address */
		wns::service::dll::UnicastAddress getDLLAddress() const;

		/** @brief Access to the StationManager */
		virtual StationManager* getStationManager();

		/** @brief Access the stationType */
		StationType getStationType() const;

		/** @brief Access the internal FUN */
		wns::ldk::fun::FUN*
		getFUN() const
		{
			return fun;
		}

		/**
		 * @brief Access the Station Name. This method is deprecated - it is
		 * recommended to use the  node's getName() instead
		 */
		virtual std::string getName() const;

		/** @brief Deliver pointer to the Node this component belongs to */
		virtual wns::node::Interface* getNode() const { return wns::node::component::Component::getNode(); };

		void
		addAssociationInfoProvider( const std::string& id, dll::services::control::AssociationInfo* provider );

		const AssociationInfoContainer&
		getAssociationInfoProvider(const std::string& id) const;

		void
		updateContext(const std::string& key, int value)
			{
				agent.updateContext(key, value);
			}

		void
		setContext(const std::string& key, int value)
			{
				agent.setContext(key, value);
			}

                virtual void onShutdown();

	protected:
		Layer2(const Layer2&);	// disallow copy constructor
		Layer2& operator=(const Layer2&); // disallow assignment

		virtual void
		doStartup();
		dll::UpperConvergence* dllUpperConvergence;

		wns::ldk::fun::Main* fun;
		wns::pyconfig::View config;
		wns::logger::Logger logger;

		StationManager* stationManager;

		/** @brief Internal ID of this Station */
		StationIDType stationID;
		/** @brief Identifies this station as AP,FRS,UT,... */
		StationType type;

		/** @brief This station's MAC Address */
		wns::service::dll::UnicastAddress address;
		/**
		 * @brief Distance measure from the associated BS to be used as probe instance id.
		 *
		 * For a AP, ring is always 0
		 * For a FRS, ring is always the ring of the next station in uplink + 2
		 * For a UT, ring is always the ring of the RAP + 1
		 *
		 * AP(1) <--> FRS(3) <--> FRS(5) <--> UT(6)
		 *   ^          ^           ^
		 *   |          |           |
		 *   v          v           v
		 *  UT(2)      UT(4)       UT(6)
		 *
		 * @todo should use Enum.
		 */
		unsigned long int ring;

		/** @brief */
		AssociationInfoRegistry air;

		/**
		 * @brief ContextAgent concentrates context information from Members
		 * of the Layer2 or its internal FUN
		 *
		 * The said classes COULD theoretically register their own ContextProviders,
		 * but for the sake of simplicity I chose to gather context information
		 * here.
		 */
		class ContextAgent :
			public wns::probe::bus::ContextProvider
		{
			/**
			 * @brief storage for context
			 */
			wns::container::Registry<std::string, int> myContextInfo;
			typedef wns::container::Registry<std::string, int>::const_iterator ContextIterator;
		public:
			void
			updateContext(const std::string& key, int value)
				{
					myContextInfo.update(key, value);
				}

			void
			setContext(const std::string& key, int value)
				{
					myContextInfo.insert(key, value);
				}
			bool
			knows(const std::string& key)
				{
					return myContextInfo.knows(key);
				}
        private:
            /**
             * @brief Implementation of ContextProvider Interface
             */
            virtual void
            doVisit(wns::probe::bus::IContext&) const;

		};

		ContextAgent agent;
	}; // Layer2
}

#endif // NOT defined DLL_LAYER2_HPP


