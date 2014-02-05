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

#include <WNS/ldk/Group.hpp>
#include <WNS/ldk/utils.hpp>
#include <WNS/ldk/Layer.hpp>

#include <string>

using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Group,
                                     FunctionalUnit,
                                     "wns.Group",
                                     FUNConfigCreator);

Group::Group(
    fun::FUN* fuNet,
    const pyconfig::View& _config) :

    CommandTypeSpecifier<>(fuNet),
    HasReceptor<>(),
    HasConnector<>(),
    HasDeliverer<>(),
    config(_config),
    logger("WNS", "Group")
{
    sub = new fun::Sub(getFUN());

    pyconfig::View funView(config, "fun");
    configureFUN(sub, funView);

    if (!config.isNone("top") && (config.len("topPorts") == 0))
    {
        FunctionalUnit* topFU = sub->getFunctionalUnit(config.get<std::string>("top"));

        updateConnectorReceptacleRegistry("SinglePort", topFU->getFromConnectorReceptacleRegistry("SinglePort"));
        updateDelivererRegistry("SinglePort", topFU->getFromDelivererRegistry("SinglePort"));
        updateReceptorRegistry("SinglePort", topFU->getFromReceptorRegistry("SinglePort"));
    }
    else if (config.isNone("top") && !(config.len("topPorts") == 0))
    {
        int numPorts = config.len("topPorts");
        for (int ii = 0; ii < numPorts; ++ii)
        {
            pyconfig::View portConfig(config, "topPorts", ii);

            FunctionalUnit* fu = sub->getFunctionalUnit(portConfig.get<std::string>("fuName"));
            std::string fuPort = portConfig.get<std::string>("fuPort.name");
            std::string groupPort = portConfig.get<std::string>("groupPort.name");

            addToConnectorReceptacleRegistry(groupPort, fu->getFromConnectorReceptacleRegistry(fuPort));
            addToDelivererRegistry(groupPort, fu->getFromDelivererRegistry(fuPort));
            addToReceptorRegistry(groupPort, fu->getFromReceptorRegistry(fuPort));
        }
    }
    else
    {
        wns::Exception e;
        e << "Misconfiguration of Group FU!";
        throw e;
    }

    if (!config.isNone("bottom") && (config.len("bottomPorts") == 0))
    {
        FunctionalUnit* bottomFU = sub->getFunctionalUnit(config.get<std::string>("bottom"));

        updateConnectorRegistry("SinglePort", bottomFU->getFromConnectorRegistry("SinglePort"));
        updateDelivererReceptacleRegistry("SinglePort", bottomFU->getFromDelivererReceptacleRegistry("SinglePort"));
        updateReceptorReceptacleRegistry("SinglePort", bottomFU->getFromReceptorReceptacleRegistry("SinglePort"));
    }
    else if (config.isNone("bottom") && !(config.len("bottomPorts") == 0))
    {
        int numPorts = config.len("bottomPorts");
        for (int ii = 0; ii < numPorts; ++ii)
        {
            pyconfig::View portConfig(config, "bottomPorts", ii);

            FunctionalUnit* fu = sub->getFunctionalUnit(portConfig.get<std::string>("fuName"));
            std::string fuPort = portConfig.get<std::string>("fuPort.name");
            std::string groupPort = portConfig.get<std::string>("groupPort.name");

            addToConnectorRegistry(groupPort, fu->getFromConnectorRegistry(fuPort));
            addToDelivererReceptacleRegistry(groupPort, fu->getFromDelivererReceptacleRegistry(fuPort));
            addToReceptorReceptacleRegistry(groupPort, fu->getFromReceptorReceptacleRegistry(fuPort));
        }
    }
    else
    {
        wns::Exception e;
        e << "Misconfiguration of Group FU!";
        throw e;
    }
}


