/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
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

#include <WNS/scheduler/tests/PhyModeStub.hpp>
#include <WNS/StaticFactory.hpp>

using namespace wns::service::phy::phymode;
using namespace wns::scheduler::tests;

STATIC_FACTORY_REGISTER_WITH_CREATOR(PhyMode,
									 wns::service::phy::phymode::PhyModeInterface,
									 "test.PhyMode.PhyMode",
									 wns::PyConfigViewCreator);

STATIC_FACTORY_REGISTER(PhyMode,
						wns::service::phy::phymode::PhyModeInterface,
						"test.PhyMode.Empty");

PhyMode::PhyMode() :
	modulation(UNDEFINED_MODULATION),
	coding(UNDEFINED_CODING),
	codeRate(0.5),
	subCarriersPerSubChannelKnown(false),
	subCarriersPerSubChannel(1),
	symbolDurationKnown(false),
	symbolDuration(0.0),
	dataRateKnown(false),
	dataRate(0.0)
{
}

/** @brief Construct PHY Mode from PyConfig	*/
PhyMode::PhyMode(const wns::pyconfig::View& config) :
	//snr2miMapper(wns::service::phy::phymode::SNR2MIInterface::getSNR2MImapper(config.get("snr2miMapping"))), // unique object pointer
	//coderMapper(wns::service::phy::phymode::CoderFullMappingInterface::getCoderFullMapping(config.get("mi2perMapper"))), // unique object pointer
	modulation(Modulations::fromString(config.get<std::string>("modulation"))),
	coding(1),
	codeRate(0.0),
	codingString(config.get<std::string>("coding")),
	subCarriersPerSubChannelKnown(false),
	subCarriersPerSubChannel(1),
	symbolDurationKnown(false),
	symbolDuration(0.0),
	dataRateKnown(false),
	dataRate(0.0)
{
	std::string::size_type loc1 = codingString.find( "-", 0 );
	if (loc1 == std::string::npos) loc1=0;
	std::string subString = codingString.substr(loc1+1);
	std::string::size_type loc2 = subString.find( "/", 0 );
	if (loc2 != std::string::npos) subString.replace( loc2, 1, " " );
	std::istringstream instream(subString);
	int zaehler,nenner;
	instream >> zaehler;
	instream >> nenner;
	codeRate = (double)zaehler/(double)nenner;
	assure(modulation > UNDEFINED_MODULATION, "UNDEFINED_MODULATION");
	assure(coding > UNDEFINED_CODING, "UNDEFINED_CODING");
	assure(modulation <= MAX_MODULATIONS, "MAX_MODULATIONS exceeded");
	if (config.knows("symbolDuration") && !config.isNone("symbolDuration")) {
		symbolDuration = config.get<simTimeType>("symbolDuration");
		symbolDurationKnown = true;
	}
	if (config.knows("subCarriersPerSubChannel") && !config.isNone("subCarriersPerSubChannel")) {
		subCarriersPerSubChannel = config.get<unsigned int>("subCarriersPerSubChannel");
		subCarriersPerSubChannelKnown = true;
	}
	calculateDataRate();
	//std::cout << "PhyMode::PhyMode("<<getString()<<" # "<<subString<<" # "<<zaehler<<"/"<<nenner<<" # "<<coding<<") = " << dataRate << std::endl;
	//std::cout << "PhyMode::PhyMode("<<getString()<<") = " << dataRate << std::endl;
}


PhyMode::~PhyMode() {}

void
PhyMode::setSubCarriersPerSubChannel(unsigned int _subCarriersPerSubChannel)
{
	subCarriersPerSubChannel = _subCarriersPerSubChannel;
	subCarriersPerSubChannelKnown = true;
	//std::cout << "PhyMode::setSubCarriersPerSubChannel("<<getString()<<","<<subCarriersPerSubChannel<<")"<< std::endl;
	calculateDataRate();
}

void
PhyMode::setSymbolDuration(simTimeType _symbolDuration)
{
	symbolDuration = _symbolDuration;
	symbolDurationKnown = true;
	//std::cout << "PhyMode::setSymbolDuration("<<getString()<<","<<symbolDuration<<")"<< std::endl;
	calculateDataRate();
}

void
PhyMode::cloneParameters(const PhyMode& other)
{
	assure(other.dataRateIsValid(),"cloneParameters() requires fully parameterized PhyMode");
	subCarriersPerSubChannel = other.subCarriersPerSubChannel;
	subCarriersPerSubChannelKnown = true;
	symbolDuration = other.symbolDuration;
	symbolDurationKnown = true;
	calculateDataRate();
}

bool
PhyMode::isValid() const
{
	return (modulation>UNDEFINED_MODULATION)
		&& (modulation<=MAX_MODULATIONS);
		//&& (coding>UNDEFINED_CODING);
}

/** @brief this PhyMode can offer a data Rate */
bool
PhyMode::dataRateIsValid() const
{
	return isValid() && dataRateKnown;
}

