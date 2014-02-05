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

#include <WNS/scheduler/tests/LinkAdaptationProxyStub.hpp>
#include <WNS/scheduler/tests/PhyModeMapperStub.hpp>
#include <WNS/pyconfig/Parser.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::tests;

LinkAdaptationProxyStub::LinkAdaptationProxyStub() :
	  phyModeMapper(NULL)
{
	wns::pyconfig::Parser parser;
	parser.loadString("import openwns.PhyMode\n"
			  "from openwns.interval import Interval\n"
              "pm1 = openwns.PhyMode.PhyModeDropin1()\n"
              "pm2 = openwns.PhyMode.PhyModeDropin2()\n"
              "pm3 = openwns.PhyMode.PhyModeDropin3()\n"
			  "phyModeMap = openwns.PhyMode.PhyModeMapperDropin()\n"
              "phyModeMap.symbolDuration = 1E-3 / 14.0\n"
              "phyModeMap.subCarriersPerSubChannel = 12\n"
			  "phyModeMap.setMinimumSINR(3.0)\n"
			  "phyModeMap.addPhyMode(Interval(-200.0,   3.0, \"(]\"), pm1)\n"
			  "phyModeMap.addPhyMode(Interval(   3.0,  10.0, \"(]\"), pm2)\n"
			  "phyModeMap.addPhyMode(Interval(  10.0, 200.0, \"(]\"), pm3)\n"
			  "\n"
		);

	phyModeMapper = new wns::scheduler::tests::PhyModeMapper(parser.get("phyModeMap"));
}

LinkAdaptationProxyStub::~LinkAdaptationProxyStub()
{
}

UserID
LinkAdaptationProxyStub::getUserForCID(ConnectionID cid)
{
	return UserID();
}

wns::service::phy::phymode::PhyModeMapperInterface*
LinkAdaptationProxyStub::getPhyModeMapper() const
{
	return phyModeMapper;
}

wns::SmartPtr<const wns::service::phy::phymode::PhyModeInterface>
LinkAdaptationProxyStub::getBestPhyMode(const wns::Ratio& sinr)
{
	return getPhyModeMapper()->getBestPhyMode(sinr);
}

ChannelQualityOnOneSubChannel
LinkAdaptationProxyStub::estimateTxSINRAt(const UserID user, int, int)
{
    /*PL = 80; I = -95; C = -80 => CIR = 15 dB*/
    return ChannelQualityOnOneSubChannel(
        wns::Ratio::from_dB(80.0),
        wns::Power::from_dBm(-95.0), 
        wns::Power::from_dBm(-80.0));
}

ChannelQualityOnOneSubChannel
LinkAdaptationProxyStub::estimateRxSINROf(const UserID user, int, int)
{
    /*PL = 80; I = -95; C = -80 => CIR = 15 dB*/
    return ChannelQualityOnOneSubChannel(
        wns::Ratio::from_dB(80.0),
        wns::Power::from_dBm(-95.0), 
        wns::Power::from_dBm(-80.0));
}

wns::scheduler::PowerCapabilities
LinkAdaptationProxyStub::getPowerCapabilities(const UserID) const
{
	wns::scheduler::PowerCapabilities result;
	result.maxOverall = wns::Power::from_dBm(24.0);
	result.maxPerSubband = wns::Power::from_dBm(6.0);
	result.nominalPerSubband = wns::Power::from_dBm(4.0);
	return result;
}

wns::scheduler::PowerCapabilities
LinkAdaptationProxyStub::getPowerCapabilities() const
{
	wns::scheduler::PowerCapabilities result;
	result.maxOverall = wns::Power::from_dBm(24.0);
	result.maxPerSubband = wns::Power::from_dBm(6.0);
	result.nominalPerSubband = wns::Power::from_dBm(4.0);
	return result;
}

wns::Ratio
LinkAdaptationProxyStub::getEffectiveUplinkSINR(const wns::scheduler::UserID sender, 
    const std::set<unsigned int>& scs,
    const int, 
    const wns::Power& txPower)
{
    std::vector<wns::Power> i;
    i.push_back(wns::Power::from_dBm(4));
    i.push_back(wns::Power::from_dBm(4));
    i.push_back(wns::Power::from_dBm(-1));
    i.push_back(wns::Power::from_dBm(-1));
    i.push_back(wns::Power::from_dBm(-7));
    i.push_back(wns::Power::from_dBm(-7));

    std::set<unsigned int>::iterator it;

    double sum = 0;

    for(it = scs.begin(); it!= scs.end(); it++)
    {
        sum += (txPower / i[(*it) % 6]).get_factor();
    }

    return wns::Ratio::from_factor(sum / double(scs.size()));
}

wns::Ratio
LinkAdaptationProxyStub::getEffectiveDownlinkSINR(const wns::scheduler::UserID receiver, 
    const std::set<unsigned int>& scs, 
    const int,
    const wns::Power& txPower,
    const bool worstCase)
{
    wns::Ratio eSINR = getEffectiveUplinkSINR(receiver, scs, 0, txPower);
    wns::Ratio offset = wns::Ratio::from_dB(1.0);

    return (eSINR + offset);
}

void 
LinkAdaptationProxyStub::updateUserSubchannels (const wns::scheduler::UserID user, std::set<int>& channels)
{
  return;
}