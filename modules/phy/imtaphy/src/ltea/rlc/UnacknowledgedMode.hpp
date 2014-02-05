/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

// based on code from openWNS with the following license:

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

#ifndef LTEA_RLC_UNACKNOWLEDGEDMODE_HPP
#define LTEA_RLC_UNACKNOWLEDGEDMODE_HPP

#include <WNS/ldk/sar/SegAndConcat.hpp>
#include <DLL/Layer2.hpp>

namespace ltea { namespace rlc {
    /**
     * @brief Implementation of LTE UM as described in 3GPP TS 36.322 V8.5.0
     * (2009-03) Section 5.1.2 ff
     */
    class UnacknowledgedMode:
        virtual public wns::ldk::sar::SegAndConcat
    {
    public:
        UnacknowledgedMode(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config);

        virtual
        ~UnacknowledgedMode();

        virtual void
        processIncoming(const wns::ldk::CompoundPtr& compound);

        virtual void
        processOutgoing(const wns::ldk::CompoundPtr&);
    private:
        dll::ILayer2* layer2;
    };    
}
}

#endif // LTEA_RLC_UNACKNOWLEDGEDMODE_HPP
