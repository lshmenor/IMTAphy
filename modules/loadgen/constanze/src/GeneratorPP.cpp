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

#include <CONSTANZE/GeneratorPP.hpp>
#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/Binding.hpp>
#include <CONSTANZE/ConstanzeComponent.hpp>

#include <WNS/module/Base.hpp>
#include <WNS/distribution/Distribution.hpp>
#include <WNS/distribution/ForwardRecurrenceTime.hpp>
#include <WNS/node/Node.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	constanze::GeneratorPP,
	constanze::GeneratorBase,
	"PointProcess",
	wns::PyConfigViewCreator);


GeneratorPP::GeneratorPP(const wns::pyconfig::View& config) :
	GeneratorBase(config),
	CanTimeout(),
	interArrivalTimeDistribution(NULL),
	packetSizeDistribution(NULL),
	correctFRT(true)
{
	wns::pyconfig::View iatConfig(pyco, "iat");
	std::string iatName = iatConfig.get<std::string>("__plugin__");
	interArrivalTimeDistribution =
		wns::distribution::DistributionFactory::creator(iatName)->create(iatConfig);

	wns::pyconfig::View packetConfig(pyco, "packetSize");
	std::string psName = packetConfig.get<std::string>("__plugin__");
	packetSizeDistribution =
		wns::distribution::DistributionFactory::creator(psName)->create(packetConfig);
	if (config.knows("correctFRT") && !config.isNone("correctFRT")) {
		correctFRT = config.get<bool>("correctFRT");
	}


	MESSAGE_BEGIN(NORMAL, log, m, "New GeneratorPP: ");
	m << "iat=" << *interArrivalTimeDistribution
	  << ", packetSize=" << *packetSizeDistribution;
	MESSAGE_END();
}


GeneratorPP::~GeneratorPP()
{
	if (hasTimeoutSet()) cancelTimeout();
	if (packetSizeDistribution != NULL) delete packetSizeDistribution; packetSizeDistribution = NULL;
	if (interArrivalTimeDistribution != NULL) delete interArrivalTimeDistribution; interArrivalTimeDistribution = NULL;
}

void
GeneratorPP::onTimeout() {
	// next event time
	double iat = (*interArrivalTimeDistribution)();
	int packetSize = static_cast<int>((*packetSizeDistribution)());
	countPackets(packetSize);

	// some empty packet
	//wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(Bit(packetSize)));
	wns::osi::PDUPtr pdu(new ConstanzePDU(Bit(packetSize)));

	MESSAGE_SINGLE(NORMAL, log, "Generated " << pdu->getLengthInBits()/8 << " bytes (" << pdu->getLengthInBits() << " bits).");

	// towards downstack direction
	sendData(pdu);

	// next event
	setTimeout(iat);

	MESSAGE_SINGLE(VERBOSE, log, "Next packet will be generated at t=" << wns::simulator::getEventScheduler()->getTime() + iat);
} // onTimeout()

void GeneratorPP::start()
{
	// statistically correct is to have a forwardRecurrenceTime:
	if (correctFRT) {
		double iat = (*interArrivalTimeDistribution)();
		double firstiat = wns::distribution::forwardRecurrenceTime(iat);
		setTimeout(firstiat);
		MESSAGE_SINGLE(NORMAL, log, "start() after firstiat="<<firstiat<<"s");
	} else {
		setTimeout(0.0); // first packet generated at once.
		MESSAGE_SINGLE(NORMAL, log, "start() now");
	}
}

void GeneratorPP::stop()
{
	cancelTimeout();
	MESSAGE_SINGLE(NORMAL, log, "stop()");
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
