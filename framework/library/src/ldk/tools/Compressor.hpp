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
#ifndef WNS_LDK_COMPRESSOR_HPP
#define WNS_LDK_COMPRESSOR_HPP

#include <WNS/ldk/Forwarding.hpp>
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Command.hpp>

namespace wns {
    namespace ldk {
        namespace tools {

            /**
             * @brief The Compressor reduces the size of the packet.
             *
             * The Compessor reduces the size of the payload data. The
             * amount of size reduction is given in Bits by the
             * reduction parameter. Use this FU to model Payload
             * Header Suppression (PHS).
             */
            class Compressor :
                public wns::ldk::fu::Plain<Compressor, wns::ldk::EmptyCommand>
            {
            public:
                Compressor(fun::FUN*, const wns::pyconfig::View& config);

                virtual void
                calculateSizes(const CommandPool* commandPool,
                               Bit& commandPoolSize, Bit& dataSize) const;

                void
                doSendData( const CompoundPtr& compound );

                void
                doOnData( const CompoundPtr& compound );

                bool doIsAccepting(const CompoundPtr& compound) const;

                void doWakeup();

            private:
                Bit reduction_;
                bool byteAlign_;
            };
        }
    }
}

#endif
