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

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/buffer/Buffer.hpp>
#include <WNS/probe/bus/utils.hpp>

using namespace wns::ldk::buffer;
using namespace wns::ldk::buffer::sizecalculators;

// Size calculation stategies
unsigned long int
PerPDU::operator()(const CompoundPtr& /* pdu */) const
{
	return 1;
}
STATIC_FACTORY_REGISTER(PerPDU, SizeCalculator, "PDU");

unsigned long int
PerBit::operator()(const CompoundPtr& compound) const
{
	return compound->getLengthInBits();
}
STATIC_FACTORY_REGISTER(PerBit, SizeCalculator, "Bit");



Buffer::Buffer(fun::FUN* _fun, const pyconfig::View& _config) :
	events::PeriodicTimeout(),
	totalPDUs(0),
	droppedPDUs(0),
	droppedPDUWindow(_config.get<double>("droppedPDUWindowDuration")),
	probeDroppedPDUInterval(_config.get<double>("probeDroppedPDUInterval")),
	logger(_config.get("logger")),
	probingEnabled(_config.get<bool>("probingEnabled"))
{
	// read the localIDs from the config
	wns::probe::bus::ContextProviderCollection localContext(_fun->getLayer()->getContextProviderCollection());
	for (int ii = 0; ii<_config.len("localIDs.keys()"); ++ii)
	{
		std::string key = _config.get<std::string>("localIDs.keys()",ii);
		unsigned long int value  = _config.get<unsigned long int>("localIDs.values()",ii);
		localContext.addProvider( wns::probe::bus::contextprovider::Constant(key, value) );
	}

	std::string sizeProbeName = _config.get<std::string>("sizeProbeName");
	std::string lossProbeName = _config.get<std::string>("lossRatioProbeName");
	lossRatioProbeBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, lossProbeName));
	sizeProbeBus      = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, sizeProbeName));
}

Buffer::~Buffer()
{
}

void
Buffer::onFunCreated()
{
	// is not called. Whyever!?
}

void
Buffer::increaseTotalPDUs()
{
	// start on first seen compound
	if (probingEnabled)
	{
		// start on first seen compound
		if ((this->hasPeriodicTimeoutSet() == false) && (probeDroppedPDUInterval>0.0))
		{
			MESSAGE_SINGLE(NORMAL, logger,"Buffer::increaseTotalPDUs: startPeriodicTimeout(every "<<probeDroppedPDUInterval<<"s)");
			this->startPeriodicTimeout(this->probeDroppedPDUInterval);
		}
	}
	++this->totalPDUs;
}


void
Buffer::increaseDroppedPDUs(int size)
{
	++this->droppedPDUs;
	this->droppedPDUWindow.put(size);
}

void
Buffer::periodically()
{
	if (this->lossRatioProbeBus)
	{
		this->lossRatioProbeBus->put(this->droppedPDUWindow.getPerSecond());
	}
}

void
Buffer::probe()
{
	if(this->sizeProbeBus)
	{
		this->sizeProbeBus->put(((double) getSize()) / getMaxSize());
	}
} // probe



