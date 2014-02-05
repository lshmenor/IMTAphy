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

#include <WNS/ldk/FlowSeparator.hpp>
#include <WNS/ldk/Layer.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/module/Base.hpp>

using namespace wns::ldk;
using namespace wns::ldk::flowseparator;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    FlowSeparator,
    FunctionalUnit,
    "wns.FlowSeparator",
    FUNConfigCreator
    );

FlowSeparator::ConnectorReceptacleSeparator::ConnectorReceptacleSeparator(FlowSeparator* fs, std::string portname)
    : ReceptacleManagement<IConnectorReceptacle>(fs),
      portname_(portname)
{}

FlowSeparator::ConnectorReceptacleSeparator::~ConnectorReceptacleSeparator()
{}

void
FlowSeparator::ConnectorReceptacleSeparator::sendData(const CompoundPtr& compound)
{
    getFU()->getFUN()->getLinkHandler()->sendData(this, compound);
}

void
FlowSeparator::ConnectorReceptacleSeparator::doSendData(const CompoundPtr& compound)
{
    IConnectorReceptacle* receptacle = tryGetInstanceAndInsertPermanent(compound, Direction::OUTGOING());
    receptacle->sendData(compound);
}

bool
FlowSeparator::ConnectorReceptacleSeparator::isAccepting(const CompoundPtr& compound)
{
    return getFU()->getFUN()->getLinkHandler()->isAccepting(this, compound);
}

bool
FlowSeparator::ConnectorReceptacleSeparator::doIsAccepting(const CompoundPtr& compound) const
{
    try
    {
        IConnectorReceptacle* candidate = _getInstance(compound, Direction::OUTGOING());
        // either no one may be busy, or the instance in question

    ConstKeyPtr key = (*(fs_->keyBuilder))(compound, Direction::OUTGOING());
    FunctionalUnit* candidateFU = fs_->instances[key];

    if (fs_->instanceBusy == NULL || fs_->instanceBusy == candidateFU)
        {
            return candidate->isAccepting(compound);
        }
        else
        {
            return false;
        }
    }
    catch(const InstanceNotFound& ifn)
    {
        MESSAGE_SINGLE(
            VERBOSE,
            fs_->logger,
            fs_->getFUN()->getName()<<": " <<
            "creating new instance for key (" <<
            ifn.key->str() <<
            ") temporarily.");
        FunctionalUnit* candidate = fs_->notFound->ifNotFound(ifn.key);
        fs_->connectFU(candidate);
        bool accepting = candidate->getFromConnectorReceptacleRegistry(portname_)->isAccepting(compound);
        delete candidate;
        return accepting;
    }
    // throw on
    catch(...)
    {
        throw;
    }
}

FunctionalUnit*
FlowSeparator::ConnectorReceptacleSeparator::getFU()
{
    return fs_;
}


FlowSeparator::DelivererReceptacleSeparator::DelivererReceptacleSeparator(FlowSeparator* fs)
    : ReceptacleManagement<IDelivererReceptacle>(fs)
{}

FlowSeparator::DelivererReceptacleSeparator::~DelivererReceptacleSeparator()
{}

void
FlowSeparator::DelivererReceptacleSeparator::onData(const CompoundPtr& compound)
{
    fs_->getFUN()->getLinkHandler()->onData(this, compound);
}

void
FlowSeparator::DelivererReceptacleSeparator::doOnData(const CompoundPtr& compound)
{
    IDelivererReceptacle* receptacle = tryGetInstanceAndInsertPermanent(compound, Direction::INCOMING());

    // Mark this as the "Busy" instance. 
    ConstKeyPtr key = (*(fs_->keyBuilder))(compound, Direction::INCOMING());
    fs_->instanceBusy = fs_->instances[key];
    receptacle->onData(compound);
    fs_->instanceBusy = NULL;
}

FunctionalUnit*
FlowSeparator::DelivererReceptacleSeparator::getFU()
{
    return fs_;
}


FlowSeparator::ReceptorReceptacleSeparator::ReceptorReceptacleSeparator(FlowSeparator* fs)
  : ReceptacleManagement<IReceptorReceptacle>(fs)
{}

FlowSeparator::ReceptorReceptacleSeparator::~ReceptorReceptacleSeparator()
{}

void
FlowSeparator::ReceptorReceptacleSeparator::wakeup()
{
    fs_->getFUN()->getLinkHandler()->wakeup(this);
}

