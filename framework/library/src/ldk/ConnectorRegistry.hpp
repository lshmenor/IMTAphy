/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2008                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 1, D-52074 Aachen, Germany                                  *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef WNS_LDK_CONNECTORREGISTRY_HPP
#define WNS_LDK_CONNECTORREGISTRY_HPP

#include <WNS/ldk/Connector.hpp>

#include <WNS/container/Registry.hpp>

#include <string>

namespace wns { namespace ldk {

        class ConnectorRegistry
        {
        public:
            ConnectorRegistry()
                : theRegistry_()
            {}

            ConnectorRegistry(const ConnectorRegistry&)
                : theRegistry_()
            {}

            virtual
            ~ConnectorRegistry()
            {}

            Connector*
            getFromConnectorRegistry(std::string portname) const
            {
                return theRegistry_.find(portname);
            }

            std::list<std::string>
            getKeysFromConnectorRegistry() const
            {
                return theRegistry_.keys();
            }

            void
            addToConnectorRegistry(std::string portname, Connector* cr)
            {
                theRegistry_.insert(portname, cr);
            }

        protected:
            void
            updateConnectorRegistry(const std::string portname, Connector* cr)
            {
                theRegistry_.update(portname, cr);
            }

        private:
            container::Registry<std::string, Connector*> theRegistry_;
        };
    } // ldk
} // wns

#endif // WNS_LDK_CONNECTORREGISTRY_HPP
