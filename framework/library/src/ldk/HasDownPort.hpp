/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef WNS_LDK_HASDOWNPORT_HPP
#define WNS_LDK_HASDOWNPORT_HPP

#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/GetHasConnector.hpp>
#include <WNS/ldk/HasReceptorReceptacle.hpp>
#include <WNS/ldk/HasDelivererReceptacle.hpp>
#include <WNS/ldk/SinglePort.hpp>

namespace wns { namespace ldk {

        namespace hasconnector
        {
            typedef SingleConnector DefaultConnectorStrategy;
        }

        template <typename CLASS,
                  typename PORTID = Port<SinglePort>,
                  typename CONNECTORTYPE = hasconnector::DefaultConnectorStrategy>
        class HasDownPort
            : public GetHasConnector<typename PORTID::PortType>,
              public HasReceptorReceptacle<CLASS, typename PORTID::PortType>,
              public HasDelivererReceptacle<CLASS, typename PORTID::PortType>
        {
        public:
            HasDownPort(CLASS* fu):
                GetHasConnector<typename PORTID::PortType>(),
                HasReceptorReceptacle<CLASS, typename PORTID::PortType>(fu),
                HasDelivererReceptacle<CLASS, typename PORTID::PortType>(fu),
                connector_(fu)
            {
                setHasConnector(&connector_);
            }

            virtual
            ~HasDownPort()
            {}

        private:
            HasConnector<CONNECTORTYPE, typename PORTID::PortType> connector_;
        };


        template <typename CLASS,
                  typename CONNECTORTYPE>
        class HasDownPort<CLASS, Port<SinglePort>, CONNECTORTYPE>
        {
        public:
            HasDownPort(CLASS*)
            {}
        };

    } //ldk
} //wns

#endif // NOT defined WNS_LDK_HASDOWNPORT