void
FlowSeparator::ReceptorReceptacleSeparator::doWakeup()
{
    ReceptacleContainer::iterator begin = receptacleContainer_.begin();
    ReceptacleContainer::iterator somewhere = receptacleContainer_.begin();
    ReceptacleContainer::iterator end = receptacleContainer_.end();

    // set jump-in point somewhere
    size_t offset = size_t(fs_->dis() * receptacleContainer_.size());
    for( size_t i = 0; i < offset; i++)
        somewhere++;

    // and walk from somewhere to the end
    for(ReceptacleContainer::iterator i = somewhere;
        i != end; ++i)
    {
        fs_->instanceBusy = fs_->instances[(*i).first];
        (*i).second->wakeup();
        fs_->instanceBusy = NULL;
    }

    // walk from the beginning to the jump-in point somewhere
    for(ReceptacleContainer::iterator i = begin;
        i != somewhere; ++i)
    {
        fs_->instanceBusy = fs_->instances[(*i).first];
        (*i).second->wakeup();
        fs_->instanceBusy = NULL;
    }
}

FunctionalUnit*
FlowSeparator::ReceptorReceptacleSeparator::getFU()
{
    return fs_;
}


FlowSeparator::FlowSeparator(
    fun::FUN* fuNet,
    const pyconfig::View& _config,
    std::auto_ptr<KeyBuilder> _keyBuilder,
    std::auto_ptr<flowseparator::NotFoundStrategy> _notFound) :

    CommandTypeSpecifier<>(fuNet),
    HasReceptor<>(),
    HasConnector<>(),
    HasDeliverer<>(),
    NotCloneable(),
    instances(),
    crsList_(),
    drsList_(),
    rrsList_(),
    prototypeFU_(NULL),
    connectorReceptacleSinglePort_(NULL),
    delivererReceptacleSinglePort_(NULL),
    receptorReceptacleSinglePort_(NULL),
    instanceBusy(NULL),
    config(_config),
    keyBuilder(_keyBuilder),
    notFound(_notFound),
    dis(),
    logger(_config.getView("logger"))
{
    init();
} // FlowSeparator


FlowSeparator::FlowSeparator(
    fun::FUN* fuNet,
    const pyconfig::View& _config) :

    CommandTypeSpecifier<>(fuNet),
    HasReceptor<>(),
    HasConnector<>(),
    HasDeliverer<>(),
    NotCloneable(),
    instances(),
    crsList_(),
    drsList_(),
    rrsList_(),
    prototypeFU_(NULL),
    connectorReceptacleSinglePort_(NULL),
    delivererReceptacleSinglePort_(NULL),
    receptorReceptacleSinglePort_(NULL),
    instanceBusy(NULL),
    config(_config),
    keyBuilder(),
    notFound(),
    dis(),
    logger(_config.get("logger"))
{
    pyconfig::View notFoundConfig(config, "notFound");
    std::string notFoundName = notFoundConfig.get<std::string>("__plugin__");
    this->notFound = std::auto_ptr<flowseparator::NotFoundStrategy>(
        flowseparator::NotFoundStrategyFactory::creator(notFoundName)->create(fuNet, notFoundConfig));

    pyconfig::View keyBuilderConfig(config, "keyBuilder");
    std::string keyBuilderName = keyBuilderConfig.get<std::string>("__plugin__");
    this->keyBuilder = std::auto_ptr<KeyBuilder>(
        KeyBuilderFactory::creator(keyBuilderName)->create(fuNet, keyBuilderConfig));

    init();
} // FlowSeparator



FlowSeparator::FlowSeparator(const FlowSeparator& other)
    : wns::ldk::CommandTypeSpecifier<EmptyCommand>(other.getFUN()),
      config(other.config)
{
    std::cout << "Copy constructor called!" << std::endl;
}