/** @brief access method for the code rate (double) */
double
PhyMode::getCodeRate() const
{
	//assure(coderMapper != NULL, "invalid coderMapper");
	if (this->isValid() == false) throw (UndefinedException());
	//return coderMapper->getRate(coding);
	//return 1.0/2.0; // fake for testing
	return codeRate;
}

/** @brief bit rate per OFDM symbol */
double
PhyMode::getBitsPerSymbol() const
{
	//assure(coderMapper != NULL, "invalid coderMapper");
	//return coderMapper->getRate(coding) * modulation;
	return codeRate*modulation;
}

double
PhyMode::getDataRate(double _symbolRate, unsigned int _subCarriersPerSubChannel) const
{
	return getBitsPerSymbol() * _symbolRate * _subCarriersPerSubChannel;
}

double
PhyMode::getDataRate() const
{
	assure(dataRateKnown,"getDataRate(): cannot get dataRate for "<<getString());
	return dataRate;
}

double PhyMode::getSINR2MI(const wns::Ratio& /*sinr*/) const
{
	//assure(snr2miMapper != NULL, "invalid snr2miMapper");
	//return snr2miMapper->convertSNR2MI(sinr,*this);
	return modulation; // best possible result
}
double PhyMode::getSINR2MIB(const wns::Ratio& /*sinr*/) const
{
	//assure(snr2miMapper != NULL, "invalid snr2miMapper");
	//return snr2miMapper->convertSNR2MIB(sinr,*this);
	return 1.0; // best possible result
}
double PhyMode::getMI2PER(const double /*mi*/, unsigned int /*bits*/) const
{
	//assure(coderMapper != NULL, "invalid coderMapper");
	//return coderMapper->mapMI2PER(mi,bits,coding);
	return 0.0; // best possible result
}
double PhyMode::getSINR2PER(const wns::Ratio& /*sinr*/, unsigned int /*bits*/) const
{
	//assure(coderMapper != NULL, "invalid coderMapper");
	//return coderMapper->mapMI2PER(snr2miMapper->convertSNR2MIB(sinr,*this),bits,coding);
	return 0.0; // best possible result
}

wns::Ratio PhyMode::getMIB2SINR(const double& mib) const
{
    return wns::Ratio::from_dB(0.0);
}

unsigned int
PhyMode::getBitCapacityFractional(simTimeType duration) const
{
	// calculate the capacity of this burst [bits]
	//static double epsilon = 1e-6; // to combat precision errors
	assure(dataRateKnown,"unknown dataRate");
	double capacity = dataRate * duration;
	return int(capacity+0.5);
}

/*
unsigned int
PhyMode::getBitCapacity(simTimeType duration) const
{
	// calculate the capacity of this burst [bits]
	static double epsilon = 1e-6;
	assure(subCarriersPerSubChannelKnown,"unknown subCarriersPerSubChannel");
	assure(symbolDurationKnown,"unknown symbolDuration");
	int OFDMsymbols = (int)(duration / symbolDuration + epsilon);
	// the following "if" is a quickhack to avoid assert failure in winprost::PhyUser
	// because duration = (phyCommand->local.stop - phyCommand->local.start) is too short
	if ((OFDMsymbols == 0) // very_short_duration, bad idea of caller!
		&& (duration > 0.0)) // there is at least some small time
		OFDMsymbols = 1; // return at least the capacity of one OFDM symbol length
	double capacity = OFDMsymbols
		* subCarriersPerSubChannel
		* getBitsPerSymbol();
	return int(capacity+epsilon); // to combat precision errors
}
*/

std::string
PhyMode::getModulationString() const
{
	if (modulation != UNDEFINED_MODULATION) {
		return Modulations::toString(modulation);
	} else {
		return std::string("UNDEFINED_MODULATION");
	}
}

std::string
PhyMode::getCodingString() const
{
	//assure(coderMapper != NULL, "invalid coderMapper");
	//return coderMapper->getString(coding);
	return codingString;
}

std::string
PhyMode::getString() const
{
	return getModulationString()+"-"+getCodingString();
}

std::string
PhyMode::doToString() const
{
	return getString();
}

bool
PhyMode::nameMatches(const std::string& name) const
{
	return (getString().compare(name) == 0);
}

unsigned int
PhyMode::toInt() const
{
	unsigned int codingInt = (unsigned int)(coding*12+0.5);
	return (codingInt << MOD_BITS) + modulation;
	//return modulation; // currently the only difference in this test
}

void
PhyMode::calculateDataRate()
{
	if (subCarriersPerSubChannelKnown && symbolDurationKnown) {
		//assure(subCarriersPerSubChannelKnown,"unknown subCarriersPerSubChannel");
		//assure(symbolDurationKnown,"unknown symbolDuration");
		dataRate = getBitsPerSymbol() * subCarriersPerSubChannel / symbolDuration;
		dataRateKnown = true;
	}
	//std::cout << "PhyMode::calculateDataRate("<<getString()<<") = " << dataRate << std::endl;
}

