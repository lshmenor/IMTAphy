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

#ifndef WNS_LDK_FUNCTIONALUNITRC_HPP
#define WNS_LDK_FUNCTIONALUNITRC_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/GetHasConnector.hpp>
#include <WNS/ldk/Connector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/GetHasReceptor.hpp>
#include <WNS/ldk/Receptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/GetHasDeliverer.hpp>
#include <WNS/ldk/Deliverer.hpp>

namespace wns { namespace ldk {

        template <typename CLASS>
        class FunctionalUnitRC
            : public FunctionalUnit,
              public HasConnector<>,
              public HasReceptor<>,
              public HasDeliverer<>
        {
        public:
            FunctionalUnitRC(CLASS* fu)
                : fu_(fu)
            {}

            virtual
            ~FunctionalUnitRC()
            {}

            template <typename T>
            Connector*
            getPortConnector() const
            {
                return fu_->GetHasConnector<typename T::PortType>::getHasConnector()->getConnector();
            }

            template <typename T>
            Receptor*
            getPortReceptor() const
            {
                return fu_->GetHasReceptor<typename T::PortType>::getHasReceptor()->getReceptor();
            }

            template <typename T>
            Deliverer*
            getPortDeliverer() const
            {
                return fu_->GetHasDeliverer<typename T::PortType>::getHasDeliverer()->getDeliverer();
            }

            virtual bool
            doIsAccepting(const CompoundPtr&) const
            {
                wns::Exception e;
                e << "doIsAccepting(const CompoundPtr&) of RC FU may not be called!";
                throw e;
            }

            virtual void
            doSendData(const CompoundPtr&)
            {
                wns::Exception e;
                e << "doSendData(const CompoundPtr&) of RC FU may not be called!";
                throw e;
            }

            virtual void
            doWakeup()
            {
                wns::Exception e;
                e << "doWakeup() of RC FU may not be called!";
                throw e;
            }

            virtual void
            doOnData(const CompoundPtr&)
            {
                wns::Exception e;
                e << "doOnData(const CompoundPtr&) of RC FU may not be called!";
                throw e;
            }

        private:
            CLASS* fu_;
        };

    } //ldk
} //wns

#endif // WNS_LDK_FUNCTIONALUNITRC_HPP
