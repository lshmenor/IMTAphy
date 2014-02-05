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

#ifndef WNS_LDK_SINGLECONNECTION_HPP
#define WNS_LDK_SINGLECONNECTION_HPP

#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/SingleConnector.hpp>

namespace wns { namespace ldk {

        /**
         * @brief HasConnector specific type declarations.
         *
         */
        namespace hasconnector
        {
            /**
             * @brief Default strategy to be used for lower scheduling.
             *
             */
            typedef SingleConnector DefaultConnectorStrategy;
        } // hasconnector

        template <typename T = hasconnector::DefaultConnectorStrategy>
        class SingleConnection
            : public HasConnector
        {
        public:
            typedef T ConnectorType;

            SingleConnection() :
                HasConnector(),
                lower_()
            {
            }

            virtual
            ~SingleConnection()
            {
            }

            template <>
            ConnectorInterface*
            getConnector<>() const
            {
                return &lower_;
            }

        private:
            ConnectorType lower_;

        };

    } //ldk
} //wns

#endif // NOT defined WNS_LDK_SINGLECONNECTION_HPP

