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

#include <WNS/ldk/utils.hpp>

using namespace wns;
using namespace wns::ldk;


FunctionalUnit*
wns::ldk::configuredFunctionalUnit(fun::FUN* fuNet, const pyconfig::View& config)
{
    std::string pluginName = config.get<std::string>("__plugin__");

    wns::pyconfig::View view = config;

    FunctionalUnit* functionalUnit = FunctionalUnitFactory::creator(pluginName)
        ->create(fuNet, view);

    return functionalUnit;
} // configuredFunctionalUnit


void
wns::ldk::configureFUN(fun::FUN* fuNet, const pyconfig::View& config)
{
    int nFunctionalUnits = config.len("functionalUnit");
    for(int ii = 0; ii < nFunctionalUnits; ++ii) {
        pyconfig::View nodeConfig(config, "functionalUnit", ii);
        std::string commandName = nodeConfig.get<std::string>("commandName");
        std::string functionalUnitName;
        if (!nodeConfig.isNone("functionalUnitName"))
            functionalUnitName = nodeConfig.get<std::string>("functionalUnitName");
        else
            functionalUnitName = commandName;
        if (nodeConfig.knows("config")){
            // Old-school config using wns.FUN.Node
            fuNet->addFunctionalUnit(commandName,
                                     functionalUnitName,
                                     configuredFunctionalUnit(fuNet, nodeConfig.get("config")));
        } else {
            // new config using FU config derived from wns.FUN.FunctionalUnit
            fuNet->addFunctionalUnit(commandName,
                                     functionalUnitName,
                                     configuredFunctionalUnit(fuNet, nodeConfig));
        }
    }

    // this is part of the connect order preserving quick hack.
    // order of connects should not matter, but here it was the simplest
    // way to get Dispatcher/FrameDispatcher running.
    int nConnections = config.len("connects");
    for(int ii = 0; ii < nConnections; ++ii) {
        pyconfig::View connection(config, "connects", ii);

        int type = connection.get<int>("type");
        std::string src;
        if (!connection.isNone("src.functionalUnitName"))
            src = connection.get<std::string>("src.functionalUnitName");
        else
            src = connection.get<std::string>("src.commandName");

        std::string srcPort = connection.get<std::string>("srcPort.name");

        std::string dst;
        if (!connection.isNone("dst.functionalUnitName"))
            dst = connection.get<std::string>("dst.functionalUnitName");
        else
            dst = connection.get<std::string>("dst.commandName");

        std::string dstPort = connection.get<std::string>("dstPort.name");

        switch(type) {
        case 0:
            fuNet->connectFunctionalUnit(src, dst, srcPort, dstPort);
            break;
        case 1:
            fuNet->downConnectFunctionalUnit(src, dst, srcPort, dstPort);
            break;
        case 2:
            fuNet->upConnectFunctionalUnit(src, dst, srcPort, dstPort);
            break;
        default:
            assure(false, "invalid connection type");
            break;
        }
    }
} // configureFUN

