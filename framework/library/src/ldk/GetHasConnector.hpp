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

#ifndef WNS_LDK_GETHASCONNECTOR_HPP
#define WNS_LDK_GETHASCONNECTOR_HPP

#include <WNS/ldk/HasConnectorInterface.hpp>

namespace wns { namespace ldk {

        template <typename PORT>
        class GetHasConnector
        {
        public:
            GetHasConnector()
                : con_(NULL)
            {}

            void
            setHasConnector(HasConnectorInterface* con)
            {
                con_ = con;
            }

            HasConnectorInterface*
            getHasConnector()
            {
                return con_;
            }

        private:
            HasConnectorInterface* con_;
        };

    } //ldk
} //wns

#endif // WNS_LDK_GETHASCONNECTOR
