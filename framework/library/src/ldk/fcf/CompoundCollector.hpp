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
#ifndef WNS_LDK_FCF_COMPOUNDCOLLECTOR_H
#define WNS_LDK_FCF_COMPOUNDCOLLECTOR_H

#include <WNS/Cloneable.hpp>
#include <WNS/ldk/ldk.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/events/CanTimeout.hpp>
#include <WNS/ldk/fcf/FrameBuilder.hpp>

#include <vector>

namespace wns { namespace ldk { namespace fcf {

    class FrameBuilder;
    class TimingNode;
    /**
     * @brief Interface definition of the CompoundCollector.
     *
     * The CompoundCollector is part of the frame configuration framework. The
     * CompoundCollector prepares compounds for the transmission in a frame which is
     * controlled by the FrameBuilder.
     *
     * @ingroup frameConfigurationFramework
     */
    class CompoundCollectorInterface
    {
    public:
        virtual ~CompoundCollectorInterface()
        {
        }

        /**
         * @brief Triggers the CompoundCollector to collect compounds.
         */
        virtual void startCollection(int) = 0;

        /**
         * @brief Triggers the CompoundCollector to resolve horiziontal dependencies.
         */
        virtual void finishCollection() = 0;

        /**
         * @brief Start the transmission of collected compounds.
         */
        virtual void start(int) = 0;

        /**
         * @brief Stop the current transmissions.
         */
        virtual void stop() = 0;
        /**
         * @brief Returns the current mode of the collector.
         */
        virtual int getMode() const = 0;


        virtual void setFrameBuilder(FrameBuilder*) = 0;

        /**
         * @brief  Returns the duration of all collected compounds so far.
         *
         * Sometimes CompoundCollectors need to know how much time is still free for
         * their phase. The FrameBuilder asks all CompoundCollectors how much time is
         * already used by the CompoundCollectors. The minimum duration is the duration
         * hint of the PhaseDescriptor.
         */
        virtual simTimeType getCurrentDuration() const = 0;

        /**
         * @brief Returns the maximum duration the compound collector may use for its phase.
         */
        virtual simTimeType getMaximumDuration() const = 0;

        virtual void setMaximumDuration( simTimeType duration ) = 0;

    };


    /**
     * @brief  Base for all compound collectors. Actually compound collectors are functional units.
     *
     * The CompoundCollector implements some basic functionality of the
     * CompoundCollectorInterface. Other compound collectors may use the basic
     * implementation of the CompoundCollector.
     */
    class CompoundCollector	:
        public virtual CompoundCollectorInterface,
        public virtual wns::ldk::FunctionalUnit,
        public virtual PythonicOutput
    {
    public:
        enum Mode
        {
            Sending,
            Receiving,
            Pausing
        };

        static const char* mode2String(int mode)
        {
            switch (mode)
            {
                case Sending:
                    return "Sending";
                case Receiving:
                    return "Receiving";
                case Pausing:
                    return "Pausing";
            }
            return "Unknown";
        }

        CompoundCollector( const wns::pyconfig::View& config) :
            mode(0),
            started(false),
            maxDuration(0.0),
            frameBuilder(0),
            timingNode(0)
        {
            frameBuilderName = config.get<std::string>("frameBuilderName");
        }

        simTimeType getMaximumDuration() const { return maxDuration; }

        FrameBuilder* getFrameBuilder() const { return frameBuilder; }

        void setFrameBuilder( FrameBuilder* _frameBuilder )
        { frameBuilder = _frameBuilder; }


        int getMode() const
        {
            return mode;
        }


        void startCollection(int _mode)
        {
            mode = _mode;
            doStartCollection(_mode);
        }


        void start(int _mode)
        {
            assure(_mode == getMode(), "compound collector starting with wrong mode" );
            doStart(_mode);
            started = true;
        }

        void stop()
        {
            assure(started, "Stop called for inactive compound collector.");
            doStop();
            started = false;
        }

        virtual void doStartCollection(int) = 0;
        virtual void doStart(int) = 0;
        virtual void doStop() {};

        virtual void doWakeup()
        {
            throw wns::Exception("Invalid wakeup call in CompoundCollector");
        }


    protected:
        int mode;

    private:

        void setMaximumDuration( simTimeType duration )
        {
            maxDuration = duration;
        }

        void setTimingNode( TimingNode* _timingNode )
        {
            timingNode = _timingNode;
        }

        bool started;
        simTimeType maxDuration;
        FrameBuilder* frameBuilder;
        std::string frameBuilderName;
        TimingNode* timingNode;

        friend class PhaseDescriptor;
    };

}
}
}
#endif


