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

#ifndef WNS_LDK_GETHASDELIVERER_HPP
#define WNS_LDK_GETHASDELIVERER_HPP

#include <WNS/ldk/HasDelivererInterface.hpp>

namespace wns { namespace ldk {

        template <typename PORT>
        class GetHasDeliverer
        {
        public:
            GetHasDeliverer()
                : del_(NULL)
            {}

            void
            setHasDeliverer(HasDelivererInterface* del)
            {
                del_ = del;
            }

            HasDelivererInterface*
            getHasDeliverer()
            {
                return del_;
            }

        private:
            HasDelivererInterface* del_;
        };

    } //ldk
} //wns

#endif // WNS_LDK_GETHASDELIVERER
