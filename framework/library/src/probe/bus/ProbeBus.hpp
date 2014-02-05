/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#ifndef WNS_PROBE_BUS_PROBEBUS_HPP
#define WNS_PROBE_BUS_PROBEBUS_HPP

#include <WNS/simulator/Time.hpp>
#include <WNS/probe/bus/Context.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/StaticFactory.hpp>

namespace wns { namespace probe { namespace bus {


    namespace detail {
        class SubjectPimpl;
        class ObserverPimpl;
    }

    /**
     * @defgroup probebusses ProbeBusses
     * @ingroup group_main_classes
     *
     * @brief List of available ProbeBusses
     */

    /**
     * @brief Interface and elementary behaviour of a ProbeBus.
     *
     * A  ProbeBus is the basic element used  for measurement distribution within the simulator.
     * It comprises three aspects:
     *  - Processing
     *  - Forwarding
     *  - Connecting
     *
     * The Forwarding and Connecting aspect constitute the elementary behaviour and are already
     * implemented by ProbeBus. The Processing aspect is left unimplemented and Each implementation
     * must implement the processing aspect which comprises the methods
     *
     * accepts : Which is called to determine if the ProbeBus would accept the
     * measurement
     *
     * onMeasurement : Which is called to actually process the measurement. This is only called
     * if 'accepts' returned true.
     *
     * output : Which is called when the simulator triggers writing to persistant storage.
     *
     * The Forwarding aspect comprises the methods:
     *
     * forwardMeasurement : Which is called by a measurement source or by chained ProbeBusses. Calls
     * accepts and onMeasurement if accepts returned true. Calls forwardMeasurement on all observing
     * ProbeBusses (see Connecting aspect) if a call to this->accepts returns true.
     *
     * forwardOutput : Which is typically called by the simulator to trigger periodical storage of
     * measurement data. Unconditionally calls output and forwardOutput on all observing ProbeBusses.
     *
     * The Connecting aspect comprises the methods:
     *
     * startObserving : Which triggers this ProbeBus (Observer) to observe another ProbeBus (Subject).
     * Subject must be given as a parameter. After this the Subject includes the Observer in its
     * Forwarding aspect.
     * 
     * stopObserving : The Observer stops observing the subject ProbeBus.
     *
     * Use startObserving and stopObserving to interconnect ProbeBusses.
     *
     * @author Daniel Bueltmann <me@daniel-bueltmann.de>
     * @ingroup probebusses
     *
     */
    class ProbeBus
    {
    public:

        ProbeBus();

        virtual ~ProbeBus();

        /**
         * @name Processing Aspect
         */
        /*@{*/
        /**
         * @brief Called to determine if the ProbeBus would accept the
         * measurement
         *
         * @param time The current timestamp of the simulation
         *
         * @param context The context of your measurement.
         */
        virtual bool
        accepts(const wns::simulator::Time& time, const IContext& context) = 0;

        /**
         * @brief Called to actually process the measurement
         *
         * @param time The current timestamp of the simulation
         *
         * @param measurement The measured value.
         *
         * @param context The context of your measurement.
         */
        virtual void
        onMeasurement(const wns::simulator::Time& time,
                      const double& measurement,
                      const IContext& context) = 0;

        /**
         * @brief Called by the simulator to trigger periodical storage of
         * measurement data
         */
        virtual void
        output() = 0;
        /*@}*/

        /**
         * @name Forwarding Aspect
         */
        /*@{*/
        /**
         * @brief Forward measurement to self and all observers if self accepts.
         *
         * @param time The current timestamp of the simulation
         *
         * @param measurement The measured value.
         *
         * @param context The context of your measurement.
         *
         * It wraps the subject/observer backend nicely and allows users to
         * determine the behaviour by implementing accepts and onMeasurement.
         * When forwardMeasurement is called it is checked at first wether this
         * ProbeBus accepts this Measurement. If accepts returns true the
         * measurement is locally delivered by calling onMeasurement. After
         * local delivery the measurement is propagated to all Observers. If
         * the call to accepts returns false, the measurement is not delivered
         * locally and is not forwarded to the Observers.
         */
        virtual void
        forwardMeasurement(const wns::simulator::Time& time,
                           const double& measurement,
                           const IContext& context);

