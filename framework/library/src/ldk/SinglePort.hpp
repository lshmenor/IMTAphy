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

#ifndef WNS_LDK_SINGLEPORT_HPP
#define WNS_LDK_SINGLEPORT_HPP

#include <WNS/ldk/IPortID.hpp>

#include <string>

namespace wns { namespace ldk {

        class SinglePort
            : public virtual IPortID
        {
        public:
            static const std::string name;
        };

        //        const std::string SinglePort::name = "SinglePort";


    } // ldk
} // wns

#endif // NOT defined WNS_LDK_SINGLEPORT_HPP
