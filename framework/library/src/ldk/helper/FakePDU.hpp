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

#ifndef WNS_OSI_HELPER_FAKEPDU_HPP
#define WNS_OSI_HELPER_FAKEPDU_HPP

#include <WNS/SmartPtr.hpp>

#include <WNS/osi/PDU.hpp>
#include <WNS/osi/PCI.hpp>

namespace wns { namespace ldk { namespace helper {

    class FakePDU :
        public wns::osi::PDU
    {
    public:
        FakePDU(Bit _length = 0) :
                wns::osi::PDU(new wns::osi::PCI),
                length(_length)
        {
        }

        void
        setLengthInBits(Bit _length)
        {
            length = _length;
        } // setLengthInBits

        FakePDU*
        clone()
        {
            return new FakePDU(*this);
        }

    private:
        virtual Bit
        doGetLengthInBits() const
        {
            return length;
        } // getLengthInBits

        Bit length;
    };

    typedef SmartPtr<FakePDU> FakePDUPtr;

}
}
}
#endif // NOT defined WNS_OSI_HELPER_FAKEPDU_HPP


