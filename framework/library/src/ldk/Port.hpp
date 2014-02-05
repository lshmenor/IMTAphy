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

#ifndef WNS_LDK_PORT_HPP
#define WNS_LDK_PORT_HPP

namespace wns { namespace ldk {

        template <typename PORT>
        class Port
        {
        public:
            typedef PORT PortType;
        };

    } // ldk
} // wns

#endif // NOT defined WNS_LDK_PORT_HPP
