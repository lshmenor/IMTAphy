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

#ifndef WNS_LDK_GROUP_HPP
#define WNS_LDK_GROUP_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/ldk/fun/Sub.hpp>

#include <WNS/logger/Logger.hpp>

#include <WNS/pyconfig/View.hpp>

namespace wns { namespace ldk {

        /**
         * @defgroup ldkgroup Group
         * @ingroup ldk
         *
         * @brief Build groups of FUs acting as a single FU.
         *
         */

        /**
         * @brief Functional Unit holding a Sub FUN.
         * @ingroup ldkgroup
         *
         * ldk::fun::Sub is a FUN that can be placed within another FUN. Group is
         * a FU wrapping such a Sub FUN.
         *
         * Group has two major use cases:
         *  1. Logical Grouping
         *  2. Interplay with FlowSeparator
         *
         */
        class Group :
            public virtual FunctionalUnit,
            public CommandTypeSpecifier<>,
            public HasReceptor<>,
            public HasConnector<>,
            public HasDeliverer<>,
            public Cloneable<Group>
        {
        public:
            Group(fun::FUN* fuNet, const pyconfig::View& _config);
            ~Group();

            Group(const Group& other);

            //
            // FU interface
            //
            virtual void onFUNCreated();
            virtual void setName(std::string _name);

            fun::Sub* getSubFUN() const;

        private:
            virtual void doSendData(const CompoundPtr& compound);
            virtual void doOnData(const CompoundPtr& compound);
            virtual bool doIsAccepting(const CompoundPtr& compound) const;
            virtual void doWakeup();

            pyconfig::View config;
            logger::Logger logger;

            fun::Sub* sub;
        };
    }
}

#endif // NOT defined WNS_LDK_GROUP_HPP