void
FlowSeparator::init()
{
    // create prototype FU
    prototypeFU_ = this->notFound->createPrototype();

    StringList connectors = prototypeFU_->getKeysFromConnectorRegistry();
    StringList::iterator connectorsIter;
    for (connectorsIter = connectors.begin();
         connectorsIter != connectors.end();
         connectorsIter++)
    {
        if (*connectorsIter == "SinglePort")
        {
            getFromConnectorRegistry("SinglePort")->set(
                prototypeFU_->getFromConnectorRegistry("SinglePort")->get());
        }
        else
        {
            addToConnectorRegistry(*connectorsIter,
                                   prototypeFU_->getFromConnectorRegistry(*connectorsIter));
        }
    }

    StringList receptors = prototypeFU_->getKeysFromReceptorRegistry();
    StringList::iterator receptorsIter;
    for (receptorsIter = receptors.begin();
         receptorsIter != receptors.end();
         receptorsIter++)
    {
        if (*receptorsIter == "SinglePort")
        {
            getFromReceptorRegistry("SinglePort")->set(
                prototypeFU_->getFromReceptorRegistry("SinglePort")->get());
        }
        else
        {
            addToReceptorRegistry(*receptorsIter,
                                  prototypeFU_->getFromReceptorRegistry(*receptorsIter));
        }
    }

    StringList deliverers = prototypeFU_->getKeysFromDelivererRegistry();
    StringList::iterator deliverersIter;
    for (deliverersIter = deliverers.begin();
         deliverersIter != deliverers.end();
         deliverersIter++)
    {
        if (*deliverersIter == "SinglePort")
        {
            getFromDelivererRegistry("SinglePort")->set(
                prototypeFU_->getFromDelivererRegistry("SinglePort")->get());
        }
        else
        {
            addToDelivererRegistry(*deliverersIter,
                                   prototypeFU_->getFromDelivererRegistry(*deliverersIter));
        }
    }

    ConnectorReceptacleSeparator* crs;
    StringList connectorReceptacles = prototypeFU_->getKeysFromConnectorReceptacleRegistry();
    StringList::iterator connectorReceptaclesIter;
    for (connectorReceptaclesIter = connectorReceptacles.begin();
         connectorReceptaclesIter != connectorReceptacles.end();
         connectorReceptaclesIter++)
    {
        if (*connectorReceptaclesIter == "SinglePort")
        {
            crs = new ConnectorReceptacleSeparator(this, "SinglePort");
            updateConnectorReceptacleRegistry("SinglePort",
                                              crs);
            crsList_.push_back(crs);
        }
        else
        {
            crs = new ConnectorReceptacleSeparator(this, *connectorReceptaclesIter);
            addToConnectorReceptacleRegistry(*connectorReceptaclesIter,
                                             crs);
            crsList_.push_back(crs);
        }
    }

    if (crsList_.size() == 1)
    {
        connectorReceptacleSinglePort_ = crs;
    }

    DelivererReceptacleSeparator* drs;
    StringList delivererReceptacles = prototypeFU_->getKeysFromDelivererReceptacleRegistry();
    StringList::iterator delivererReceptaclesIter;
    for (delivererReceptaclesIter = delivererReceptacles.begin();
         delivererReceptaclesIter != delivererReceptacles.end();
         delivererReceptaclesIter++)
    {
        drs = new DelivererReceptacleSeparator(this);
        if (*delivererReceptaclesIter == "SinglePort")
        {
            updateDelivererReceptacleRegistry("SinglePort",
                                              drs);
        }
        else
        {
            addToDelivererReceptacleRegistry(*delivererReceptaclesIter,
                                             drs);
        }
        drsList_.push_back(drs);
    }

    if (drsList_.size() == 1)
    {
        delivererReceptacleSinglePort_ = drs;
    }

    ReceptorReceptacleSeparator *rrs;
    StringList receptorReceptacles = prototypeFU_->getKeysFromReceptorReceptacleRegistry();
    StringList::iterator receptorReceptaclesIter;
    for (receptorReceptaclesIter = receptorReceptacles.begin();
         receptorReceptaclesIter != receptorReceptacles.end();
         receptorReceptaclesIter++)
    {
        rrs = new ReceptorReceptacleSeparator(this);
        if (*receptorReceptaclesIter == "SinglePort")
        {
            updateReceptorReceptacleRegistry("SinglePort",
                                             rrs);
        }
        else
        {
            addToReceptorReceptacleRegistry(*receptorReceptaclesIter,
                                            rrs);
        }
        rrsList_.push_back(rrs);
    }

    if (rrsList_.size() == 1)
    {
        receptorReceptacleSinglePort_ = rrs;
    }
}


FlowSeparator::~FlowSeparator()
{
    assure(instanceBusy == NULL, "One FU in FlowSeparator is still busy. Can't delete FlowSeparator!");

    while(instances.empty() == false)
    {
        delete instances.begin()->second;
        instances.erase(instances.begin());
    }

    notFound.reset();
    keyBuilder.reset();

    while (crsList_.empty() == false)
    {
        delete *(crsList_.begin());
        crsList_.erase(crsList_.begin());
    }
    while (drsList_.empty() == false)
    {
        delete *(drsList_.begin());
        drsList_.erase(drsList_.begin());
    }
    while (rrsList_.empty() == false)
    {
        delete *(rrsList_.begin());
        rrsList_.erase(rrsList_.begin());
    }

    delete prototypeFU_;
} // ~FlowSeparator


