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

#ifndef DLL_TESTS_LAYERSTUB_HPP
#define DLL_TESTS_LAYERSTUB_HPP

#include <DLL/Layer2.hpp>
#include <WNS/node/tests/Stub.hpp>
#include <WNS/pyconfig/View.hpp>

namespace dll { namespace tests {

    /**
     * @brief Simple dll::Layer Stub for unit tests
     * @author Ismet Aktas <isi@comnets.rwth-aachen.de>
     *
     * This LayerStub will automatically create a node::tests::Stub
     */
    class LayerStub :
        public dll::ILayer2
    {
    public:
        LayerStub();

        virtual
        ~LayerStub();

        virtual std::string getName() const;

        virtual wns::node::Interface* getNode() const;

        virtual void onNodeCreated();

        virtual void onWorldCreated();

        virtual void onShutdown();

        virtual std::string getNodeName() const;

                virtual wns::ldk::ControlServiceRegistry*
                getCSR() 
                {
                    assure(false, "Not implemented");
                }

                virtual void
                addControlService(const std::string& name, wns::ldk::ControlServiceInterface* csi)
                {
                }

                virtual wns::ldk::ManagementServiceRegistry*
                getMSR() 
                {
                    assure(false, "Not implemented");
                }

                virtual void
                addManagementService(const std::string& name, wns::ldk::ManagementServiceInterface* msi)
                {
                }

                virtual wns::ldk::ManagementServiceInterface*
            findManagementService(std::string) const 
                {
                    assure(false, "Not implemented");
                }

                virtual wns::ldk::ControlServiceInterface*
            findControlServiceInterface(std::string) const
                {
                    assure(false, "Not implemented");
                }

                /** @brief Access to the internal ID */
                virtual StationIDType
                getID() const {return ILayer2::invalidStationID;}

                /** @brief Access to the DLL Address */
                virtual wns::service::dll::UnicastAddress
                getDLLAddress() const { return wns::service::dll::UnicastAddress(); }

                /** @brief Access to the StationManager */
                virtual StationManager*
                getStationManager() { assure(false, "Not implemented");}

                /** @brief Access the stationType */
                virtual StationType
                getStationType() const { assure(false, "Not implemented");}

                /** @brief Access the internal FUN */
                virtual wns::ldk::fun::FUN*
            getFUN() const;

                virtual void
                addAssociationInfoProvider( const std::string& id, dll::services::control::AssociationInfo* provider )
                {
                    assure(false, "Not implemented");
                }

                virtual const AssociationInfoContainer&
            getAssociationInfoProvider(const std::string& id) const;

                virtual void
                updateContext(const std::string& key, int value)
                {
                    assure(false, "Not implemented");
                }

                virtual void
                setContext(const std::string& key, int value)   
                {
                    assure(false, "Not implemented");
                }
    private:
        virtual void
        doStartup();

        wns::node::Interface* node_;

        wns::ldk::fun::FUN* fun_;

        AssociationInfoContainer aic_; 
    };
} // tests
} // dll

#endif // NOT defined WNS_LDK_LAYER_HPP