Group::Group(const Group& other) :
    CompoundHandlerInterface<FunctionalUnit>(other),
    CommandTypeSpecifierInterface(other),
    HasConnectorInterface(other),
    HasReceptorInterface(other),
    HasDelivererInterface(other),
    CloneableInterface(other),
    IOutputStreamable(other),
    PythonicOutput(other),
    FunctionalUnit(other),
    HasReceptor<>(),
    HasConnector<>(),
    HasDeliverer<>(),
    CommandTypeSpecifier<>(other),
    Cloneable<Group>(other),

    config(other.config),
    logger("WNS", "Group")
{
    sub = new fun::Sub(getFUN());

    pyconfig::View funView(config, "fun");
    configureFUN(sub, funView);
    sub->onFUNCreated();

    if (!config.isNone("top") && (config.len("topPorts") == 0))
    {
        FunctionalUnit* topFU = sub->getFunctionalUnit(config.get<std::string>("top"));

        updateConnectorReceptacleRegistry("SinglePort", topFU->getFromConnectorReceptacleRegistry("SinglePort"));
        updateDelivererRegistry("SinglePort", topFU->getFromDelivererRegistry("SinglePort"));
        updateReceptorRegistry("SinglePort", topFU->getFromReceptorRegistry("SinglePort"));
    }
    else if (config.isNone("top") && !(config.len("topPorts") == 0))
    {
        int numPorts = config.len("topPorts");
        for (int ii = 0; ii < numPorts; ++ii)
        {
            pyconfig::View portConfig(config, "topPorts", ii);

            FunctionalUnit* fu = sub->getFunctionalUnit(portConfig.get<std::string>("fuName"));
            std::string fuPort = portConfig.get<std::string>("fuPort.name");
            std::string groupPort = portConfig.get<std::string>("groupPort.name");

            addToConnectorReceptacleRegistry(groupPort, fu->getFromConnectorReceptacleRegistry(fuPort));
            addToDelivererRegistry(groupPort, fu->getFromDelivererRegistry(fuPort));
            addToReceptorRegistry(groupPort, fu->getFromReceptorRegistry(fuPort));
        }
    }
    else
    {
        wns::Exception e;
        e << "Misconfiguration of Group FU!";
        throw e;
    }

    if (!config.isNone("bottom") && (config.len("bottomPorts") == 0))
    {
        FunctionalUnit* bottomFU = sub->getFunctionalUnit(config.get<std::string>("bottom"));

        updateConnectorRegistry("SinglePort", bottomFU->getFromConnectorRegistry("SinglePort"));
        updateDelivererReceptacleRegistry("SinglePort", bottomFU->getFromDelivererReceptacleRegistry("SinglePort"));
        updateReceptorReceptacleRegistry("SinglePort", bottomFU->getFromReceptorReceptacleRegistry("SinglePort"));
    }
    else if (config.isNone("bottom") && !(config.len("bottomPorts") == 0))
    {
        int numPorts = config.len("bottomPorts");
        for (int ii = 0; ii < numPorts; ++ii)
        {
            pyconfig::View portConfig(config, "bottomPorts", ii);

            FunctionalUnit* fu = sub->getFunctionalUnit(portConfig.get<std::string>("fuName"));
            std::string fuPort = portConfig.get<std::string>("fuPort.name");
            std::string groupPort = portConfig.get<std::string>("groupPort.name");

            addToConnectorRegistry(groupPort, fu->getFromConnectorRegistry(fuPort));
            addToDelivererReceptacleRegistry(groupPort, fu->getFromDelivererReceptacleRegistry(fuPort));
            addToReceptorReceptacleRegistry(groupPort, fu->getFromReceptorReceptacleRegistry(fuPort));
        }
    }
    else
    {
        wns::Exception e;
        e << "Misconfiguration of Group FU!";
        throw e;
    }
}


Group::~Group()
{
    delete sub;
}


void
Group::onFUNCreated()
{
    sub->onFUNCreated();
} // onFUNCreated


bool
Group::doIsAccepting(const CompoundPtr& compound) const
{
    wns::Exception e;
    e << "doIsAccepting(const CompoundPtr&) of Group FU must not be called!";
    throw e;
} // isAccepting


void
Group::doSendData(const CompoundPtr& compound)
{
    wns::Exception e;
    e << "doSendData(const CompoundPtr&) of Group FU must not be called!";
    throw e;
} // doSendData


void
Group::doOnData(const CompoundPtr& compound)
{
    wns::Exception e;
    e << "doOnData(const CompoundPtr&) of Group FU must not be called!";
    throw e;
} // doOnData


void
Group::doWakeup()
{
    wns::Exception e;
    e << "doWakeup() of Group FU must not be called!";
    throw e;
} // wakeup


fun::Sub*
Group::getSubFUN() const
{
    return sub;
} // getSubFUN


void
Group::setName(std::string _name)
{
    FunctionalUnit::setName(_name);
    getSubFUN()->setNameParentFU(_name);
}