void
FlowSeparator::onFUNCreated()
{
    /*
     * The prototype is registered at the FUN itself, so its onFUNCreated method
     * gets called by the FUN directly.
     * <br>
     * The keyBuilder itself may need to make friends, since the keys will usually
     * need the content of some Commands at instantiation time.
     * Resolving these dependencies at keyBuilder startup time instead during Key
     * instantiation pushes run-time complexity to configuration time.
     */
    keyBuilder->onFUNCreated();
    notFound->onFUNCreated();
} // onFUNCreated


void
FlowSeparator::doSendData(const CompoundPtr& compound)
{
    assureNotNull(connectorReceptacleSinglePort_);
    connectorReceptacleSinglePort_->sendData(compound);
} // doSendData


void
FlowSeparator::doOnData(const CompoundPtr& compound)
{
    assureNotNull(delivererReceptacleSinglePort_);
    delivererReceptacleSinglePort_->onData(compound);
} // doOnData


std::size_t
FlowSeparator::size() const
{
    return instances.size();
} // size


FunctionalUnit*
FlowSeparator::getInstance(const ConstKeyPtr& key) const
{
    InstanceMap::const_iterator it = instances.find(key);
    if(it == instances.end())
    {
        return NULL;
    }

    return it->second;
} // getInstance

FunctionalUnit*
FlowSeparator::getInstance(const CompoundPtr& compound, int direction) const
{
    return(this->getInstance(this->getKey(compound, direction)));
} // getInstance

ConstKeyPtr
FlowSeparator::getKey(const CompoundPtr& compound, int direction) const
{
    ConstKeyPtr key = (*keyBuilder)(compound, direction);
    return key;
}


void
FlowSeparator::addInstance(const ConstKeyPtr& key)
{
    FunctionalUnit* functionalUnit = notFound->ifNotFound(key);
    addInstance(key, functionalUnit);
}

void
FlowSeparator::addInstance(const ConstKeyPtr& key, FunctionalUnit* functionalUnit)
{
    assure(NULL == getInstance(key),
           "trying to add an instance for an already known key.");

    MESSAGE_BEGIN(NORMAL, logger, m, this->getFUN()->getName());
    m <<": add Instance/Flow      Key: "<< key->str()
      << ";       FU: " << functionalUnit->getName();
    MESSAGE_END();

    this->integrate(key, functionalUnit);
} // addInstance


void
FlowSeparator::removeInstance(const ConstKeyPtr& key)
{
    assure(NULL != getInstance(key),
           "trying to remove an instance using an unknown key.");

    MESSAGE_BEGIN(NORMAL, logger, m, this->getFUN()->getName());
    m <<": remove Instance/Flow      Key: " << key->str()
      << ";       FU: " << getInstance(key)->getName();
    MESSAGE_END();

    this->disintegrate(key);
} // removeInstance


bool
FlowSeparator::doIsAccepting(const CompoundPtr& compound) const
{
    assureNotNull(connectorReceptacleSinglePort_);
    connectorReceptacleSinglePort_->isAccepting(compound);
} // doIsAccepting


void
FlowSeparator::doWakeup()
{
    assureNotNull(receptorReceptacleSinglePort_);
    receptorReceptacleSinglePort_->wakeup();
} // doWakeup


void
FlowSeparator::connectFU(FunctionalUnit* functionalUnit) const
{
    /**
     * @todo If something changes in the layout of the FUN, it will not be
     * propagated to the FUs inside this FlowSep!
     */
    StringList connectors = getKeysFromConnectorRegistry();
    StringList::iterator connectorsIter;
    for (connectorsIter = connectors.begin();
         connectorsIter != connectors.end();
         connectorsIter++)
    {
        functionalUnit->getFromConnectorRegistry(*connectorsIter)->set(
            getFromConnectorRegistry(*connectorsIter)->get());
    }
    StringList receptors = getKeysFromReceptorRegistry();
    StringList::iterator receptorsIter;
    for (receptorsIter = receptors.begin();
         receptorsIter != receptors.end();
         receptorsIter++)
    {
        functionalUnit->getFromReceptorRegistry(*receptorsIter)->set(
            getFromReceptorRegistry(*receptorsIter)->get());
    }
    StringList deliverers = getKeysFromDelivererRegistry();
    StringList::iterator deliverersIter;
    for (deliverersIter = deliverers.begin();
         deliverersIter != deliverers.end();
         deliverersIter++)
    {
        functionalUnit->getFromDelivererRegistry(*deliverersIter)->set(
            getFromDelivererRegistry(*deliverersIter)->get());
    }
}


