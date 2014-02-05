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

#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/Binding.hpp>
#include <CONSTANZE/ConstanzeComponent.hpp>

#include <WNS/module/Base.hpp>
#include <WNS/distribution/Distribution.hpp>
#include <WNS/node/Node.hpp>

using namespace constanze;

GeneratorBase::GeneratorBase( const wns::pyconfig::View& config) :
	pyco(config),
	log(config.get("logger")),
	binding(NULL),
	startTrigger(NULL),
	stopTrigger(NULL),
	packetCounter(0UL),
	bitCounter(0UL)
{
}

GeneratorBase::~GeneratorBase()
{
	MESSAGE_BEGIN(NORMAL, log, m, "Created a total of ");
	m << packetCounter << " packets with " << bitCounter << " bits";
	if (binding) m << " to IP=" << *binding;
	MESSAGE_END();

	if (startTrigger != NULL) delete startTrigger; startTrigger = NULL;
	if (stopTrigger != NULL) delete stopTrigger; stopTrigger = NULL;
}

void GeneratorBase::registerBinding(constanze::Binding* _binding)
{
	binding = _binding;
	double offset   = pyco.get<simTimeType>("offset");
	double duration = pyco.get<simTimeType>("duration");
	MESSAGE_SINGLE(NORMAL, log, "registerBinding(@offset="<<offset<<"s for duration="<<duration<<"s)");
	startTrigger = new constanze::StartTrigger(binding, this, offset);
	// duration==0 means infinite
	if (duration != 0.0)
		stopTrigger = new constanze::StopTrigger(binding, this, offset + duration);
}

void GeneratorBase::bindingReady()
{
	start();
	assure(startTrigger!=NULL,"no startTrigger");
	MESSAGE_SINGLE(NORMAL, log, "bindingReady()");
	delete startTrigger;
	startTrigger = NULL;
}

void
GeneratorBase::bindingReleased()
{
	stop();
	assure(stopTrigger!=NULL,"no stopTrigger");
	delete stopTrigger;
	stopTrigger = NULL;
}

void GeneratorBase::sendData(const wns::osi::PDUPtr& pdu)
{
	binding->sendData(pdu);
}

void GeneratorBase::countPackets(const int packetSizeBits)
{
	packetCounter += 1;
	bitCounter += packetSizeBits;
}

/*************************** Trigger Events *******************************/

StartTrigger::StartTrigger(constanze::Binding* _binding, constanze::GeneratorBase* _generator, double _startTime) :
	binding(_binding),
	generator(_generator)
{
	setTimeout(_startTime);
}


void StartTrigger::onTimeout()
{
	binding->initBinding(this);
}


void StartTrigger::bindingReady()
{
	generator->bindingReady();
}


StopTrigger::StopTrigger(constanze::Binding* _binding, constanze::GeneratorBase* _generator, double _stopTime) :
	binding(_binding),
	generator(_generator)
{
	setTimeout(_stopTime);
}


void StopTrigger::onTimeout()
{
	binding->releaseBinding(this);
}


void StopTrigger::bindingReady()
{
	generator->bindingReleased();
}

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
