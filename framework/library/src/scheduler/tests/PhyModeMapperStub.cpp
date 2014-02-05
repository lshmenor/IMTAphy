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

//#include <WNS/StaticFactory.hpp>
//#include <WNS/ldk/FUNConfigCreator.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/service/phy/phymode/PhyModeMapperInterface.hpp>
#include <WNS/pyconfig/Parser.hpp>
//#include "PhyMode.hpp"
//#include "PhyModeMapper.hpp"
#include "PhyModeStub.hpp"
#include "PhyModeMapperStub.hpp"

//using namespace wns::service::phy::phymode;
//using namespace rise::plmapping;

using namespace wns::scheduler;
using namespace wns::scheduler::tests;

PhyModeMapper::PhyModeMapper(const wns::pyconfig::View& config)
	: subCarriersPerSubChannel(config.get<unsigned int>("subCarriersPerSubChannel")),
	  symbolDuration(config.get<simTimeType>("symbolDuration")),
	  phyModeCount(config.len("mapEntries")),
	  minimumSINR(config.get<double>("minimumSINR"))
{
	// phyModeRangeMap is empty here
	// iterate through the SINR-2-PhyMode table
	for (unsigned int phymodeIndex=0; phymodeIndex < phyModeCount; ++phymodeIndex) {
		wns::pyconfig::View mapEntry = config.getView("mapEntries",phymodeIndex);
		wns::service::phy::phymode::PhyModeInterface* phyModeInterfacePtr = wns::service::phy::phymode::createPhyModeNonConst(mapEntry.get<wns::pyconfig::View>("phyMode"));
		PhyModePtr aPhyMode =
			PhyModePtr(dynamic_cast<PhyMode*>(phyModeInterfacePtr));
		aPhyMode->setSubCarriersPerSubChannel(subCarriersPerSubChannel);
		aPhyMode->setSymbolDuration(symbolDuration);
		assure(aPhyMode->dataRateIsValid(),"unknown dataRate for PhyMode "<< *aPhyMode);
		phyModeVector.push_back(aPhyMode);
		wns::service::phy::phymode::SINRRange sinrRange =
			wns::service::phy::phymode::SINRRange::CreateFrom(mapEntry.get<wns::pyconfig::View>("sinrInterval"));
		phyModeRangeMap.insert( sinrRange, phymodeIndex );
		phyModeSINRRangeRegistry.insert( *aPhyMode,sinrRange );
	}
}

/*
PhyModeMapper::PhyModeMapper()
	: subCarriersPerSubChannel(10),
	  symbolDuration(0.000001),
	  phyModeCount(1),
	  minimumSINR(0.0)
{
	wns::pyconfig::Parser parser;
	parser.loadString("import openwns.PhyMode\n"
			  "phyMode = openwns.PhyMode.PhyModeDropin()\n");
	wns::service::phy::phymode::PhyModeInterfacePtr
		phyModeInterfacePtr = wns::service::phy::phymode::PhyModeInterfacePtr
		( new wns::scheduler::tests::PhyMode(parser.get("phyMode")) );
	PhyModePtr aPhyMode =
			wns::SmartPtr<PhyMode>(dynamic_cast<PhyMode*>(phyModeInterfacePtr.getPtr()));
	unsigned int phymodeIndex=0;
	aPhyMode->setSubCarriersPerSubChannel(subCarriersPerSubChannel);
	aPhyMode->setSymbolDuration(symbolDuration);
	assure(aPhyMode->dataRateIsValid(),"unknown dataRate for PhyMode "<<*aPhyMode);
	phyModeVector.push_back(aPhyMode); // the ordered list of PhyModes
	// now phyModeVector[phymodeIndex] == aPhyMode
	wns::service::phy::phymode::SINRRange sinrRange = wns::service::phy::phymode::SINRRange::From(0.0).To(20.0);
	phyModeRangeMap.insert( sinrRange, phymodeIndex );
	phyModeSINRRangeRegistry.insert( *aPhyMode,sinrRange );
}
*/

wns::service::phy::phymode::PhyModeInterfacePtr
PhyModeMapper::getBestPhyMode(const wns::Ratio& sinr) const
{
	return getBestPhyMode(sinr.get_dB());
}

wns::service::phy::phymode::PhyModeInterfacePtr
PhyModeMapper::getBestPhyMode(double sinr /* dB */) const
{
	unsigned int phymodeIndex;
	try {
		phymodeIndex=phyModeRangeMap.get(sinr);
	} catch ( ... ) { // out of range
		if (sinr<0) { return getLowestPhyMode(); }
		else { return getHighestPhyMode(); }
	}
	return phyModeVector[phymodeIndex];
}
/*
unsigned int
PhyModeMapper::getBestPhyModeIndex(double sinr) const
{
	return phyModeRangeMap.get(sinr);
}
*/

wns::Ratio
PhyModeMapper::getMinSINRRatio(const wns::service::phy::phymode::PhyModeInterfacePtr phyMode) const
{
	wns::Ratio ratio;
	ratio.set_dB(getMinSINR(phyMode)); // double dB
	return ratio;
}

/** @brief find required (minimum) SINR [dB] for a certain PhyMode */
double
PhyModeMapper::getMinSINR(const wns::service::phy::phymode::PhyModeInterfacePtr phyMode) const
{
	assure(phyMode != wns::service::phy::phymode::PhyModeInterfacePtr(),"invalid phyModePtr");
	const PhyMode& aPhyMode = static_cast<const PhyMode&>(*phyMode);
	return phyModeSINRRangeRegistry.find(aPhyMode).min(); // faster
}

/** @brief find possible SINR range for a certain PhyMode */
wns::service::phy::phymode::SINRRange
PhyModeMapper::getSINRRange(const wns::service::phy::phymode::PhyModeInterfacePtr phyMode) const
{
	const PhyMode& aPhyMode = static_cast<const PhyMode&>(*phyMode);
	return phyModeSINRRangeRegistry.find(aPhyMode);
}

const wns::service::phy::phymode::PhyModeInterfacePtr
PhyModeMapper::getHighestPhyMode() const
{
	return phyModeVector.back();
}

const wns::service::phy::phymode::PhyModeInterfacePtr
PhyModeMapper::getLowestPhyMode() const
{
	return phyModeVector.front();
}

int
PhyModeMapper::getPhyModeCount() const
{
	return (int)phyModeCount;
}

const std::vector< wns::service::phy::phymode::PhyModeInterfacePtr >
PhyModeMapper::getListOfPhyModePtr() const
{
	std::vector< wns::service::phy::phymode::PhyModeInterfacePtr > phyModePtrVector(phyModeCount);
	for(unsigned int i=0; i<phyModeCount; i++) {
		phyModePtrVector[i] = phyModeVector[i];
	}
	return phyModePtrVector;
}

//const wns::service::phy::phymode::PhyModeInterface&
wns::service::phy::phymode::PhyModeInterfacePtr
PhyModeMapper::getPhyModeForIndex(int index) const
{
	return phyModeVector[index];
}

int
PhyModeMapper::getIndexForPhyMode(const wns::service::phy::phymode::PhyModeInterface& phyMode) const
{
	for (unsigned int phymodeIndex=0; phymodeIndex < phyModeCount; ++phymodeIndex) {
		if (*(phyModeVector[phymodeIndex]) == phyMode) return phymodeIndex;
	}
	assure(-1,"getIndexForPhyMode("<<phyMode<<"): not found");
	return wns::service::phy::phymode::UNDEFINED_PHYMODEINDEX; // (-1) = not found
}


