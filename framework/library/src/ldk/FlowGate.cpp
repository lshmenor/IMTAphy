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

#include <WNS/ldk/FlowGate.hpp>

using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	FlowGate,
	FunctionalUnit,
	"wns.FlowGate",
	FUNConfigCreator
	);

FlowGate::FlowGate(fun::FUN* fun, const wns::pyconfig::View& config) :
	CommandTypeSpecifier<>(fun),
	HasReceptor<>(),
	HasConnector<>(),
	HasDeliverer<>(),
	Cloneable<FlowGate>(),

	flowStatus(),
	keyBuilder(NULL),
	logger(config.get("logger"))
{
	pyconfig::View keyBuilderConfig(config, "keyBuilder");
	std::string keyBuilderName = keyBuilderConfig.get<std::string>("__plugin__");
	keyBuilder = KeyBuilderFactory::creator(keyBuilderName)->create(fun, keyBuilderConfig);
}

FlowGate::FlowGate(fun::FUN* fun, const wns::pyconfig::View& config, KeyBuilder* builder) :
	CommandTypeSpecifier<>(fun),
	HasReceptor<>(),
	HasConnector<>(),
	HasDeliverer<>(),
	Cloneable<FlowGate>(),

	flowStatus(),
	keyBuilder(builder),
	logger(config.get("logger"))
{}

FlowGate::~FlowGate()
{
	delete keyBuilder;
	keyBuilder = NULL;

	while(flowStatus.empty() == false)
	{
		flowStatus.erase(flowStatus.begin());
	}
}

void
FlowGate::onFUNCreated()
{
	keyBuilder->onFUNCreated();
}

void
FlowGate::doSendData(const CompoundPtr& compound)
{
	assure(this->isAccepting(compound), "send data called even though not accepting");

	// Forward call to next FU
	return getConnector()->getAcceptor(compound)->sendData(compound);
}

void
FlowGate::doOnData(const CompoundPtr& compound)
{
	ConstKeyPtr key = (*keyBuilder)(compound, Direction::INCOMING());

	if (flowIsKnown(key))
	{
		if ( flowStatus.find(key)->second == true )
		{
			// Forward call to next FU
			getDeliverer()->getAcceptor(compound)->onData(compound);
		}

		else
		{
			// Gate is closed for this flow, drop the compound
			MESSAGE_SINGLE(NORMAL, logger, "Dropped incoming compound for closed flow ("<< key->str()<<")!");
		}
	}
	else
	{
		// Gate is unknown for this flow, drop the compound
		MESSAGE_SINGLE(NORMAL, logger, "Dropped incoming compound for unknown flow ("<< key->str()<<")!");
	}
}

bool
FlowGate::doIsAccepting(const CompoundPtr& compound) const
{
	ConstKeyPtr key = (*keyBuilder)(compound, Direction::OUTGOING());

	if (flowIsKnown(key))
	{
		if ( flowStatus.find(key)->second == true )
		{
			// Forward call to next FU
			return getConnector()->hasAcceptor(compound);
		}
		else
		{
			// Gate is closed for this flow
			return false;
		}
	}
	else
	{
		std::stringstream ss;
		ss << "Compound for unknown flow " << key->str() << " reached FlowGate!\n";
		throw wns::Exception(ss.str());
	}
}

void
FlowGate::doWakeup()
{
	// pass this call to the next FU
	getReceptor()->wakeup();
}

void
FlowGate::createFlow(const ConstKeyPtr& key)
{
	assure(!flowIsKnown(key), "You cannot create a flow twice!");
	flowStatus[key] = true;
	MESSAGE_SINGLE(NORMAL, logger, "Flow Created for key:" << key->str());
}

void
FlowGate::destroyFlow(const ConstKeyPtr& key)
{
	MESSAGE_SINGLE(NORMAL, logger, "Destroying flow for key:" << key->str());

	FlowStatus::iterator remove = flowStatus.find(key);
	assure(remove != flowStatus.end(), "Flow does not exist!");
	flowStatus.erase(remove);
}

void
FlowGate::openFlow(const ConstKeyPtr& key)
{
	MESSAGE_SINGLE(NORMAL, logger, "Opening flow for key:" << key->str());
	assure(flowIsKnown(key), "Flow does not exist!");
	flowStatus[key] = true;
}

void
FlowGate::closeFlow(const ConstKeyPtr& key)
{
	MESSAGE_SINGLE(NORMAL, logger, "Closing flow for key:" << key->str());
	assure(flowIsKnown(key), "Flow does not exist!");
	flowStatus[key] = false;
}

bool
FlowGate::flowIsKnown(const ConstKeyPtr& key) const
{
	return flowStatus.find(key) != flowStatus.end();
}


