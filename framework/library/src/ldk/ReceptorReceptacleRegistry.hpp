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

#ifndef WNS_LDK_RECEPTORRECEPTACLEREGISTRY_HPP
#define WNS_LDK_RECEPTORRECEPTACLEREGISTRY_HPP

#include <WNS/ldk/IReceptorReceptacle.hpp>

#include <WNS/container/Registry.hpp>

#include <string>

namespace wns { namespace ldk {

        class ReceptorReceptacleRegistry
        {
        public:
            ReceptorReceptacleRegistry()
                : theRegistry_()
            {}

            ReceptorReceptacleRegistry(const ReceptorReceptacleRegistry&)
                : theRegistry_()
            {}

            virtual
            ~ReceptorReceptacleRegistry()
            {}

            IReceptorReceptacle*
            getFromReceptorReceptacleRegistry(std::string portname) const
            {
                return theRegistry_.find(portname);
            }

            std::list<std::string>
            getKeysFromReceptorReceptacleRegistry() const
            {
                return theRegistry_.keys();
            }

        protected:
            void
            updateReceptorReceptacleRegistry(const std::string portname, IReceptorReceptacle* cr)
            {
                theRegistry_.update(portname, cr);
            }

            void
            addToReceptorReceptacleRegistry(std::string portname, IReceptorReceptacle* cr)
            {
                theRegistry_.insert(portname, cr);
            }

        private:
            container::Registry<std::string, IReceptorReceptacle*> theRegistry_;
        };
    } // ldk
} // wns

#endif // WNS_LDK_RECEPTORRECEPTACLEREGISTRY_HPP
