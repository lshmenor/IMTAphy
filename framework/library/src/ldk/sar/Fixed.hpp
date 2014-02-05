/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef WNS_LDK_SAR_FIXED_HPP
#define WNS_LDK_SAR_FIXED_HPP

#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Delayed.hpp>

#include <WNS/ldk/sar/SAR.hpp>

#include <WNS/pyconfig/View.hpp>

#include <list>

namespace wns { namespace ldk { namespace sar {

    /**
     * @brief Segmentation and Reassembly with a fixed size MTU.
     */
    class Fixed :
        public SAR<SARCommand>,
        public HasReceptor<>,
        public HasConnector<>,
        public HasDeliverer<>,
        public Delayed<Fixed>,
        public Cloneable<Fixed>
    {
    public:
        Fixed(fun::FUN* fuNet, const wns::pyconfig::View& config);
        //
        // Completed Processor interface
        //
        virtual void processIncoming(const CompoundPtr& compound);

    private:
        std::list<CompoundPtr> incoming;
        int fragmentNumber;
    };
}
}
}

#endif // NOT defined WNS_LDK_SAR_FIXED_HPP