        /**
         * @brief Forward output trigger to self and all observers.
         *
         * This method unconditionally calls the output method to trigger
         * writing to persistant storage. After this it unconditionally
         * propagates the trigger to all its Observers.
         */
        virtual void
        forwardOutput();
        /*@}*/

        /**
         * @name Connecting Aspect
         */
        /*@{*/

        /**
         * @brief Start observing (connect to) another Probebus.
         *
         * @param other The ProbeBus to observe
         */
        virtual void
        startObserving(ProbeBus* other);

        /**
         * @brief Stop observing (connect to) another Probebus.
         *
         * @param other The ProbeBus to stop observing
         */
        virtual void
        stopObserving(ProbeBus* other);

        virtual bool
        hasObservers() const;
        /*@}*/

    private:
        detail::SubjectPimpl* subject_;
        detail::ObserverPimpl* observer_;
    };

    typedef PyConfigViewCreator<ProbeBus> ProbeBusCreator;
    typedef StaticFactory<ProbeBusCreator> ProbeBusFactory;


} // bus
} // probe
} // wns

/**
 * @page probing Collecting Measurements
 *
 * @section Introduction Introduction to the designated openWNS Probe Subsystem
 * In a nutshell, openWNS now decouples the generation and publication
 * of measurements from their processing, may that be logging, filtering,
 * sorting or simply storing them. In the following, it will shortly be
 * explained what generation, publication and processing are about.
 *
 * @section Generation Generation of Measurements
 * Generating measurement values is up to you, the Implementer of a certain,
 * class, module, node, whatever that might be. Measurements may currently be
 * everything that can be represented by a double value, like SINR,
 * Interference, Power values, delay times, packet sizes,
 * throughput figures ...
 *
 * @section Publication Publication of Measurements
 * Assuming you have obtained a measurement, you would like to publish it so
 * it can be handled in any way you desire. When publishing the measurement,
 * it is not necessary to know in which way and by whom it is being processed.
 * However, the entities that do process the measurement may require certain
 * additional information about the circumstances under which the measurement
 * was taken, e.g. which Node has taken the measurement, at which point in the
 * scenario it was taken, what kind of node took it, which Traffic Category
 * the measured packet belongs to, where the packet originated from, etc. etc.
 *
 * We refer to this kind of information as the so called @link wns::probe::bus::IContext context @endlink
 * of the measurement. Hence, the process of publishing the measurement involves
 * gathering and compiling this Context and then forwarding everything to the
 * appropriate processing entities. All this is handled by the so-called
 * ContextCollector. The only information the ContextCollector needs to have
 * about the processing of the measurements is a configurable name of the
 * processing channel into which to forward the measurement and the context
 * info.
 *
 * Entry points to the existing processing channels are governed by a global
 * registry called the wns::probe::bus::ProbeBusRegistry. They may be accessed
 * via the aforementioned name.
 * <b>NOTE</b> that the processing channels (further referred to as the
 * ProbeBusses) are <b>centralized</b>, while the publication of measurements
 * is <b>distributed</b>. This means that all entities that take and publish
 * measurements of the same type have a ContextCollector of their own, but all
 * these ContextCollectors forward their measurements and context into the same
 * wns::probe::bus::ProbeBus.
 *
 * @section Processing Processing of Measurements
 * Processing of measurements is accomplished by the so called
 * wns::probe::bus::ProbeBus. The ProbeBus is an object that supports
 * hierarchical chaining to form trees of ProbeBusses. The measurement and the
 * wns::probe::bus::context are inserted at the root of the tree and each node
 * may store and forward the received measurement, depending on the outcome of
 * an internal decision unit that determines whether or not the current node
 * will accept the measurement. By appropriate combination and parameterization,
 * the said tree can then be used to filter and thus sort the measurements
 * based on the available context information. openWNS provides a toolbox of
 * different ProbeBus implementations that can be flexibly combined to
 * accomplish all kinds of measurement processing tasks. In addition, the
 * simple wns::probe::bus::ProbeBus interface and the very generic
 * wns::probe::bus::context::IContext information concept allow for the
 * quick prototyping of tailor-made solutions.

 */
#endif // WNS_PROBE_BUS_PROBEBUS_HPP
