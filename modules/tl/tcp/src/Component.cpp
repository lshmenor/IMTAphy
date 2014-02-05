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

#include <TCP/Component.hpp>
#include <TCP/Service.hpp>
#include <TCP/LowerConvergence.hpp>
#include <TCP/IPDataHandler.hpp>
#include <TCP/UpperConvergence.hpp>

#include <WNS/ldk/utils.hpp>
#include <WNS/ldk/Group.hpp>

#include <WNS/ldk/ControlServiceInterface.hpp>
#include <WNS/service/dll/FlowEstablishmentAndRelease.hpp>
#include <WNS/service/dll/Handler.hpp>

using namespace tcp;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Component,
    wns::node::component::Interface,
    "tcp.Component",
    wns::node::component::ConfigCreator
    );

Component::Component(
    wns::node::Interface* node,
    const wns::pyconfig::View& config) :

    wns::node::component::Component(node, config),
    fun(NULL),
    subFUN(NULL),
    tlService(NULL),
    upperConvergence(NULL),
    ipDataHandler(new IPDataHandler()),
    lowerConvergence(NULL),
    ipDataTransmission(NULL),
    flowSeparator(NULL),
    handshakeStrategy(NULL),
    logger(config.get<wns::pyconfig::View>("logger")),
    tcpFlowHandler(NULL),
    fear(NULL),
    dllNotification(NULL)
{
} // Component


void
Component::doStartup()
{
    wns::pyconfig::View config = this->getConfig();

    // build FUN
    fun = new wns::ldk::fun::Main(this);
    // build Sub-FUN
    subFUN = new wns::ldk::fun::Sub(fun);

    wns::ldk::configureFUN(fun, config.get<wns::pyconfig::View>("fun"));

    lowerConvergence = dynamic_cast<LowerConvergence*>(
        fun->getFunctionalUnit(
            getConfig().get<std::string>("lowerConvergence.commandName")));

    ipDataTransmission = getService<wns::service::nl::Service*>(config.get<std::string>("ipDataTransmission"));

    // configure ipDataHandler with Dispatcher from FUN
    ipDataHandler->setLowerConvergence(lowerConvergence);

    upperConvergence = dynamic_cast<UpperConvergence*>(
        fun->getFunctionalUnit(
            getConfig().get<std::string>("upperConvergence.commandName")));

    flowSeparator = dynamic_cast<wns::ldk::FlowSeparator*>(
        fun->getFunctionalUnit(
            getConfig().get<std::string>("flowSeparator.commandName")));

    wns::ldk::ControlServiceRegistry* csr = NULL;

    // DLL FlowHandling:
    // if FlowEstablishmentAndRelease is being used.

    tcpFlowHandler = new tcp::FlowHandler();

    if (!config.isNone("flowEstablishmentAndRelease"))
    {
        fear = getService<wns::service::dll::FlowEstablishmentAndRelease*> (config.get<std::string>("flowEstablishmentAndRelease"));
    }
    if (!config.isNone("dllNotification"))
    {
        dllNotification = getService<wns::service::dll::Notification*> (config.get<std::string>("dllNotification"));
        dllNotification->registerFlowHandler(tcpFlowHandler);
    }

    tlService = new Service(upperConvergence, lowerConvergence, flowSeparator, config.get<wns::pyconfig::View>("serviceConfig"),tcpFlowHandler, fear,  csr);
    addService(getConfig().get<std::string>("service"), tlService);
    fun->getLayer()->addControlService("fip", tlService);

    tcpFlowHandler->setTLService(tlService);
    lowerConvergence->setTLService(tlService);

    subFUN->onFUNCreated();
    MESSAGE_SINGLE(NORMAL, logger, "Constructed subFUN");

    fun->onFUNCreated();
    MESSAGE_SINGLE(NORMAL, logger, "Constructed FUN");
}


Component::~Component()
{
    delete ipDataHandler;
    delete fun;
    delete subFUN;
} // ~Component

void
Component::onNodeCreated()
{
    MESSAGE_SINGLE(NORMAL, logger, fun->getName() << ": onNodeCreated(), connecting to IP");

    //tcp service needs to know the ip address for setting the flow id
    tlService->setDataTransmissionService(
        getService<wns::service::nl::Service*>(
            getConfig().get<std::string>("ipDataTransmission")));

    lowerConvergence->setDataTransmissionService(getService<wns::service::nl::Service*>(
                                                     getConfig().get<std::string>("ipDataTransmission")));

    wns::service::nl::protocolNumber protocolNr = stringToProtocolNumber(getConfig().get<std::string>("protocolNumber"));

    lowerConvergence->setProtocolNumber(protocolNr);
    getService<wns::service::nl::Notification*>(
        getConfig().get<std::string>("ipNotification"))->registerHandler(protocolNr, ipDataHandler);

}

void
Component::onWorldCreated()
{
    tlService->setDNSService(
        getService<wns::service::nl::DNSService*>(
            getConfig().get<std::string>("dnsService")));
}

void
Component::onShutdown()
{
}

wns::service::nl::protocolNumber
Component::stringToProtocolNumber(std::string _protNr)
{
    if (_protNr == "TCP")
    {
        return wns::service::nl::TCP;
    }
    else if (_protNr == "UDP")
    {
        return wns::service::nl::UDP;
    }
    else
    {
        assure(false, "Unknown protocol type");
    }

    return wns::service::nl::TCP;
}
