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

#include <WNS/ldk/crc/CRC.hpp>

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/ErrorRateProviderInterface.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/probe/bus/utils.hpp>

#include <WNS/rng/RNGen.hpp>

using namespace wns::ldk;
using namespace wns::ldk::crc;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	CRC,
	FunctionalUnit,
	"wns.crc.CRC",
	FUNConfigCreator);

CRC::CRC(fun::FUN* fuNet, const wns::pyconfig::View& config) :
	fu::Plain<CRC, CRCCommand>(fuNet),
	SuspendSupport(fuNet, config),
	dis(),
	checkSumLength(config.get<int>("CRCsize")),
	PERProviderName(config.get<std::string>("PERProvider")),
	behaviour( config.get<bool>("isDropping") ? DROPPING : MARKING ),
	friends(),
	logger(config.get("logger"))
{
	// read the localIDs from the config
	wns::probe::bus::ContextProviderCollection localContext(&fuNet->getLayer()->getContextProviderCollection());

	for (int ii = 0; ii<config.len("localIDs.keys()"); ++ii)
	{
		std::string key = config.get<std::string>("localIDs.keys()",ii);
		unsigned long int value  = config.get<unsigned long int>("localIDs.values()",ii);
		localContext.addProvider( wns::probe::bus::contextprovider::Constant(key, value) );
	}

	friends.PERProvider = NULL;
	lossRatio = wns::probe::bus::collector(localContext, config, "lossRatioProbeName");
} // CRC

CRC::~CRC()
{
	friends.PERProvider = NULL;
}

void
CRC::onFUNCreated()
{
	friends.PERProvider = getFUN()->findFriend<FunctionalUnit*>(PERProviderName);
	assure(friends.PERProvider, "CRC requires a PERProvider friend with name '" + PERProviderName + "'");
}  // onFUNCreated

void
CRC::doSendData(const CompoundPtr& compound)
{
	activateCommand(compound->getCommandPool());

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " passing through CRC outgoing";
	MESSAGE_END();

	getConnector()->getAcceptor(compound)->sendData(compound);
}

void
CRC::doOnData(const CompoundPtr& compound)
{
	assure( (behaviour==DROPPING || behaviour==MARKING), "Unknown CRC behaviour" );

	CRCCommand* command = getCommand(compound->getCommandPool());

	ErrorRateProviderInterface* ppi = dynamic_cast<ErrorRateProviderInterface*>(
		friends.PERProvider->getCommand(compound->getCommandPool()));

	assure(ppi, "Expected a ErrorRateProviderInterface instance.");

	double per = ppi->getErrorRate();
	// if no known friends exist, the per should be
	if ( dis() < per ){

		if (lossRatio != NULL)
		{
			lossRatio->put(compound, 1);
		}

		MESSAGE_SINGLE(VERBOSE, logger, ppi->getInfo());

		if (behaviour == DROPPING)
		{
			MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
			m << " CRCheck - failed - dropping compound";
			MESSAGE_END();
			return;
		}
		else
		{
			command->local.checkOK = false;
			MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
			m << " CRCheck - failed - compound marked";
			MESSAGE_END();
		}
	}
	else
	{
		command->local.checkOK = true;

		if (lossRatio != NULL)
		{
			lossRatio->put(compound, 0);
		}

		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << " CRCheck - success";
		MESSAGE_END();
	}
	if(getDeliverer()->size())
		getDeliverer()->getAcceptor(compound)->onData(compound);
}


bool
CRC::doIsAccepting(const CompoundPtr& compound) const
{
	CompoundPtr compoundCopy = compound->copy();

	activateCommand(compoundCopy->getCommandPool());

	return getConnector()->hasAcceptor(compoundCopy);
} // doIsAccepting


void
CRC::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
	//What are the sizes in the upper Layers
	getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);
	commandPoolSize += checkSumLength;

} // calculateSizes


bool
CRC::isMarking() const
{
	return behaviour == MARKING;
} // isMarking



