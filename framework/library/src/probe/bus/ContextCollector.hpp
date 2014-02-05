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

#ifndef WNS_PROBE_BUS_PUTDECORATOR_HPP
#define WNS_PROBE_BUS_PUTDECORATOR_HPP

#include <WNS/probe/bus/ContextProvider.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>
#include <WNS/probe/bus/ProbeBusRegistry.hpp>
#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/simulator/ISimulator.hpp>

#include <WNS/osi/PDU.hpp>

#include <WNS/RefCountable.hpp>
#include <WNS/SmartPtr.hpp>

#include <boost/tuple/tuple.hpp>

#include <vector>

namespace wns { namespace probe { namespace bus {

	/**
	 * @brief Front-end for the probing subsystem.
     *
	 * @ref wns.probe.bus.contextcollector
     * The ContextCollector
	 * -# takes your measured value
	 * -# uses a visitor pattern to gather as much context information
	 *    related to the measurement as it can
	 * -# forwards the measurement and the context to potential measurement
	 *    sinks in the probebus subsystem
	 */
	class ContextCollector :
		virtual public wns::RefCountable
	{
		ContextProviderCollection contextProviders_;
        /** @brief The ProbeBus where we publish our measurements */
        wns::probe::bus::ProbeBus* probeBus_;

        template <typename Tuple, int keyIndex, int valueIndex>
        struct detail
        {
            static void fillContext(Context& c, const Tuple& tuple)
                {
                    std::string k = boost::tuples::get<keyIndex>(tuple);

                    c.insert(k, boost::tuples::get<valueIndex>(tuple));

                    detail<Tuple, keyIndex-2, valueIndex-2>::fillContext(c, tuple);
                }
        };

        template <typename Tuple>
        struct detail<Tuple, 0, 1>
        {
            static void fillContext(Context& c, const Tuple& tuple)
                {
                    std::string k = boost::tuples::get<0>(tuple);

                    c.insert(k, boost::tuples::get<1>(tuple));
                }
        };

    public:

        ContextCollector(std::string probeBusId);

        /**
         * @brief Stores a copy of the contextProviders and retrieves
         * a ProbeBus from the ProbeBusRegistry
         */
		ContextCollector(ContextProviderCollection contextProviders,
						 std::string probeBusId);

		~ContextCollector()
		{};

        bool
        hasObservers() const;

        void
        put(double value) const;

        /**
         * @brief Add a variable number of context entries in place.
         *
         * Example:
         *
         * cc_.put(1.234, make_tuple("Key1", 1, "Key2", "stringvalue"));
         */
        template<typename Tuple>
        void
        put(double value, const Tuple& contextentries) const
            {
                // early return if no one is listening
                if (!probeBus_->hasObservers())
                {
                    return;
                }
                // Create vanilla Context object
                Context c;

                contextProviders_.fillContext(c);

                ContextCollector::detail<Tuple, boost::tuples::length<Tuple>::value-2, boost::tuples::length<Tuple>::value-1>::fillContext(c, contextentries);

                // determine simTime
                wns::simulator::Time t = wns::simulator::getEventScheduler()->getTime();
                probeBus_->forwardMeasurement(t, value, c);
            }

        void
        put(const wns::osi::PDUPtr&, double value) const;

        template<typename Tuple>
        void
        put(const wns::osi::PDUPtr& compound, double value, const Tuple& contextentries) const
            {
                // early return if no one is listening
                if (!probeBus_->hasObservers())
                {
                    return;
                }
                // Create vanilla Context object
                Context c;

                contextProviders_.fillContext(c, compound);

                ContextCollector::detail<Tuple, boost::tuples::length<Tuple>::value-2, boost::tuples::length<Tuple>::value-1>::fillContext(c, contextentries);

                // determine simTime
                wns::simulator::Time t = wns::simulator::getEventScheduler()->getTime();
                probeBus_->forwardMeasurement(t, value, c);
            }
    };

	typedef wns::SmartPtr<ContextCollector> ContextCollectorPtr;

}}}

