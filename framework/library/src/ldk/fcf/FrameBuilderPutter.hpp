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
#ifndef WNS_LDK_FCF_FRAMEBUILDERPUTTER_H
#define WNS_LDK_FCF_FRAMEBUILDERPUTTER_H

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/fun/FUN.hpp>

namespace wns { namespace ldk {

    class ILayer;

    namespace fun
    {
        class FUN;
    }

    namespace fcf
    {

        /// The FrameBuilderPutter forwards incoming packets to the FrameBuilder.
        /**
         * The FrameBuilderPutter is a helper class for the FrameBuilder. The
         * FrameBuilder does only accept pairs of functional unit
         * connections. The FrameBuilderPutter indicates compounds directly to
         * the FrameBuilder.
         * @todo This class has not a real function. It is a workaround.
         * @ingroup frameConfigurationFramework
         */
        class FrameBuilderPutter
            : public virtual wns::ldk::FunctionalUnit,
              public CommandTypeSpecifier<>,
              public HasReceptor<>,
              public HasConnector<>,
              public HasDeliverer<>,
              public Cloneable<FrameBuilderPutter>
        {
        public:
            FrameBuilderPutter( wns::ldk::fun::FUN*, const wns::pyconfig::View& );

            void doOnData(const wns::ldk::CompoundPtr&);
            void doSendData(const wns::ldk::CompoundPtr&);
            void onFUNCreated();

        private:
            void doWakeup()
            {
            }
            bool doIsAccepting(const wns::ldk::CompoundPtr&) const;

            std::string frameBuilderName;
            struct
            {
                wns::ldk::FunctionalUnit* destination;
            } friends;
        };
}
}
}
#endif

