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

#ifndef WNS_LDK_HARQ_SOFTCOMBINING_UNIFORMRANDOMDECODER_HPP
#define WNS_LDK_HARQ_SOFTCOMBINING_UNIFORMRANDOMDECODER_HPP

#include <WNS/ldk/harq/softcombining/IDecoder.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/ldk/fun/FUN.hpp>

#include <WNS/distribution/Distribution.hpp>

namespace wns { namespace ldk { namespace harq { namespace softcombining {

     class UniformRandomDecoder:
        public IDecoder
     {
     public:

         UniformRandomDecoder(wns::ldk::fun::FUN*, const wns::pyconfig::View&);

         virtual ~UniformRandomDecoder();

         bool
         canDecode(const Container<wns::ldk::CompoundPtr>& c);

         void
         onFUNCreated();

     private:

         std::auto_ptr<wns::distribution::Distribution> dis_;

         double initialPER_;

         double rolloffFactor_;

         wns::logger::Logger logger_;
     };

} // softcombining
} // harq
} // ldk
} // wns

#endif // WNS_LDK_HARQ_SOFTCOMBINING_UNIFORMRANDOMDECODER_HPP