/**
 * @page wns_probe_bus_probing openWNS Probing Subsystem
 *
 * @section subpages Subpages
 * @subpage wns_probe_bus_contextcollector
 *
 * @section intro Introduction
 *
 * In a nutshell, openWNS now decouples the generation and publication of
 * measurements from their processing, may that be logging, filtering, sorting
 * or simply storing them. In the following, it will shortly be explained what
 * generation, publication and processing are about.
 *
 * @section generation Generation of Measurements
 *
 * Generating measurement values is up to you, the Implementer of a certain,
 * class, module, node, whatever that might be. Measurements may currently be
 * everything that can be represented by a double value, like SINR,
 * Interference, Power values, delay times, packet sizes, throughput figures ...
 *
 * @section publication Publication of Measurements
 *
 * Assuming you have obtained a measurement, you would like to publish it so
 * it can be handled in any way you desire. When publishing the measurement,
 * it is not necessary to know in which way and by whom it is being processed.
 * However, the entities that do process the measurement may require certain
 * additional information about the circumstances under which the measurement
 * was taken, e.g. which Node has taken the measurement, at which point in the
 * scenario it was taken, what kind of node took it, which Traffic Category
 * the measured packet belongs to, where the packet originated from, etc. etc.
 *
 * We refer to this kind of information as the so called
 * @link wns::probe::bus:::IContext Context @endlink of the measurement. Hence, 
 * the process of publishing the measurement involves gathering and compiling
 * this Context and then forwarding everything to the appropriate processing
 * entities. All this is handled by the
 * @link wns::probe::bus::ContextCollector ContextCollector@endlink.
 * The only information the
 * @link wns::probe::bus::ContextCollector ContextCollector@endlink needs to have
 * about the processing of the measurements is a configurable name of the
 * ProbeBusID into which to forward the measurement and the context info.
 *
 * Entry points to the existing ProbeBusses are governed by a global Registry
 * called the @link wns::probe::bus::ProbeBusRegistry ProbeBusRegistry@endlink.
 * They may be accessed via the aforementioned name.
 *
 * <b>NOTE</b> that the @link wns::probe::bus::ProbeBus ProbeBusses @endlink
 * are <b>centralized</b>, while the publication of measurements is
 * <b>distributed</b>. This means that all entities that take and publish
 * measurements of the same type have a
 * @link wns::probe::bus::ContextCollector ContextCollector@endlink
 * of their own, but all these @link wns::probe::bus::ContextCollector
 * ContextCollectors @endlink forward their measurements and context into
 * the same @link wns::probe::bus::ProbeBus ProbeBus@endlink.
 *
 * See @ref wns_probe_bus_contextcollector for more information on the
 * @link wns::probe::bus::ContextCollector ContextCollector@endlink
 *
 * @section processing Processing of Measurements
 *
 * Processing of measurements is accomplished by the
 * @link wns::probe::bus::ProbeBus ProbeBus@endlink.
 * The @link wns::probe::bus::ProbeBus ProbeBus@endlink is an object that
 * supports hierarchical chaining to form trees of
 * @link wns::probe::bus::ProbeBus ProbeBusses@endlink.
 * The measurement and the @link wns::probe::bus::IContext context@endlink are
 * inserted at the root of the tree and each
 * @link wns::probe::bus::ProbeBus ProbeBus@endlink may
 * store and forward the received measurement, depending on the outcome of an
 * internal decision unit that determines whether or not the current node
 * @link wns::probe::bus::ProbeBus::accepts() accepts@endlink the measurement.
 * By appropriate combination and parameterization, the said tree can then be
 * used to filter and thus sort the measurements based on the available
 * @link wns::probe::bus::IContext context@endlink information. openWNS
 * provides a @ref probebusses "toolbox" of different
 * @linkwns::probe::bus::ProbeBus ProbeBus@endlink implementations
 * that can be flexibly combined to accomplish all kinds of measurement
 * processing tasks. In addition, the simple
 * @link wns::probe::bus::ProbeBus ProbeBus@endlink interface and the very
 * generic @link wns::probe::bus::IContext context@endlink information concept
 * allow for the quick prototyping of tailor-made solutions.
 * @section contextcollector Measurement Publication by using the ContextCollector
 * @copydoc wns_probe_bus_contextcollector
 *
 * @page wns_probe_bus_contextcollector Measurement Publication by using the ContextCollector
 *
 * The ContextCollector serves the task of gathering context information. To do
 * so, it creates an empty Context object and asks the ContextProviderCollection
 * to add information into that object. After that, it forwards the measured
 * value and the Context to the (root of a) ProbeBus hierarchy.
 * When being constructed, the ContextCollector relies on two mandatory parameters:
 *
 *   -# A ContextProviderCollection, of which the ContextCollector will make a
 *   copy for its internal purposes. (The copying allows to instantiate a number
 *   of ContextCollectors with the same instance of the local
 *   ContextProviderCollection)
 *   -# The unique, identifying name of the ProbeBus into which to forward the
 *   measurements (under this name the ContextCollector will look up in the
 *   wns::probe::bus::ProbeBusRegistry
 * This procedure is illustrates in the sequence diagram below
 * @msc
 * hscale="2";
 * MeasurementSource,
 * ContextCollector [label = "ContextCollector", URL="\ref wns::probe::bus::ContextCollector"],
 * ContextProviderCollection [label = "ContextProviderCollection", URL="\ref wns::probe::bus::ContextProviderCollection"],
 * ContextProvider [label = "ContextProvider", URL="\ref wns::probe::bus::IContextProvider"],
 * ProbeBus [label = "ProbeBus", URL="\ref wns::probe::bus::ProbeBus"];
 *
 * MeasurementSource=>>ContextCollector [label = "ContextCollector(ContextProviderCollection, probeBusID)", URL="\ref wns::probe::bus::ContextCollector::ContextCollector()", ID="1"];
 * MeasurementSource=>>ContextCollector [label = "put(measurement)", URL="\ref wns::probe::bus::ContextCollector::put()"];
 * ContextCollector=>>ContextProviderCollection [label = "fillContext(context, compound)", URL="\ref wns::probe::bus::ContextProviderCollection::fillContext()", ID="2"];
 * --- [label = "foreach ContextProvider in ContextProviderCollection"];
 * ContextProviderCollection=>>ContextProvider [label = "visit(context, compound)", URL="\ref wns::probe::bus::IContextProvider::visit()", ID="3"];
 * ContextProviderCollection<<ContextProvider;
 * --- [label = "next ContextProvider"];
 * ContextCollector<<ContextProviderCollection;
 * ContextCollector=>>ProbeBus [label = "forwardMeasurement(timestamp, measurement, context)", URL="\ref wns::probe::bus::ProbeBus::forwardMeasurement()"];
 * --- [label = "ProbeBus forwards measurement to all its accepting children"];
 * ContextCollector<<ProbeBus;
 * MeasurementSource<<ContextCollector;
 * @endmsc
 * <OL>
 * <LI> In this example the MeasurementSource creates the ContextCollector. The
 * creation can be done by any object. The MeasurementSource only needs to know
 * its ContextCollector object to place the put() call.
 * <LI> If only the measurement is used when putting the compound is set to an
 * empty compound automatically. A description on gathering context from
 * compounds will follow
 * <LI> The ContextProvider may add any number of key, value pairs to the Context
 * </OL>
 */
#endif // NOT defined WNS_PROBE_PUTDECORATOR_HPP
