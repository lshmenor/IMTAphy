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

#include <WNS/ldk/fun/Main.hpp>

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/CommandPool.hpp>
#include <WNS/ldk/CommandProxy.hpp>
#include <WNS/ldk/Group.hpp>
#include <WNS/ldk/LinkHandlerInterface.hpp>
#include <WNS/ldk/PyConfigCreator.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/StaticFactory.hpp>

#include <iostream>

using namespace ::wns::ldk;
using namespace ::wns::ldk::fun;

Main::Main(ILayer* _layer) :
        layer(_layer),
        proxy(new CommandProxy),
        fuMap(),
        linkHandler(NULL),
        logger("default","default")
{
    wns::pyconfig::Parser pyco;
    pyco.loadString("import openwns.logger\n"
            "class LinkHandler:\n"
            "  type = \"wns.ldk.SimpleLinkHandler\"\n"
            "  isAcceptingLogger = openwns.logger.Logger(\"WNS\", \"LinkHandler\", False)\n"
            "  isAcceptingLogger.level = 3\n"
            "  sendDataLogger = openwns.logger.Logger(\"WNS\", \"LinkHandler\", False)\n"
            "  sendDataLogger.level = 3\n"
            "  wakeupLogger = openwns.logger.Logger(\"WNS\", \"LinkHandler\", False)\n"
            "  wakeupLogger.level = 3\n"
            "  onDataLogger = openwns.logger.Logger(\"WNS\", \"LinkHandler\", False)\n"
            "  onDataLogger.level = 3\n"
            "  traceCompoundJourney = False\n"
            "linkHandler = LinkHandler()\n");
    wns::pyconfig::View view(pyco, "linkHandler");

    linkHandler = wns::StaticFactory< wns::ldk::PyConfigCreator<LinkHandlerInterface> >
        ::creator(view.get<std::string>("type"))->create(view);
} // Main


Main::Main(ILayer* _layer, const wns::pyconfig::View& _config) :
        layer(_layer),
        proxy(new CommandProxy(_config.get("fun.commandProxy"))),
        fuMap(),
        linkHandler(NULL),
        logger(_config.get("fun.logger"))
{
    wns::pyconfig::View view = _config.get<wns::pyconfig::View>("linkHandler");

    linkHandler = wns::StaticFactory< wns::ldk::PyConfigCreator<LinkHandlerInterface> >
        ::creator(view.get<std::string>("type"))
        ->create(view);
} // Main

void
Main::onFUNCreated(wns::logger::Logger* logger)
{
    for(FunctionalUnitMap::iterator it = fuMap.begin();
        it != fuMap.end();
        ++it)
    {
        if (logger)
            MESSAGE_SINGLE(NORMAL, (*logger), "Post-FUN-Creation for:" << it->second->getName());
        it->second->onFUNCreated();
    }
}

Main::~Main()
{
    for(FunctionalUnitMap::iterator it = fuMap.begin();
        it != fuMap.end();
        ++it)
    {
        delete it->second;
    }
    delete linkHandler;
    delete proxy;
} // ~Main


CommandProxy*
Main::getProxy() const
{
    return this->proxy;
} // getProxy


void
Main::addFunctionalUnit(const std::string& commandName,
            const std::string& functionalUnitName,
            FunctionalUnit* functionalUnit)
{
    assure(functionalUnit, "tried to add invalid functionalUnit.");

    if(this->knowsFunctionalUnit(functionalUnitName))
        throw Exception("FunctionalUnit " + functionalUnitName + " already added.");

    functionalUnit->setName(functionalUnitName);

    this->fuMap[functionalUnitName] = functionalUnit;
    this->getProxy()->addFunctionalUnit(commandName, functionalUnit);

    MESSAGE_BEGIN(VERBOSE, logger, m, "");
    m << getName() << ": added FU '" << functionalUnit->getName() 
      << "', commandName: '" << commandName<< "', got PCIID: " << functionalUnit->getPCIID();
    MESSAGE_END();
} // addFunctionalUnit




void
Main::addFunctionalUnit(const std::string& name, FunctionalUnit* functionalUnit)
{
    // Backward compatibility Function, assuming that fuName and commandName
    // are equal
    this->addFunctionalUnit(name, name, functionalUnit);
}

void
Main::removeFunctionalUnit(const std::string& name)
{
    if(!this->knowsFunctionalUnit(name))
        throw Exception("FunctionalUnit " + name + " is not registered.");

    // remove command of FU from command pool
    this->getProxy()->removeFunctionalUnit(name);

    this->fuMap.erase(name);
} // removeFunctionalUnit


FunctionalUnit*
Main::getFunctionalUnit(const std::string& name) const
{
    FunctionalUnitMap::const_iterator it = this->fuMap.find(name);

    if(it == this->fuMap.end())
    {
        Exception e;
        e << "FunctionalUnit '" << name << "' not found in " << getName() << ".\n\n"
          << "Available FUs are: \n";

        for(FunctionalUnitMap::const_iterator iter = fuMap.begin();
            iter != fuMap.end();
            ++iter)
        {
            e << " * " << (iter->second)->getName() << "\n";
        }

        throw e;
    }

    return it->second;
} // getFunctionalUnit


