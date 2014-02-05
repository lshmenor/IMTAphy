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

#ifndef WNS_LDK_TOOLS_CONSTANTDELAY_HPP
#define WNS_LDK_TOOLS_CONSTANTDELAY_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/ldk/Command.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/events/CanTimeout.hpp>

namespace wns { namespace ldk { namespace tools {

    /**
	 * @brief This FU delays any outgoing compound by a fixed duration
	 */
    class ConstantDelay:
        public wns::ldk::fu::Plain<ConstantDelay, wns::ldk::EmptyCommand>,
        public wns::ldk::Delayed<ConstantDelay>,
        public wns::events::CanTimeout
    {
    public:
        /**
		 * @brief Constructor
		 */
        ConstantDelay(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

        /** @brief process incoming frames: Forward */
        virtual void
        processIncoming(const wns::ldk::CompoundPtr& compound);

        /** @brief process ougoing frames: Save & start delay */
        virtual void
        processOutgoing(const wns::ldk::CompoundPtr& compound);

        /** @brief Checks if a compound is saved */
        virtual bool
        hasCapacity() const;

        /** @brief Checks delay has passed & a compound is saved */
        virtual const wns::ldk::CompoundPtr
        hasSomethingToSend() const;

        /** @brief Returns the saved compound */
        virtual wns::ldk::CompoundPtr
        getSomethingToSend();

        /** @brief CanTimeout interface realisation: TryToSend */
        virtual void
        onTimeout();

    private:
        /** @brief Storage of one frame */
        wns::ldk::CompoundPtr currentFrame;

        /** @brief Duration to delay frames */
        const wns::simulator::Time delay;

        wns::logger::Logger logger;
    };

} // tools
} // ldk
} // wns

#endif // NOT defined WNS_LDK_TOOLS_CONSTANTDELAY_HPP


