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

#ifndef WNS_SCHEDULER_PHYMODEMAPPERSTUB_HPP
#define WNS_SCHEDULER_PHYMODEMAPPERSTUB_HPP

#include <string>
#include <vector>
#include <WNS/PowerRatio.hpp>
#include <WNS/Interval.hpp>
#include <WNS/container/Registry.hpp>
#include <WNS/container/RangeMap.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/service/phy/phymode/PhyModeMapperInterface.hpp>
//#include "PhyMode.hpp"
//#include "CodeRates.hpp"
//#include "MI2PER.hpp"
//#include "SNR2MI.hpp"
#include "PhyModeStub.hpp"

namespace wns { namespace scheduler { namespace tests {

	/** @brief Helper class to find the best PHYmode depending on SINR */
	/** @see used in RegistryProxyWINPROST for ResourceScheduler */
	/** rise::plmapping::PhyModeMapper */
	/** @author Rainer Schoenen (rs@comnets.rwth-aachen.de) */
	class PhyModeMapper
		: public service::phy::phymode::PhyModeMapperInterface // in libwns
	//: public wns::service::phy::phymode::PhyModeMapperInterface // in libwns
	{
	private:
		unsigned int subCarriersPerSubChannel;
		simTimeType symbolDuration;
		unsigned int phyModeCount;
		double minimumSINR;

		typedef wns::SmartPtr<PhyMode> PhyModePtr;

		/** @brief indexed list of all usable PhyModes, in order of quality */
		typedef std::vector< wns::SmartPtr<PhyMode> > PhyModeVector;
		/** @brief indexed list of all usable PhyModes, in order of quality */
		PhyModeVector phyModeVector;

		/** @brief map SINR values (double dB) to phymodeIndex */
		//typedef wns::container::RangeMap<double, PhyMode> PhyModeRangeMap;
		typedef wns::container::RangeMap<double, unsigned int> PhyModeRangeMap;
		/** @brief map SINR range/interval (double dB) to PHYMode */
		PhyModeRangeMap phyModeRangeMap;

		/** @brief maps from the PhyMode to the suitable SINRRange */
		/** (opposite of PhyModeRangeMap) */
		typedef wns::container::Registry< PhyMode, wns::service::phy::phymode::SINRRange > PhyModeSINRRangeRegistry;

		/** @brief maps from the PhyMode to the suitable SINRRange */
		PhyModeSINRRangeRegistry phyModeSINRRangeRegistry;

		/** @brief type to map PHYmode(pair) to its DataRate(float) */
		//typedef std::map<wns::scheduler::PHYmode, wns::scheduler::DataRate> Mode2Rate;
		/** @brief map of PHYmode(pair) to its DataRate(float) */
		//Mode2Rate mode2rate; // needed for libwns scheduler?

		//CoderFullMapping* mi2perMapper; // Singleton
		//SNR2MI* SNR2MImapper; // Singleton

	public:
		// Constructor
		//PhyModeMapper(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
		PhyModeMapper(const wns::pyconfig::View& config);
		// simple default constructor
		//PhyModeMapper();

		virtual ~PhyModeMapper() {};

		/** @brief find best PhyMode for a given SINR */
		//explicit // explicit forbids implicit type conversion from wns::Ratio
		virtual wns::service::phy::phymode::PhyModeInterfacePtr
		getBestPhyMode(const wns::Ratio& sinr) const;

		/** @brief find best PhyMode for a given SINR */
		virtual wns::service::phy::phymode::PhyModeInterfacePtr
		getBestPhyMode(double sinr /* dB */) const;

		/** @brief find best PhyModeIndex for a given SINR (faster than above) */
		int
		getBestPhyModeIndex(double sinr /* dB */) const;

		/** @brief find required (minimum) SINR for a certain PhyMode */
		virtual wns::Ratio
		getMinSINRRatio(const wns::service::phy::phymode::PhyModeInterfacePtr phyMode) const;

		/** @brief find required (minimum) SINR for a certain PhyMode */
		virtual double
		getMinSINR(const wns::service::phy::phymode::PhyModeInterfacePtr phyMode) const;

		/** @brief find possible SINR range for a certain PhyMode */
		virtual wns::service::phy::phymode::SINRRange
		getSINRRange(const wns::service::phy::phymode::PhyModeInterfacePtr phyMode) const;

		/** @brief best PhyMode of all available (most bits/symbol) */
		virtual const wns::service::phy::phymode::PhyModeInterfacePtr
		getHighestPhyMode() const;
		/** @brief worst PhyMode of all available (least bits/symbol) */
		virtual const wns::service::phy::phymode::PhyModeInterfacePtr
		getLowestPhyMode() const;

		/** @brief get number of available PhyModes */
		virtual int
		getPhyModeCount() const;

		/** @brief list of all available PhyModes */
		virtual const std::vector< wns::service::phy::phymode::PhyModeInterfacePtr >
		getListOfPhyModePtr() const;

		/** @brief get PhyMode at index position. O(1) */
		virtual wns::service::phy::phymode::PhyModeInterfacePtr
		getPhyModeForIndex(int index) const;

		/** @brief get index for PhyMode in the mapper list. O(n) */
		virtual int
		getIndexForPhyMode(const wns::service::phy::phymode::PhyModeInterface& phyMode) const;

		/** @brief below this minimumSINR there is no useful transmission (PER too high) */
		virtual double getMinimumSINR() const { return minimumSINR; };
		/** @brief tells if a useful transmission is possible */
		virtual bool sinrIsAboveLimit(double sinr /* dB */) const { return (sinr>=minimumSINR); };
		virtual bool sinrIsAboveLimit(const wns::Ratio& sinr) const { return (sinr.get_dB()>=minimumSINR); };


	protected:
		virtual void setMinimumSINR(double sinr) { minimumSINR=sinr; }
		/** @brief get PhyModeSINRRangeRegistry (only for jxu) */
		//virtual const rise::plmapping::PhyModeSINRRangeRegistry&
		//getPhyModeSINRRangeRegistry() const;
	};

}}}
//}}}}

#endif // RISE_PLMAPPING_PHYMODEMAPPER_HPP
