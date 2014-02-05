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

#ifndef WNS_LDK_HASCONNECTORRECEPTACLE_HPP
#define WNS_LDK_HASCONNECTORRECEPTACLE_HPP

#include <WNS/ldk/IConnectorReceptacle.hpp>
#include <WNS/ldk/ConnectorReceptacleRegistry.hpp>
#include <WNS/ldk/LinkHandlerInterface.hpp>
#include <WNS/ldk/SinglePort.hpp>
#include <WNS/ldk/Port.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/CommandTypeSpecifierInterface.hpp>

#include <WNS/Assure.hpp>

#include <string>

namespace wns { namespace ldk {

        template <typename CLASS, typename PORTID = SinglePort>
        class HasConnectorReceptacle
            : public virtual ConnectorReceptacleRegistry
        {
        public:
            HasConnectorReceptacle(CLASS* fu)
                : ConnectorReceptacleRegistry(),
                  connectorReceptacle_(new ConnectorReceptacle(fu))
            {
                addToConnectorReceptacleRegistry(PORTID().name, connectorReceptacle_);
            }

            HasConnectorReceptacle(const HasConnectorReceptacle&)
            {
                wns::Exception e;
                e << "The copy constructor of class HasConnectorReceptacle must not be called!";
                throw e;
            }

            virtual
            ~HasConnectorReceptacle()
            {
                delete connectorReceptacle_;
            }

            class ConnectorReceptacle
                : public virtual IConnectorReceptacle
            {
            public:
                ConnectorReceptacle(CLASS* fu)
                    : fu_(fu)
                {}

                virtual
                ~ConnectorReceptacle()
                {}

                virtual void
                sendData(const CompoundPtr& compound)
                {
                    fu_->getFUN()->getLinkHandler()->sendData(this, compound);
                }

                virtual void
                doSendData(const CompoundPtr& compound)
                {
                    fu_->doSendData(compound, Port<PORTID>());
                }

                virtual bool
                isAccepting(const CompoundPtr& compound)
                {
                    return fu_->getFUN()->getLinkHandler()->isAccepting(this, compound);
                }

                virtual bool
                doIsAccepting(const CompoundPtr& compound)
                {
                    return fu_->doIsAccepting(compound, Port<PORTID>());
                }

                virtual FunctionalUnit*
                getFU()
                {
                    return fu_;
                }

            private:
                CLASS* fu_;
            };

        private:
            HasConnectorReceptacle()
            {}

            ConnectorReceptacle* connectorReceptacle_;
        };


        template <typename CLASS>
        class HasConnectorReceptacle<CLASS, SinglePort>
            : public virtual IConnectorReceptacle,
              public virtual ConnectorReceptacleRegistry,
            public virtual CommandTypeSpecifierInterface
        {
        public:
            HasConnectorReceptacle()
                : ConnectorReceptacleRegistry()
            {
                addToConnectorReceptacleRegistry(SinglePort().name, this);
            }

            HasConnectorReceptacle(FunctionalUnit*)
                : ConnectorReceptacleRegistry()
            {
                addToConnectorReceptacleRegistry(SinglePort().name, this);
            }

            HasConnectorReceptacle(const HasConnectorReceptacle&)
                : ConnectorReceptacleRegistry()
            {
                addToConnectorReceptacleRegistry(SinglePort().name, this);
            }

            virtual
            ~HasConnectorReceptacle()
            {}

            virtual void
            sendData(const CompoundPtr& compound)
            {
                getFUN()->getLinkHandler()->sendData(this, compound);
            }

            virtual bool
            isAccepting(const CompoundPtr& compound)
            {
                getFUN()->getLinkHandler()->isAccepting(this, compound);
            }

            virtual FunctionalUnit*
            getFU()
            {
                return dynamic_cast<CLASS*>(this);
            }
        };


    } // ldk
} // wns

#endif // NOT defined WNS_LDK_HASCONNECTORRECEPTACLE_HPP
