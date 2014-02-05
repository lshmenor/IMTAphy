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

#ifndef WNS_LDK_HASUPPORT_HPP
#define WNS_LDK_HASUPPORT_HPP

#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/GetHasDeliverer.hpp>
#include <WNS/ldk/SingleDeliverer.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/GetHasReceptor.hpp>
#include <WNS/ldk/SingleReceptor.hpp>
#include <WNS/ldk/HasConnectorReceptacle.hpp>
#include <WNS/ldk/SinglePort.hpp>

namespace wns { namespace ldk {

        namespace hasdeliverer
        {
            typedef SingleDeliverer DefaultDelivererStrategy;
        }

        namespace hasreceptor
        {
            typedef SingleReceptor DefaultReceptorStrategy;
        }

        template <typename CLASS,
                  typename PORTID = Port<SinglePort>,
                  typename DELIVERERTYPE = hasdeliverer::DefaultDelivererStrategy,
                  typename RECEPTORTYPE = hasreceptor::DefaultReceptorStrategy>
        class HasUpPort
            : public GetHasDeliverer<typename PORTID::PortType>,
              public GetHasReceptor<typename PORTID::PortType>,
              public HasConnectorReceptacle<CLASS, typename PORTID::PortType>
        {
        public:
            HasUpPort(CLASS* fu)
                : GetHasDeliverer<typename PORTID::PortType>(),
                  GetHasReceptor<typename PORTID::PortType>(),
                  HasConnectorReceptacle<CLASS, typename PORTID::PortType>(fu),
                  deliverer_(fu),
                  receptor_(fu)
            {
                setHasDeliverer(&deliverer_);
                setHasReceptor(&receptor_);
            }

            virtual
            ~HasUpPort()
            {}

        private:
            HasDeliverer<DELIVERERTYPE, typename PORTID::PortType> deliverer_;
            HasReceptor<RECEPTORTYPE, typename PORTID::PortType> receptor_;
        };


        template <typename CLASS,
                  typename DELIVERERTYPE,
                  typename RECEPTORTYPE>
        class HasUpPort<CLASS, Port<SinglePort>, DELIVERERTYPE, RECEPTORTYPE>
        {
        public:
            HasUpPort(CLASS*)
            {}
        };

    } //ldk
} //wns

#endif // NOT defined WNS_LDK_HASUPPORT
