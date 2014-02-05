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

#ifndef WNS_PROBE_BUS_COMMANDCONTEXTPROVIDER_HPP
#define WNS_PROBE_BUS_COMMANDCONTEXTPROVIDER_HPP

#include <WNS/probe/bus/CompoundContextProvider.hpp>
#include <WNS/ldk/CommandReaderInterface.hpp>
#include <WNS/ldk/fun/FUN.hpp>

namespace wns { namespace probe { namespace bus {

    /** @brief Base class for all ContextProviders which use a specific command in
	 * the compound to determine the context
	 * input variables:
	 *   commandName: the name of the command where the information is readable
     *   specificKey: the name of the key which allows context-filtering
	 *                afterwards. Use a key starting with 'MAC.Compound' so that
	 *                it is clear that only compound-based probes (e.g. Packet)
	 *                can use this filter
	 */
    template <class COMMANDTYPE>
    class CommandContextProvider:
        virtual public CompoundContextProvider
    {
    public:
        CommandContextProvider(wns::ldk::fun::FUN* fun, std::string commandName, std::string specificKey):
            commandReader(fun->getCommandReader(commandName)),
            key(specificKey)
            {}

        virtual
        ~CommandContextProvider() {}

        virtual const std::string&
        getKey() const
            {
                return this->key;
            }
    protected:
        wns::ldk::CommandReaderInterface* commandReader;
        const std::string key;
    private:
        virtual void
        doVisitCommand(wns::probe::bus::IContext& c, const COMMANDTYPE* command) const = 0;
        virtual void
        doVisit(wns::probe::bus::IContext& c, const wns::ldk::CompoundPtr& compound) const
            {
                assure(compound, "Received NULL CompoundPtr");
                if(commandReader->commandIsActivated(compound->getCommandPool()))
                {
                    COMMANDTYPE* command = commandReader->readCommand<COMMANDTYPE>(compound->getCommandPool());
                    this->doVisitCommand(c, command);
                }
            };
    };

}}}

#endif // NOT defined WNS_PROBE_BUS_COMMANDCONTEXTPROVIDER_HPP


