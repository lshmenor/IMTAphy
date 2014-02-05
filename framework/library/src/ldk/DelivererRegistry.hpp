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

#ifndef WNS_LDK_DELIVERERREGISTRY_HPP
#define WNS_LDK_DELIVERERREGISTRY_HPP

#include <WNS/ldk/Deliverer.hpp>

#include <WNS/container/Registry.hpp>

#include <string>

namespace wns { namespace ldk {

        class DelivererRegistry
        {
        public:
            DelivererRegistry()
                : theRegistry_()
            {}

            DelivererRegistry(const DelivererRegistry&)
                : theRegistry_()
            {}

            virtual
            ~DelivererRegistry()
            {}

            Deliverer*
            getFromDelivererRegistry(std::string portname) const
            {
                return theRegistry_.find(portname);
            }

            std::list<std::string>
            getKeysFromDelivererRegistry() const
            {
                return theRegistry_.keys();
            }

            void
            addToDelivererRegistry(std::string portname, Deliverer* cr)
            {
                theRegistry_.insert(portname, cr);
            }

        protected:
            void
            updateDelivererRegistry(const std::string portname, Deliverer* cr)
            {
                theRegistry_.update(portname, cr);
            }

        private:
            container::Registry<std::string, Deliverer*> theRegistry_;
        };
    } // ldk
} // wns

#endif // WNS_LDK_DELIVERERREGISTRY_HPP
