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

#ifndef WNS_LDK_GETHASRECEPTOR_HPP
#define WNS_LDK_GETHASRECEPTOR_HPP

#include <WNS/ldk/HasReceptorInterface.hpp>

namespace wns { namespace ldk {

        template <typename PORT>
        class GetHasReceptor
        {
        public:
            GetHasReceptor()
                : rec_(NULL)
            {}

            void
            setHasReceptor(HasReceptorInterface* rec)
            {
                rec_ = rec;
            }

            HasReceptorInterface*
            getHasReceptor()
            {
                return rec_;
            }

        private:
            HasReceptorInterface* rec_;
        };

    } //ldk
} //wns

#endif // WNS_LDK_GETHASRECPTOR
