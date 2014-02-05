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

#ifndef WNS_LDK_LAYER_HPP
#define WNS_LDK_LAYER_HPP

#include <WNS/node/Interface.hpp>
#include <WNS/node/component/Interface.hpp>
#include <WNS/ldk/ControlServiceInterface.hpp>
#include <WNS/ldk/ManagementServiceInterface.hpp>

#include <string>

namespace wns { namespace ldk {

	/**
	 * @page LayerDevelopmentPage How to implement a Layer
	 *
	 * @section introduction Introduction
	 *
	 * Layer development is often the fundamental step of developing a wns
	 * module. Layers in the wns correspond to ISO OSI Layers and are divided
	 * into functionalUnits. There is a simple machanism to connect functionalUnits
	 * together. These functionalUnits connected in a specific way and assembled in
	 * the Layer class represent the layer of the wns module.
	 *
	 * @section usage Usage
	 *
	 * To define a wns layer it is necessary to derive a class from
	 * wns::ldk::Layer and implement three virtual methods.
	 *
	 * @include Layer:InterfaceDefinition.example
	 *
	 * The two methods doSendData and doOnData are called whenever the layer is
	 * asked to process outgoing compounds (sendData) and incoming compounds (onData).
	 * Typically the function of a layer is quite complex hence a split of the
	 * layer is necessary. The wns library provides a mechanism to easily
	 * build up a layer split into functionalUnits. For this several functionalUnits are
	 * already defined (FunctionalUnit reference).
	 *
	 * If a custom functionalUnit is defined there are minimum requirements to be met:
	 *
	 * The functionalUnit has to be derived from
	 * \li wns::ldk::FunctionalUnit
	 * \li wns::ldk::CommandTypeSpecifier<>
	 * \li wns::ldk::HasReceptor<>
	 * \li wns::ldk::HasConnector<>
	 * \li wns::ldk::HasDeliverer<>
	 *
	 * Further the methods CompoundHandlerInterface::isAccepting(),
	 * CompoundHandlerInterface::sendData(), CompoundHandlerInterface::onData() and
	 * CompoundHandlerInterface::wakeup() need to be implemented which are derived
	 * from the FunctionalUnit which is again derived from the
	 * CompoundHandlerInterface. The class declaration may look like
	 * this:
	 *
	 * @include Layer:FunctionalUnitDeclaration.example
	 *
	 * For further details how to implement a functionalUnit see @ref FunctionalUnitPage.
	 *
	 * Deriving from the FunctionalUnit the method FunctionalUnit::connect() gives access to
	 * the connection mechanism of functionalUnits. Higher functionalUnits may be connected
	 * with lower level functionalUnits. The internal structure of the layer is set up
	 * in the constructor of the layer. Here the functionalUnits are created and
	 * connected.
	 *
	 * @include Layer:ConstructorDefinition.example
	 *
	 * In this example there are four functionalUnits declared. Two stubs are set up
	 * as upper and lower endpoint of the layer. A buffer is part of the
	 * functionalUnit chain to store incoming packets. Note that the buffer needs to be
	 * initialized with a pyconfig::View which tells the buffer the bufferlength.
	 * Last but not least the ExampleFunctionalUnit is part of the layer.
	 *
	 * All four functionalUnits need to be connected. This is done by calling the
	 * connect() method of the highest functionalUnit. The next lower functionalUnit is
	 * given as parameter to the connect() method which returns a pointer of the
	 * lower functionalUnit. This makes connecting functionalUnits easy by calling connect()
	 * from the highest functionalUnit to the lowest.
	 *
	 * Finally the Layer has to direct doSendData() calls to the highest functionalUnit
	 * and doOnData() calls to the lowest functionalUnit in the functionalUnit chain.
	 *
	 * @include Layer:sendDataAndonDataDefinition.example
	 *
	 * Now the layer is ready to use.
	 */

    class ILayer :
        public virtual wns::node::component::Interface
        {
        public:
            virtual std::string
            getNodeName() const = 0;

            virtual ControlServiceRegistry*
            getCSR() = 0;

            virtual void
            addControlService(const std::string& name, ControlServiceInterface* csi) = 0;

            virtual ManagementServiceRegistry*
            getMSR() = 0;

            virtual void
            addManagementService(const std::string& name, ManagementServiceInterface* msi) = 0;

            virtual ManagementServiceInterface*
            findManagementService(std::string) const = 0;

            virtual ControlServiceInterface*
            findControlServiceInterface(std::string) const = 0;

            template <typename MANAGEMENTSERVICE>
            MANAGEMENTSERVICE*
            getManagementService(const std::string& name) const
            {
                ManagementServiceInterface* msi = findManagementService(name);
                assureType(msi, MANAGEMENTSERVICE*);
                // we can't use C-Style downcasts here!
                return dynamic_cast<MANAGEMENTSERVICE*>(msi);
            }

            template <typename CONTROLSERVICE>
            CONTROLSERVICE*
            getControlService(const std::string& name) const
            {
                ControlServiceInterface* csi = findControlServiceInterface(name);
                assureType(csi, CONTROLSERVICE*);
                // we can't use C-Style downcasts here!
                return dynamic_cast<CONTROLSERVICE*>(csi);
            }
        };

	/**
	 * @brief The Layer is the base class of all Layers implemented into the
	 * wns.
	 *
	 * @sa @ref LayerDevelopmentPage
	 *
	 */
	class Layer :
                virtual public ILayer,
		public virtual wns::node::component::Interface
	{
	public:
		Layer() : managementServices( this ), controlServices( this ) {}

		virtual std::string
		getNodeName() const
		{
			return getNode()->getName();
		}

		ControlServiceRegistry*
		getCSR()
		{
			return &controlServices;
		}

		void
		addControlService(const std::string& name, ControlServiceInterface* csi)
		{
			controlServices.insert(name, csi);
		}

		ManagementServiceRegistry*
		getMSR()
		{
			return &managementServices;
		}

		void
		addManagementService(const std::string& name, ManagementServiceInterface* msi)
		{
			managementServices.insert(name, msi);
		}

                virtual ManagementServiceInterface*
                findManagementService(std::string name) const
                {
                    return managementServices.find(name);
                }

                virtual ControlServiceInterface*
                findControlServiceInterface(std::string name) const
                {
                    return controlServices.find(name);
                }

	private:
		ManagementServiceRegistry managementServices;
		ControlServiceRegistry controlServices;

	};
}}


#endif // NOT defined WNS_LDK_LAYER_HPP


