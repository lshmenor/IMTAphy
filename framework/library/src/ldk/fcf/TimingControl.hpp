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
#ifndef WNS_LDK_FCF_TIMINGCONTROL_H
#define WNS_LDK_FCF_TIMINGCONTROL_H

#include <WNS/ldk/ldk.hpp>
#include <WNS/ldk/fcf/PhaseDescriptor.hpp>
#include <WNS/ldk/fcf/FrameBuilderConfigCreator.hpp>
#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/logger/Logger.hpp>

#include <list>

namespace wns { namespace ldk { namespace fcf {

    class CompoundCollectorInterface;
    class FrameBuilder;

    /**
     * @brief Interface definition of the TimingControl.
     *
     * @ingroup frameConfigurationFramework
     */
    class TimingControlInterface
    {
    public:
        virtual ~TimingControlInterface()
        {
        }

        /**
         * @brief Configures the TimingControl
         */
        virtual void configure() = 0;

        /**
         * @brief Starts the timingcontrol's operation
         */
        virtual void start() = 0;

        /**
         * @brief Pause the timingcontrol's operation
         */
        virtual void pause() = 0;

        /**
         * @brief Stops the timingcontrol's operation
         */
        virtual void stop() = 0;

        /**
         * @brief Return the role for the questioned Phase
         */
        //virtual int getRole(PhaseDescriptorPtr p) = 0;

        virtual FrameBuilder* getFrameBuilder() const = 0;

        /**
         * @brief Inform the TimingControl about the end of the phase
         * that is controlled by the collector.
         */
        virtual void finishedPhase(CompoundCollectorInterface* collector) = 0;
    };


    /**
     * @brief Control entity of the TimingNodes.
     *
     * The TimingControl manages the set of TimingNodes for all frame
     * phases. TimingNodes inform the TimingControl whenever the phase has
     * finished. The timing control calls the next timing node to start its phase.
     *
     * @ingroup frameConfigurationFramework
     */
    class TimingControl :
        public virtual TimingControlInterface,
        public wns::events::PeriodicTimeout
    {
    public:
        typedef std::list<CompoundCollectorInterface*> CompoundCollectors;

        TimingControl( FrameBuilder* frameBuilder, const pyconfig::View& config );

        void onFUNCreated();

        virtual void nextPhase();

        virtual void configure();

        virtual void start();

        virtual void pause();

        virtual void stop();

        virtual int getRole(PhaseDescriptorPtr p);

        void periodically();

        virtual void finishedPhase( CompoundCollectorInterface* )
        {
            nextPhase();
        }

        virtual FrameBuilder* getFrameBuilder() const
        {
            return frameBuilder;
        }


    private:
        CompoundCollectors compoundCollectors;
        CompoundCollectors::iterator activeCC;
        FrameBuilder* frameBuilder;
        wns::logger::Logger logger;
        bool running;
    };

    typedef FrameBuilderConfigCreator<TimingControlInterface> TimingControlCreator;
    typedef wns::StaticFactory<TimingControlCreator> TimingControlFactory;

}
}
}
#endif