void
FlowSeparator::integrate(const ConstKeyPtr& key, FunctionalUnit* functionalUnit)
{
    connectFU(functionalUnit);

    StringList connectorReceptacles = prototypeFU_->getKeysFromConnectorReceptacleRegistry();
    StringList::iterator connectorReceptaclesIter;
    for (connectorReceptaclesIter = connectorReceptacles.begin();
         connectorReceptaclesIter != connectorReceptacles.end();
         connectorReceptaclesIter++)
    {
        (static_cast<ConnectorReceptacleSeparator*>(getFromConnectorReceptacleRegistry(*connectorReceptaclesIter)))
            ->addInstance(key, functionalUnit->getFromConnectorReceptacleRegistry(*connectorReceptaclesIter));
    }

    StringList delivererReceptacles = prototypeFU_->getKeysFromDelivererReceptacleRegistry();
    StringList::iterator delivererReceptaclesIter;
    for (delivererReceptaclesIter = delivererReceptacles.begin();
         delivererReceptaclesIter != delivererReceptacles.end();
         delivererReceptaclesIter++)
    {
        (static_cast<DelivererReceptacleSeparator*>(getFromDelivererReceptacleRegistry(*delivererReceptaclesIter)))
            ->addInstance(key, functionalUnit->getFromDelivererReceptacleRegistry(*delivererReceptaclesIter));
    }

    StringList receptorReceptacles = prototypeFU_->getKeysFromReceptorReceptacleRegistry();
    StringList::iterator receptorReceptaclesIter;
    for (receptorReceptaclesIter = receptorReceptacles.begin();
         receptorReceptaclesIter != receptorReceptacles.end();
         receptorReceptaclesIter++)
    {
        (static_cast<ReceptorReceptacleSeparator*>(getFromReceptorReceptacleRegistry(*receptorReceptaclesIter)))
            ->addInstance(key, functionalUnit->getFromReceptorReceptacleRegistry(*receptorReceptaclesIter));
    }

    instances[key] = functionalUnit;
} // instance.integrate


void
FlowSeparator::disintegrate(const ConstKeyPtr& key)
{
    InstanceMap::iterator it = instances.find(key);
    assure(it != instances.end(),
           "trying to disintegrate FU for an unknown key.");

    assure( instanceBusy != it->second,
            "FlowSeparator::disintegrate: Can't disintegrate busy Instance/Flow!");

    StringList connectorReceptacles = prototypeFU_->getKeysFromConnectorReceptacleRegistry();
    StringList::iterator connectorReceptaclesIter;
    for (connectorReceptaclesIter = connectorReceptacles.begin();
         connectorReceptaclesIter != connectorReceptacles.end();
         connectorReceptaclesIter++)
    {
        (static_cast<ConnectorReceptacleSeparator*>(getFromConnectorReceptacleRegistry(*connectorReceptaclesIter)))
            ->removeInstance(key);
    }

    StringList delivererReceptacles = prototypeFU_->getKeysFromDelivererReceptacleRegistry();
    StringList::iterator delivererReceptaclesIter;
    for (delivererReceptaclesIter = delivererReceptacles.begin();
         delivererReceptaclesIter != delivererReceptacles.end();
         delivererReceptaclesIter++)
    {
        (static_cast<DelivererReceptacleSeparator*>(getFromDelivererReceptacleRegistry(*delivererReceptaclesIter)))
            ->removeInstance(key);
    }

    StringList receptorReceptacles = prototypeFU_->getKeysFromReceptorReceptacleRegistry();
    StringList::iterator receptorReceptaclesIter;
    for (receptorReceptaclesIter = receptorReceptacles.begin();
         receptorReceptaclesIter != receptorReceptacles.end();
         receptorReceptaclesIter++)
    {
        (static_cast<ReceptorReceptacleSeparator*>(getFromReceptorReceptacleRegistry(*receptorReceptaclesIter)))
            ->removeInstance(key);
    }

    delete it->second;
    instances.erase(it);
} // instance.disintegrate


