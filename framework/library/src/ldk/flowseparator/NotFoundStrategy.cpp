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

#include <WNS/ldk/flowseparator/NotFoundStrategy.hpp>
#include <WNS/ldk/flowseparator/CreatorStrategy.hpp>
#include <WNS/ldk/flowseparator/FlowInfoProvider.hpp>

#include <WNS/ldk/tools/FakeFU.hpp>
#include <WNS/ldk/FUNConfigCreator.hpp>
#include <WNS/ldk/Layer.hpp>

using namespace wns::ldk::flowseparator;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Complain,
    NotFoundStrategy,
    "complain",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    CreateOnFirstCompound,
    NotFoundStrategy,
    "createonfirstcompound",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    CreateOnValidFlow,
    NotFoundStrategy,
    "createonvalidflow",
    FUNConfigCreator);

NotFoundStrategy::~NotFoundStrategy()
{
}


Complain::Complain(fun::FUN* /* fuNet */, const pyconfig::View& /* config */)
{
}


Complain::Complain()
{
}


FunctionalUnit*
Complain::ifNotFound(const ConstKeyPtr& /* key */) const
{
    throw wns::Exception("flowseparator::Complain: no matching instance!");
}

FunctionalUnit*
Complain::createPrototype() const
{
    return new wns::ldk::tools::FakeFU();
}

void
Complain::onFUNCreated()
{
}

CreateOnFirstCompound::CreateOnFirstCompound(fun::FUN* fuNet, const pyconfig::View& config) :
    creator()
{
    pyconfig::View creatorConfig(config, "creator");
    std::string creatorName = creatorConfig.get<std::string>("__plugin__");
    creator = std::auto_ptr<CreatorStrategy>(
        flowseparator::CreatorStrategyFactory::creator(creatorName)->create(fuNet, creatorConfig));
}

CreateOnFirstCompound::CreateOnFirstCompound(CreatorStrategy* _creator) :
    creator(_creator)
{
}

CreateOnFirstCompound::~CreateOnFirstCompound()
{
}

FunctionalUnit*
CreateOnFirstCompound::ifNotFound(const ConstKeyPtr& /* key */) const
{
    return creator->create();
}

FunctionalUnit*
CreateOnFirstCompound::createPrototype() const
{
    return creator->createPrototype();
}

void
CreateOnFirstCompound::onFUNCreated()
{
}


CreateOnValidFlow::CreateOnValidFlow(fun::FUN* fuNet, const pyconfig::View& config) :
    fun(fuNet),
    creator(),
    flowInfo(NULL),
    flowInfoProviderName(config.get<std::string>("flowInfoProviderName"))
{
    pyconfig::View creatorConfig(config, "creator");
    std::string creatorName = creatorConfig.get<std::string>("__plugin__");
    creator = std::auto_ptr<CreatorStrategy>(
        flowseparator::CreatorStrategyFactory::creator(creatorName)->create(fuNet, creatorConfig));
}

CreateOnValidFlow::CreateOnValidFlow(CreatorStrategy* _creator, FlowInfoProvider* _flowInfo) :
    fun(NULL),
    creator(_creator),
    flowInfo(_flowInfo),
    flowInfoProviderName("")
{
}

CreateOnValidFlow::~CreateOnValidFlow()
{
}

FunctionalUnit*
CreateOnValidFlow::ifNotFound(const ConstKeyPtr& key) const
{
    if ( flowInfo->isValidFlow(key) )
    {
        return creator->create();
    }
    else
    {
        std::string error = "flowseparator::CreateOnValidFlow: Can't create FU for this flow: " + key->str() + ". FlowInfoProvider says flow is not valid.";
        throw wns::Exception(error);
    }
}

FunctionalUnit*
CreateOnValidFlow::createPrototype() const
{
    return creator->createPrototype();
}

void
CreateOnValidFlow::onFUNCreated()
{
    assure(fun != NULL, "flowseparator::CreateOnValidFlow: invalid FUN");

    // set pointer to FlowInfoProvider
    flowInfo = fun->getLayer()->getControlService<FlowInfoProvider>(flowInfoProviderName);
}

