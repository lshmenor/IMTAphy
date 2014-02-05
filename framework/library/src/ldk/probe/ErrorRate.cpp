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

#include <WNS/ldk/probe/ErrorRate.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Layer.hpp>

using namespace wns::ldk;
using namespace wns::ldk::probe;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ErrorRate,
    Probe,
    "wns.probe.ErrorRate",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ErrorRate,
    FunctionalUnit,
    "wns.probe.ErrorRate",
    FUNConfigCreator);

ErrorRate::ErrorRate(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    fu::Plain<ErrorRate>(fuNet),
    Forwarding<ErrorRate>(),
    errorRateProviderName(config.get<std::string>("errorRateProvider")),
    logger(config.get("logger"))
{
    // this is for the new probe bus
    wns::probe::bus::ContextProviderCollection* cpcParent = &fuNet->getLayer()->getContextProviderCollection();

    wns::probe::bus::ContextProviderCollection cpc(cpcParent);

    probe = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(cpc, config.get<std::string>("probeName")));
} // ErrorRate

ErrorRate::~ErrorRate()
{
}

void
ErrorRate::onFUNCreated()
{
    friends.errorRateProvider = getFUN()->findFriend<FunctionalUnit*>(errorRateProviderName);
    assure(
        friends.errorRateProvider,
        "ErrorRate probe requires a ErrorRateProvider friend with name '" + errorRateProviderName + "'");
}

void
ErrorRate::processOutgoing(const CompoundPtr& compound)
{
    // nothing to do here, simply forward the PDU
    Forwarding<ErrorRate>::processOutgoing(compound);
}
    // processOutgoing


void
ErrorRate::processIncoming(const CompoundPtr& compound)
{
    ErrorRateProviderInterface* ppi = dynamic_cast<ErrorRateProviderInterface*>(
        friends.errorRateProvider->getCommand(compound->getCommandPool()));

    assure(ppi, "Expected a ErrorRateProviderInterface instance.");

    probe->put(ppi->getErrorRate());

    Forwarding<ErrorRate>::processIncoming(compound);
}
    // processIncoming



