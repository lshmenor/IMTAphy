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
#ifndef WNS_LDK_FCF_FRAMEBUILDER_H
#define WNS_LDK_FCF_FRAMEBUILDER_H

#include <list>

#include <WNS/ldk/ldk.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/ldk/multiplexer/Dispatcher.hpp>

#include <WNS/ldk/fcf/PhaseDescriptor.hpp>
#include <WNS/ldk/fcf/NewFrameProviderObserver.hpp>

namespace wns { namespace ldk {

    class ILayer;

    namespace fcf {


        /** @defgroup frameConfigurationFramework Frame Configuration
         * Framework (FCF) The Frame Configuration Framework offers a
         * set of functional units to easily configure the frame
         * layout. The central element of the framework is the
         * FrameBuilder.
         *
         * The FrameBuilder is configured by a list of
         * PhaseDescriptors. For each phase a CompoundCollector handles
         * the compounds that are to be sent during the phase. Building
         * the frame is done in three steps:
         *   - The FrameBuilder activates the CompoundCollectors 
               depending on the activations, listed in
         *   - The FrameBuilder calls startCollection() of each
         *     compound collector. Compounds are collected dependend on
         *     the type of the collector.
         *   - If all collectors have finished thier collection the
         *     have a chance to fine tune ther collection though their
         *     method: finishCollection(). Here dependencies to other
         *     compound collectors may be resolved.
         *   - The frame builder triggers the first compound collector to
         *     start transmitting the compounds of the first phase.
         *     When the phase is finished the timing control starts the
         *     next timing node.
         *
         * The CompoundCollector catches compounds from upper functional
         * units. When the TimingControl starts the frame. All
         * CompoundCollectors are finished by finishCollection(). The
         * CompoundCollectors may do final configurations to their
         * collected compounds. The TimingControl activates the
         * TimingNode of the first Phase, which starts the compound
         * transmission of the corresponding CompoundCollector. The
         * TimingNode itself informs the TimingControl when its
         * transmission has finished.
         *
         \dot
         digraph frameBuilder {
         subgraph cluster {
         frameBuilderNode [shape=record, label="<p0>Opcode\nSetter|<p1>Opcode\nSetter|<p2>Opcode\nSetter|<p3>Opcode\nSetter", width=5.0];
         label = "Frame Builder";
         timingControl [shape=record];
         phaseDescriptors [shape=record, label="Phase\nDescriptors"];
         style=filled;
         fillcolor=skyblue;
         }

         node [shape=record, fontname="helvetica", fontsize=10];
         edge [fontname="helvetica", fontsize=10];

         compoundCollector0 [label="Compound\nCollector"];
         compoundCollector1 [label="Compound\nCollector"];
         compoundCollector2 [label="Compound\nCollector"];
         compoundCollector3 [label="Compound\nCollector"];

         timingControl [label="Timing\nControl"];

         phyuser [label="Phy\nUser"];

         { rank=min; compoundCollector0; compoundCollector1; compoundCollector2; compoundCollector3; }
         compoundCollector0 -> frameBuilderNode:p0 [weight=10];
         compoundCollector1 -> frameBuilderNode:p1 [weight=10];
         compoundCollector2 -> frameBuilderNode:p2 [weight=10];
         compoundCollector3 -> frameBuilderNode:p3 [weight=10];

         frameBuilderNode -> phyuser [weight=10];

         phaseDescriptors -> compoundCollector0 [style=dotted];
         phaseDescriptors -> compoundCollector1 [style=dotted];
         phaseDescriptors -> compoundCollector2 [style=dotted];
         phaseDescriptors -> compoundCollector3 [style=dotted];
         }
         \enddot

         * For an overview of the participating elements of the frame
         * setup have a look at the following concept graph.

         \dot
         digraph d {

         node [fontname="helvetica", fontsize=10, shape=record, style=filled, fillcolor="yellow"];
         edge [fontname="helvetica", fontsize=10];

         frameBuilder [label="Frame\nBuilder"];
         timingControl [label="Timing\nControl"];
         compoundCollector [label="Compound\nCollector"];
         phaseDescriptor [label="Phase\nDescriptors"];

         frameBuilder -> compoundCollector [label="owns 0 .. *", arrowhead=odiamond];
         frameBuilder -> timingControl [label="owns 1", arrowhead=odiamond];
         frameBuilder -> phaseDescriptor [label="owns 0 .. *", arrowhead=odiamond];
         timingControl -> compoundCollector [label="activates"];
         compoundCollector -> timingControl [label="informs about\nfinish"];
         compoundCollector -> frameBuilder [label="knows"];
         }
         \enddot
         */

        class TimingControlInterface;
        class CompoundCollectorInterface;

        /// The FrameBuilder keeps track of the frame phases.
        /**
         * The FrameBuilder is the central element of the Frame Configuration
         * Framework. Frame phases are configured through a list of phase descriptors
         * given by the pyconfig::View. The TimingControl is part of the
         * FrameBuilder.
         * \ingroup frameConfigurationFramework
         */
        class FrameBuilder
            : public wns::ldk::multiplexer::Dispatcher,
              public wns::ldk::fcf::NewFrameProvider
        {
        public:
            typedef std::list< PhaseDescriptorPtr > Descriptors;

            FrameBuilder( wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config );
            ~FrameBuilder();

            void onFUNCreated();

            /// Get the maximum duration of the whole frame.
            simTimeType getFrameDuration() const
            {
                return frameDuration_;
            }

            /// Returns a copy of all phase descriptors.
            Descriptors getAllPhaseDescriptors() const
            {
                return descriptors_;
            }

            /// Start the timing control.
            void start();

            /// Pause the timing control.
            void pause();

            /// Stop the timing control.
            void stop();

            virtual void finishedPhase( CompoundCollectorInterface* collector );

            TimingControlInterface*
            getTimingControl() const
            {
                return timingControl_;
            }
        private:

            Descriptors descriptors_;
            simTimeType frameDuration_;
            simTimeType symbolDuration_;

            TimingControlInterface* timingControl_;
            wns::logger::Logger logger;
        };
    }
}
}
#endif


