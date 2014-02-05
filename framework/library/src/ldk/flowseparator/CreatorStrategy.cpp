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

#include <WNS/ldk/flowseparator/CreatorStrategy.hpp>
#include <WNS/ldk/FUNConfigCreator.hpp>
#include <WNS/ldk/fun/FUN.hpp>

using namespace wns::ldk::flowseparator;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ConfigCreator,
    CreatorStrategy,
    "configcreator",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    PrototypeCreator,
    CreatorStrategy,
    "prototypecreator",
    FUNConfigCreator);


PrototypeCreator::PrototypeCreator(fun::FUN* fuNet, const pyconfig::View& config)
{
    pyconfig::View layerConfig(config, "prototypeConfig");
    std::string prototypeName = layerConfig.get<std::string>("__plugin__");
    prototype =	FunctionalUnitFactory::creator(prototypeName)->create(fuNet, layerConfig);

    std::string commandName = config.get<std::string>("name");
    std::string fuName = config.get<std::string>("fuName");
    fuNet->addFunctionalUnit(commandName, fuName, prototype);
}


PrototypeCreator::PrototypeCreator(fun::FUN* fuNet, std::string name, FunctionalUnit* _prototype) :
    prototype(_prototype)
{
    fuNet->addFunctionalUnit(name, prototype);
}


FunctionalUnit*
PrototypeCreator::create() const
{
    return dynamic_cast<FunctionalUnit*>(prototype->clone());
}


FunctionalUnit*
PrototypeCreator::createPrototype() const
{
    return dynamic_cast<FunctionalUnit*>(prototype->clone());
}


ConfigCreator::ConfigCreator(fun::FUN* _fun, const pyconfig::View& _config) :
    fun(_fun),
    config(_config.get("prototypeConfig")),
    creatorName(config.get<std::string>("__plugin__")),
    commandName(_config.get<std::string>("name"))
{
    // TODO: rename logger such that the index can be seen, e.g.
    // (  0.0187200) [  WNS] BS1.L2.BufferSep                                  BS1: add Instance/Flow      Key: UT address: 3, BS address: 1;       FU: None
    // (  0.0187521) [WinPr] BS1.L2.PriorityBuffer[BS=1,UT=3]
    FunctionalUnit* prototype = FunctionalUnitFactory::creator(creatorName)->create(fun, config);
    std::string fuName = _config.get<std::string>("fuName");
    fun->addFunctionalUnit(commandName, fuName, prototype);
}

FunctionalUnit*
ConfigCreator::create() const
{
    FunctionalUnit* fu = FunctionalUnitFactory::creator(creatorName)->create(fun, config);
    fun->getProxy()->addFunctionalUnit(commandName, fu);
    fu->onFUNCreated();
    return fu;
}

FunctionalUnit*
ConfigCreator::createPrototype() const
{
    FunctionalUnit* fu = FunctionalUnitFactory::creator(creatorName)->create(fun, config);
    return fu;
}
