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

#ifndef WNS_LDK_RECEPTORREGISTRY_HPP
#define WNS_LDK_RECEPTORREGISTRY_HPP

#include <WNS/ldk/Receptor.hpp>

#include <WNS/container/Registry.hpp>

#include <string>

namespace wns { namespace ldk {

        class ReceptorRegistry
        {
        public:
            ReceptorRegistry()
                : theRegistry_()
            {}

            ReceptorRegistry(const ReceptorRegistry&)
                : theRegistry_()
            {}

            virtual
            ~ReceptorRegistry()
            {}

            Receptor*
            getFromReceptorRegistry(const std::string portname) const
            {
                return theRegistry_.find(portname);
            }

            std::list<std::string>
            getKeysFromReceptorRegistry() const
            {
                return theRegistry_.keys();
            }

            void
            addToReceptorRegistry(const std::string portname, Receptor* cr)
            {
                theRegistry_.insert(portname, cr);
            }

        protected:
            void
            updateReceptorRegistry(const std::string portname, Receptor* cr)
            {
                theRegistry_.update(portname, cr);
            }

        private:
            container::Registry<std::string, Receptor*> theRegistry_;
        };
    } // ldk
} // wns

#endif // WNS_LDK_RECEPTORREGISTRY_HPP