bool
Main::knowsFunctionalUnit(const std::string& name) const
{
    return this->fuMap.find(name) != this->fuMap.end();
} // knowsFunctionalUnit


void
Main::connectFunctionalUnit(const std::string& upperName, const std::string& lowerName,
                            const std::string& srcPort, const std::string& dstPort)

{
    FunctionalUnit* upper = this->getFunctionalUnit(upperName);
    FunctionalUnit* lower = this->getFunctionalUnit(lowerName);

        upper->connect(lower, srcPort, dstPort);
} // connectFunctionalUnit


void
Main::upConnectFunctionalUnit(const std::string& upperName, const std::string& lowerName,
                              const std::string& srcPort, const std::string& dstPort)
{
    FunctionalUnit* upper = this->getFunctionalUnit(upperName);
    FunctionalUnit* lower = this->getFunctionalUnit(lowerName);

        upper->upConnect(lower, srcPort, dstPort);
} // connectFunctionalUnit


void
Main::downConnectFunctionalUnit(const std::string& upperName, const std::string& lowerName,
                                const std::string& srcPort, const std::string& dstPort)
{
    FunctionalUnit* upper = this->getFunctionalUnit(upperName);
    FunctionalUnit* lower = this->getFunctionalUnit(lowerName);

        upper->downConnect(lower, srcPort, dstPort);
} // connectFunctionalUnit

/*
void
Main::reconfigureFUN(const wns::pyconfig::View& reconfig)
{
    // pointer to FU to be replaced
    std::string oldFUName = reconfig.get<std::string>("replaceFU");
    FunctionalUnit* oldFU = getFunctionalUnit(oldFUName);

    // create new FU
    wns::pyconfig::View FUconfig(reconfig, "newFUConfig");
    std::string pluginName = FUconfig.get<std::string>("__plugin__");
    FunctionalUnit* newFU = FunctionalUnitFactory::creator(pluginName)
        ->create(oldFU->getFUN(), FUconfig);
    addFunctionalUnit(reconfig.get<std::string>("newFUName"), newFU);

    // remove command of old FU from command pool
    getProxy()->removeFunctionalUnit(oldFUName);

    // if old FU contains a SubFUN remove all commands of FUs inside the
    // SubFUN from command pool
    Group* groupFU = dynamic_cast<Group*>(oldFU);
    if (groupFU)
        groupFU->getSubFUN()->removeFUsFromCommandPool();

    // change links of upper connectors, lower deliverers and lower receptors
    // to point to new FU
    for(FunctionalUnitMap::iterator it = fuMap.begin();
        it != fuMap.end();
        ++it)
    {
        // upper connector links
        Link::ExchangeContainer connectorLink = it->second->getConnector()->get();
        for (unsigned int i = 0; i < connectorLink.size(); ++i)
        {
            if (connectorLink[i] == oldFU)
            {
                connectorLink[i] = newFU;
                it->second->getConnector()->set(connectorLink);
                break;
            }
        }

        // lower deliverer links
        Link::ExchangeContainer delivererLink = it->second->getDeliverer()->get();
        for (unsigned int i = 0; i < delivererLink.size(); ++i)
        {
            if (delivererLink[i] == oldFU)
            {
                delivererLink[i] = newFU;
                it->second->getDeliverer()->set(delivererLink);
                break;
            }
        }

        // lower receptor links
        Link::ExchangeContainer receptorLink = it->second->getReceptor()->get();
        for (unsigned int i = 0; i < receptorLink.size(); ++i)
        {
            if (receptorLink[i] == oldFU)
            {
                receptorLink[i] = newFU;
                it->second->getReceptor()->set(receptorLink);
                break;
            }
        }
    }
    // copy upper receptor link set from old FU to new FU
    newFU->getReceptor()->set(oldFU->getReceptor()->get());

    // copy upper deliverer link set from old FU to new FU
    newFU->getDeliverer()->set(oldFU->getDeliverer()->get());

    // copy lower connector link set from old FU to new FU
    newFU->getConnector()->set(oldFU->getConnector()->get());

    // delete old FU
    fuMap.erase(oldFUName);
    delete oldFU;

    // find new friends
    onFUNCreated();
} // reconfigureFUN
*/

//
// Layer delegations
//


ILayer*
Main::getLayer() const
{
    return this->layer;
} // getLayer


std::string
Main::getName() const
{
    return this->getLayer()->getName();
} // getName

LinkHandlerInterface*
Main::getLinkHandler() const
{
    return linkHandler;
}

CommandReaderInterface*
Main::getCommandReader(const std::string& commandName) const
{
    return proxy->getCommandReader(commandName);
}

void
Main::onShutdown()
{
    FunctionalUnitMap::iterator it;
    for(it = fuMap.begin();
        it != fuMap.end();
        it++)
    {
        it->second->onShutdown();
    }
}

