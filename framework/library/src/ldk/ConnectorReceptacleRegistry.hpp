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

#ifndef WNS_LDK_CONNECTORRECEPTACLEREGISTRY_HPP
#define WNS_LDK_CONNECTORRECEPTACLEREGISTRY_HPP

#include <WNS/ldk/IConnectorReceptacle.hpp>

#include <WNS/container/Registry.hpp>

#include <string>

namespace wns { namespace ldk {

        class ConnectorReceptacleRegistry
        {
        public:
            ConnectorReceptacleRegistry()
                : theRegistry_()
            {}

            ConnectorReceptacleRegistry(const ConnectorReceptacleRegistry&)
                : theRegistry_()
            {}

            virtual
            ~ConnectorReceptacleRegistry()
            {}

            IConnectorReceptacle*
            getFromConnectorReceptacleRegistry(std::string portname) const
            {
                return theRegistry_.find(portname);
            }

            std::list<std::string>
            getKeysFromConnectorReceptacleRegistry() const
            {
                return theRegistry_.keys();
            }

        protected:
            void
            updateConnectorReceptacleRegistry(const std::string portname, IConnectorReceptacle* cr)
            {
                theRegistry_.update(portname, cr);
            }

            void
            addToConnectorReceptacleRegistry(std::string portname, IConnectorReceptacle* cr)
            {
                theRegistry_.insert(portname, cr);
            }

        private:
            container::Registry<std::string, IConnectorReceptacle*> theRegistry_;
        };
    } // ldk
} // wns

#endif // WNS_LDK_CONNECTORRECEPTACLEREGISTRY_HPP
