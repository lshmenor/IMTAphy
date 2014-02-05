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

#include <CONSTANZE/SubGenerator.hpp>
#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/ConstanzePDU.hpp>
#include <CONSTANZE/Binding.hpp>
#include <CONSTANZE/ConstanzeComponent.hpp>

#include <WNS/module/Base.hpp>
#include <WNS/distribution/Distribution.hpp>
#include <WNS/distribution/ForwardRecurrenceTime.hpp>
//#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/node/Node.hpp>

using namespace constanze;

/*************************** Generator *******************************/

SubGenerator::SubGenerator(constanze::GeneratorBase* _master, const wns::pyconfig::View& config) :
	CanTimeout(),
	interArrivalTimeDistribution(NULL),
	packetSizeDistribution(NULL),
	log(config.get("logger")),
	master(_master),
	//binding(_binding),
	rateScale(1.0)
{
	wns::pyconfig::View iatConfig(config, "iat");
	std::string iatName = iatConfig.get<std::string>("__plugin__");
	interArrivalTimeDistribution =
		wns::distribution::DistributionFactory::creator(iatName)->create(iatConfig);

	wns::pyconfig::View packetConfig(config, "packetSize");
	std::string psName = packetConfig.get<std::string>("__plugin__");
	packetSizeDistribution =
		wns::distribution::DistributionFactory::creator(psName)->create(packetConfig);

	MESSAGE_BEGIN(NORMAL, log, m, "New SubGenerator: ");
	m << "iat=" << *interArrivalTimeDistribution
	  << ", packetSize=" << *packetSizeDistribution;
	MESSAGE_END();
	//assure(binding,"undefined binding");
	assure(master,"undefined master reference");
}

SubGenerator::SubGenerator(constanze::GeneratorBase* _master, wns::logger::Logger& _log, wns::distribution::Distribution* _interArrivalTimeDistribution, wns::distribution::Distribution* _packetSizeDistribution, double _rateScale):
	CanTimeout(),
	interArrivalTimeDistribution(_interArrivalTimeDistribution),
	packetSizeDistribution(_packetSizeDistribution),
	log(_log),
	master(_master),
	//binding(_binding),
	rateScale(_rateScale)
{
	// Idee: change log name to +".SubGenerator[chain]"
	MESSAGE_BEGIN(NORMAL, log, m, "New SubGenerator: ");
	if (interArrivalTimeDistribution && packetSizeDistribution) { // not NULL
		m << "iat=" << *interArrivalTimeDistribution
		  << ", packetSize=" << *packetSizeDistribution;
	} else {
		m << "Empty";
	}
	MESSAGE_END();
	//assure(binding,"undefined binding");
	assure(master,"undefined master reference");
}

SubGenerator::~SubGenerator()
{
	if (hasTimeoutSet()) cancelTimeout();
}


void SubGenerator::reconfigLogger(wns::logger::Logger& _log)
{
	// previous value of log is a valid logger
	// but carries the name of its parent GeneratorMMPP
	// now the name should be .SubGen[0]
	log = _log;
}

void SubGenerator::reconfig(wns::distribution::Distribution* _interArrivalTimeDistribution, wns::distribution::Distribution* _packetSizeDistribution)
{
	interArrivalTimeDistribution = _interArrivalTimeDistribution;
	packetSizeDistribution = _packetSizeDistribution;
	assure (interArrivalTimeDistribution && packetSizeDistribution, "Empty distribution(s)");
	MESSAGE_BEGIN(NORMAL, log, m, "SubGenerator::reconfig(): ");
	m << "iat=" << *interArrivalTimeDistribution
	  << ", packetSize=" << *packetSizeDistribution;
	MESSAGE_END();
	if (hasTimeoutSet()) cancelTimeout();
	// new timeout for the new traffic:
	double iat = (*interArrivalTimeDistribution)();
	double firstiat = wns::distribution::forwardRecurrenceTime(iat);
	setTimeout(firstiat);
}

void SubGenerator::onTimeout() {
	// next event time
	assure (interArrivalTimeDistribution && packetSizeDistribution, "Empty distribution(s)");
	double iat = (*interArrivalTimeDistribution)();
	iat /= rateScale;
	int packetSize = static_cast<int>((*packetSizeDistribution)());
	MESSAGE_BEGIN(VERBOSE, log, m, "SubGenerator::onTimeout(): ");
	m << "iat=" << iat << ", packetSize=" << packetSize;
	MESSAGE_END();
	// some empty packet
	//wns::osi::PDUPtr pdu(new wns::ldk::helper::FakePDU(Bit(packetSize)));
	wns::osi::PDUPtr pdu(new ConstanzePDU(Bit(packetSize)));
	MESSAGE_SINGLE(NORMAL, log, "SubGenerator: Generated " << pdu->getLengthInBits()/8 << " bytes (" << pdu->getLengthInBits() << " bits).");
	// towards downstack direction
	master->countPackets(packetSize);
	master->sendData(pdu);
	// next event
	setTimeout(iat);
	MESSAGE_SINGLE(VERBOSE, log, "Next packet will be generated at t=" << wns::simulator::getEventScheduler()->getTime() + iat);
} // onTimeout()

void SubGenerator::start()
{
	assure (interArrivalTimeDistribution && packetSizeDistribution, "Empty distribution(s)");
	// statistically correct is to have a forwardRecurrenceTime:
	double iat = (*interArrivalTimeDistribution)();
	double firstiat = wns::distribution::forwardRecurrenceTime(iat);
	setTimeout(firstiat);
	//setTimeout(0.0); // first packet generated at once.
}

void SubGenerator::stop()
{
	cancelTimeout();
}



