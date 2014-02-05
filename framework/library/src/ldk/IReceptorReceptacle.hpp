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

#ifndef WNS_LDK_IRECEPTORRECEPTACLE_HPP
#define WNS_LDK_IRECEPTORRECEPTACLE_HPP

#include <WNS/ldk/Compound.hpp>

namespace wns { namespace ldk {

        class FunctionalUnit;

        class IReceptorReceptacle
        {
        public:
            virtual
            ~IReceptorReceptacle()
            {}

            virtual void
            wakeup() = 0;

            virtual void
            doWakeup() = 0;

            virtual FunctionalUnit*
            getFU() = 0;

        };

    } // ldk
} // wns

#endif // NOT defined WNS_LDK_IRECEPTORRECEPTACLE_HPP
