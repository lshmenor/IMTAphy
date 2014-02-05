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

#include <WNS/ldk/fun/Sub.hpp>

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/CommandPool.hpp>
#include <WNS/ldk/CommandProxy.hpp>
#include <WNS/ldk/LinkHandlerInterface.hpp>

using namespace ::wns::ldk;
using namespace ::wns::ldk::fun;


Sub::Sub(fun::FUN* fuNet) :
        parent(fuNet),
        layer(parent->getLayer()),
        proxy(parent->getProxy()),
        fuMap(),
        linkHandler(parent->getLinkHandler()),
        nameParentFU("None")
{
} // Sub


void
Sub::onFUNCreated()
{
    for(FunctionalUnitMap::iterator it = fuMap.begin();
        it != fuMap.end(); ++it)
    {
        it->second->onFUNCreated();
    }
}


Sub::~Sub()
{
        for(FunctionalUnitMap::iterator it = fuMap.begin();
            it != fuMap.end(); ++it)
            delete it->second;
} // ~Sub


CommandProxy*
Sub::getProxy() const
{
        return this->proxy;
} // getProxy


void
Sub::addFunctionalUnit(const std::string& commandName,
               const std::string& functionalUnitName,
               FunctionalUnit* functionalUnit)
{
    assure(functionalUnit, "tried to add invalid functionalUnit.");

        if(this->_knowsFunctionalUnit(functionalUnitName))
                throw Exception("FunctionalUnit " + functionalUnitName + " already added.");

        functionalUnit->setName(functionalUnitName);

        this->fuMap[functionalUnitName] = functionalUnit;
        this->getProxy()->addFunctionalUnit(commandName, functionalUnit);
} // addFunctionalUnit


void
Sub::addFunctionalUnit(const std::string& name, FunctionalUnit* functionalUnit)
{
        assure(functionalUnit, "tried to add invalid functionalUnit.");

        if(this->_knowsFunctionalUnit(name))
                throw Exception("FunctionalUnit " + name + " already added.");

        functionalUnit->setName(name);

        this->fuMap[name] = functionalUnit;
        this->getProxy()->addFunctionalUnit(name, functionalUnit);
} // addFunctionalUnit


void
Sub::removeFunctionalUnit(const std::string& name)
{
    if(!this->knowsFunctionalUnit(name))
        throw Exception("FunctionalUnit " + name + " is not registered.");

    // remove command of FU from command pool
    this->getProxy()->removeFunctionalUnit(name);

    this->fuMap.erase(name);
} // removeFunctionalUnit


FunctionalUnit*
Sub::getFunctionalUnit(const std::string& name) const
{
    FunctionalUnitMap::const_iterator it = this->fuMap.find(name);

    if(it != this->fuMap.end())
        return it->second;

    return parent->getFunctionalUnit(name);
} // getFunctionalUnit


bool
Sub::_knowsFunctionalUnit(const std::string& name) const
{
    FunctionalUnitMap::const_iterator it = this->fuMap.find(name);

    return it != this->fuMap.end();
} // _knowsFunctionalUnit


bool
Sub::knowsFunctionalUnit(const std::string& name) const
{
    if(_knowsFunctionalUnit(name))
        return true;

    return parent->knowsFunctionalUnit(name);
} // knowsFunctionalUnit


void
Sub::connectFunctionalUnit(const std::string& upperName, const std::string& lowerName,
                           const std::string& srcPort, const std::string& dstPort)
{
        FunctionalUnit* upper = this->getFunctionalUnit(upperName);
        FunctionalUnit* lower = this->getFunctionalUnit(lowerName);

        upper->connect(lower, srcPort, dstPort);
} // connectFunctionalUnit


void
Sub::upConnectFunctionalUnit(const std::string& upperName, const std::string& lowerName,
                             const std::string& srcPort, const std::string& dstPort)
{
        FunctionalUnit* upper = this->getFunctionalUnit(upperName);
        FunctionalUnit* lower = this->getFunctionalUnit(lowerName);

        upper->upConnect(lower, srcPort, dstPort);
} // connectFunctionalUnit


void
Sub::downConnectFunctionalUnit(const std::string& upperName, const std::string& lowerName,
                               const std::string& srcPort, const std::string& dstPort)
{
        FunctionalUnit* upper = this->getFunctionalUnit(upperName);
        FunctionalUnit* lower = this->getFunctionalUnit(lowerName);

        upper->downConnect(lower, srcPort, dstPort);
} // connectFunctionalUnit

/*
void
Sub::reconfigureFUN(const wns::pyconfig::View&)
{
	throw wns::Exception("Reconfiguration of SubFUNs is not supported yet.");
} // reconfigureFUN
*/

void
Sub::removeFUsFromCommandPool()
{
    for(FunctionalUnitMap::iterator it = fuMap.begin();
        it != fuMap.end(); ++it)
    {
        getProxy()->removeFunctionalUnit(it->first);
    }
} // removeFUsFromCommandPool

/*
template <RECEPTACLETYPE>
void
translateLink(Link<RECEPTACLETYPE>* link,
                          const std::map<RECEPTACLETYPE*,RECEPTACLETYPE*>& translate)
{
    Link::ExchangeContainer src = link->get();
    Link::ExchangeContainer dst;

    for(Link::ExchangeContainer::iterator linkDestination = src.begin();
        linkDestination != src.end();
        ++linkDestination) {

        TranslationMap::const_iterator newLinkDestination = translate.find(*linkDestination);
        if(newLinkDestination != translate.end()) {
            dst.push_back(newLinkDestination->second);
        }
    }

    link->set(dst);
} // translateLink


Sub*
Sub::clone() const
{
    fun::Sub* sub = new Sub(parent);

    for(FunctionalUnitMap::const_iterator it = fuMap.begin();
        it != fuMap.end();
        ++it)
    {

        FunctionalUnit* other = dynamic_cast<FunctionalUnit*>(it->second->clone());
        sub->fuMap[it->first] = other;
        translate[it->second] = other;
    }

    for(FunctionalUnitMap::const_iterator it = sub->fuMap.begin();
        it != sub->fuMap.end();
        ++it)
    {
        translateLink(it->second->getReceptor(), translate);
        translateLink(it->second->getConnector(), translate);
        translateLink(it->second->getDeliverer(), translate);
    }

    return sub;
} // clone
*/

//
// Layer delegations
//


ILayer*
Sub::getLayer() const
{
    return this->layer;
} // getLayer


std::string
Sub::getName() const
{
    return this->getLayer()->getName();
} // getName

void
Sub::setNameParentFU(std::string _name)
{
    nameParentFU = _name;
}


std::string
Sub::getNameParentFU() const
{
    return nameParentFU;
}


LinkHandlerInterface*
Sub::getLinkHandler() const
{
    return linkHandler;
}

CommandReaderInterface*
Sub::getCommandReader(const std::string& commandName) const
{
    return proxy->getCommandReader(commandName);
}

void
Sub::onShutdown()
{
    FunctionalUnitMap::iterator it;
    for(it = fuMap.begin();
        it != fuMap.end();
        it++)
    {
        it->second->onShutdown();
    }
}
