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

#ifndef DLL_COMPOUNDSWITCH_COMPOUNDSWITCH_HPP
#define DLL_COMPOUNDSWITCH_COMPOUNDSWITCH_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Processor.hpp>

#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <DLL/compoundSwitch/CompoundSwitchConnector.hpp>
#include <DLL/compoundSwitch/CompoundSwitchDeliverer.hpp>

namespace dll { namespace compoundSwitch {

    class FilterInterface;
    class Filter;

    /**
     * @brief Command for the FU CompoundSwitch and its filters
     * @author Markus Grauer <gra@comnets.rwth-aachen.de>
     *
     */

    class CompoundSwitchCommand :
        public wns::ldk::Command
    {
    public:
        CompoundSwitchCommand()
        {
            local.filterName = "";
        }

        struct 
        {
            // Used by filter::FilterByFilterName
            std::string filterName;
        } local;
		struct {} peer;
		struct {} magic;
	};

	/**
	 * @brief This FU switch compounds by a filter to a specified FU connection.
	 * @author Markus Grauer <gra@comnets.rwth-aachen.de>
     *                  -------
     *                 | upper |
     *                  -------
     *                     |
     *    ----------------------------------
     *   |             compoundSwitch      |
     *    ----------------------------------
     *       |filer1   |filer2          |filter3
     *       v         v                v
     *    ------    ------           ------
     *   | FU 1 |  | FU 2 |   ...   | FU N |
     *    ------    ------           ------
	 *
	 * The CompoundSwitch direct the compound to the first FU connection
	 * which filter matches. These functionality is implemented for compound
	 * processed by the Deliverer/onData and the Connector/sendData.
	 *
	 * Configuring:
	 * The filter for the processed compounds are defined in the PyConfig.
	 * There are lists for onData and sendData filters. All possible filters are
	 * related to compoundSwitch::filter.
	 * The order of the FU connections must require the order of the filters in
	 * the filterlist to bind the filter to the right FU connection.
     *
     * With the parameter mustAccept, two different behaviors can be selected
     * for an outgoing compound:
     *    1) mustAccept = true --> take the first FU whose filter matches AND
     * which is accepting.
     *    2) mustAccept = false --> take the first FU whose filter matches,
     * independently if the FU is accepting.
	 *
	 * @todo (gra): It should migrate to the LDK, after it is unit tested
	 *              and some more detailed documentation.
	 */

	class CompoundSwitch :
		virtual public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<CompoundSwitchCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<CompoundSwitchConnector>,
		public wns::ldk::HasDeliverer<CompoundSwitchDeliverer>,
		public wns::Cloneable<CompoundSwitch>
	{
	public:
		CompoundSwitch(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config);

		~CompoundSwitch();

		virtual void onFUNCreated();

		wns::ldk::FunctionalUnit*
		findFUNFriend( std::string friendName);

		wns::ldk::CommandProxy*
		getCommandProxy()
	    {
			return getFUN()->getProxy();
		}

	private:

		typedef std::list<Filter*> Filters;

		virtual void
		doSendData(const wns::ldk::CompoundPtr& compound);

		virtual void
		doOnData(const wns::ldk::CompoundPtr& compound);

		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

		virtual void
		doWakeup();

		// Log output of association between Filter and Functional Unit
		void
		printFilterAssociation();

		struct Friends {
		} friends_;

		wns::logger::Logger logger_;

        bool mustAccept_;
	};
}}

#endif // NOT defined DLL_COMPOUNDSWITCH_COMPOUNDSWITCH_HPP
