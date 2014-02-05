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

#ifndef WNS_LDK_FUN_HPP
#define WNS_LDK_FUN_HPP

#include <WNS/ldk/Compound.hpp>
#include <WNS/osi/PDU.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/TypeInfo.hpp>
#include <WNS/Exception.hpp>

#include <map>
#include <string>


namespace wns {	namespace ldk {
	class ILayer;
	class FunctionalUnit;
	class CommandProxy;
	class CommandPool;
	class CommandTypeSpecifierInterface;
	class LinkHandlerInterface;
}}

namespace wns { namespace ldk { namespace fun {

	class FindFriendException :
		public wns::Exception
	{
	public:
		FindFriendException(const std::string& e) :
			wns::Exception(e)
		{}
        ~FindFriendException() throw() {}
	};

	/**
	 * @brief FUN - Abstract Functional Unit Network Base
	 *
	 * @sa @ref LayerDevelopmentPage
	 *
	 */
	class FUN
	{
	public:
		typedef std::map<std::string, FunctionalUnit*> FunctionalUnitMap;
		virtual ~FUN() {}

		//
		// FUN construction
		//

		/**
		 * @brief Register a FU at the FUN with a given fuName and
		 * distinct commandName.
		 */
		virtual void addFunctionalUnit(const std::string& commandName,
					       const std::string& functionalUnitName,
					       FunctionalUnit* functionalUnit) = 0;

		/**
		 * @brief Register a FU at the FUN with a given name.
		 */
		virtual void addFunctionalUnit(const std::string& name, FunctionalUnit* functionalUnit) = 0;

		/**
		 * @brief Remove an FU with a given name from the FUN.
		 */
		virtual void removeFunctionalUnit(const std::string& name) = 0;

		/**
		 * @brief Connect two FUs in the outgoing and in the incoming flow.
		 */
		virtual void connectFunctionalUnit(
                    const std::string& upperName, const std::string& lowerName,
                    const std::string& srcPort = "SinglePort", const std::string& dstPort = "SinglePort") = 0;

		/**
		 * @brief Connect two FUs in the incoming flow.
		 */
		virtual void upConnectFunctionalUnit(
                    const std::string& upperName, const std::string& lowerName,
                    const std::string& srcPort = "SinglePort", const std::string& dstPort = "SinglePort") = 0;

		/**
		 * @brief Connect two FUs in the outgoing flow.
		 */
		virtual void downConnectFunctionalUnit(
                    const std::string& upperName, const std::string& lowerName,
                    const std::string& srcPort = "SinglePort", const std::string& dstPort = "SinglePort") = 0;

		/**
		 * @brief Replace a single FU by another FU using a PyConfigView for
		 * controling the reconfiguration process.
		 */
            //		virtual void reconfigureFUN(const wns::pyconfig::View& reconfig) = 0;

		//
		// FU access
		//
		/**
		 * @brief Return the FU registered at the FUN with the given name.
		 */
		virtual FunctionalUnit* getFunctionalUnit(const std::string& name) const = 0;

		/**
		 * @brief Ask whether a FU with the given name is registered at the FUN.
		 */
		virtual bool knowsFunctionalUnit(const std::string& name) const = 0;

		/** @name Getter methods */
		//@{
		virtual CommandProxy* getProxy() const = 0;
		virtual ILayer* getLayer() const = 0;

		template <typename LAYERTYPE>
		LAYERTYPE
		getLayer() const
		{
			ILayer* aLayer = getLayer();
			assureType(aLayer, LAYERTYPE);
			// we can't use C-Style downcasts here!
			return dynamic_cast<LAYERTYPE>(aLayer);
		}

		virtual std::string getName() const = 0;
		virtual LinkHandlerInterface* getLinkHandler() const = 0;
		virtual CommandReaderInterface* getCommandReader(const std::string& commandName) const = 0;
		//@}

		/**
		 * @brief Find a FunctionalUnit by name of a given type.
		 *
		 * FUs get instantiated in two steps. First they pass their constructor,
		 * initializing themself using information from the FUN and their
		 * pyconfig::View. In a second step, after all the functional units have
		 * been created, their onFUNCreated method is called. In fiendFriends,
		 * functional units have to resolve all the missing references to
		 * functional units they are about to interact with during operation.
		 *
		 */
		template <typename T>
		T findFriend(const std::string& name) const
		{
			FunctionalUnit* functionalUnit = this->getFunctionalUnit(name);
			T t = dynamic_cast<T>(functionalUnit);

			if (!t)
			{
				std::stringstream ss;

				ss << "FindFriend call failed. Friend FU " << name << " is not of required type "
				   << wns::TypeInfo::create<T>() << ". " << "FU " << name << " is of type "
				   << wns::TypeInfo::create(*functionalUnit) << "." << std::endl;

				throw FindFriendException(ss.str());
			}

			return t;
		} // findFriend

		//
		// forwarding to meet the LawOfDemeter
		//
		void
		calculateSizes(const CommandPool* commandPool,
					   Bit& commandPoolSize, Bit& sduSize,
					   const CommandTypeSpecifierInterface* questioner = NULL) const
		{
			getProxy()->calculateSizes(commandPool, commandPoolSize, sduSize,
									   questioner);
		} // calculateSizes

		CommandPool*
		createCommandPool()
		{
			return getProxy()->createCommandPool(this);
		} // createCommandPool

		//
		// shortcut
		//
		CompoundPtr
		createCompound(osi::PDUPtr sdu = osi::PDUPtr())
		{
			return CompoundPtr(new Compound(createCommandPool(), sdu));
		} // createCompound
	};

}}}



#endif // NOT defined WNS_LDK_FUN_HPP


