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
// #include <WNS/service/phy/phymode/PhyModeMapperInterface.hpp>

#ifndef WNS_SERVICE_PHY_PHYMODE_PHYMODEMAPPERINTERFACE_HPP
#define WNS_SERVICE_PHY_PHYMODE_PHYMODEMAPPERINTERFACE_HPP

#include <string>
#include <WNS/PowerRatio.hpp>
#include <WNS/Interval.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/SmartPtr.hpp>

namespace wns { namespace service { namespace phy { namespace phymode {

	/** @brief interval of SINR values within which one PHYMode is best */
	typedef wns::Interval<double> SINRRange;

	/** @brief used if PhyMode is not in "official" list of PhyModeMapper */
	const int UNDEFINED_PHYMODEINDEX    = -1;

	/** @brief Helper class to find the best PHYmode depending on SINR */
	/** @see used in RegistryProxy for ResourceScheduler */
	/** wns::service::phy::phymode::PhyModeMapperInterface */
	class PhyModeMapperInterface
	{
	public:
		virtual ~PhyModeMapperInterface() {}

		/** @brief find best PhyMode for a given SINR */
		virtual PhyModeInterfacePtr
		getBestPhyMode(const wns::Ratio& sinr) const = 0;

		/** @brief find required (minimum) SINR for a certain PhyMode */
		virtual wns::Ratio
		getMinSINRRatio(const wns::service::phy::phymode::PhyModeInterfacePtr phyMode) const = 0;

		/** @brief tells if a successful transmission is possible,
		 i.e. we have more than the minimum required SINR */
		virtual bool
		sinrIsAboveLimit(const wns::Ratio& sinr) const = 0;

		/** @brief get PhyMode at index position [0=start] */
		virtual PhyModeInterfacePtr
		getPhyModeForIndex(int index) const = 0;

		/** @brief get index position of PhyMode in order of required SINR [0=start] */
		virtual int
		getIndexForPhyMode(const wns::service::phy::phymode::PhyModeInterface& phyMode) const = 0;

		/** @brief best PhyMode of all available (most bits/symbol) */
		virtual const wns::service::phy::phymode::PhyModeInterfacePtr
		getHighestPhyMode() const = 0;
		/** @brief worst PhyMode of all available (least bits/symbol) */
		virtual const wns::service::phy::phymode::PhyModeInterfacePtr
		getLowestPhyMode() const = 0;

		/** @brief find required (minimum) SINR [dB] for a certain PhyMode */
		virtual double
		getMinSINR(const wns::service::phy::phymode::PhyModeInterfacePtr phyMode) const = 0;

		/** @brief find possible SINR range [dB] for a certain PhyMode */
		virtual wns::service::phy::phymode::SINRRange
		getSINRRange(const wns::service::phy::phymode::PhyModeInterfacePtr phyMode) const = 0;

		/** @brief below this minimumSINR there is no useful transmission (PER too high) */
		virtual double getMinimumSINR() const = 0;

		/** @brief get number of available PhyModes */
		virtual int
		getPhyModeCount() const = 0;

		/** @brief list of all available PhyModes */
		virtual const std::vector< wns::service::phy::phymode::PhyModeInterfacePtr >
		getListOfPhyModePtr() const = 0;

		/** @brief to retrieve the Mapping Object from the Broker Singleton */
		static PhyModeMapperInterface*
		getPhyModeMapper(const wns::pyconfig::View& config);

	private:
/** Commented out all the methods below because they are not used anywhere */

//		/** @brief find best PhyMode for a given SINR */
//		virtual const wns::service::phy::phymode::PhyModeInterface&
//		getBestPhyMode(double sinr /* dB */) const = 0;

//		/** @brief find best PhyModeIndex for a given SINR (faster than above) */
//		int
//		getBestPhyModeIndex(double sinr /* dB */) const;

//		virtual bool sinrIsAboveLimit(double sinr /* dB */) const = 0;

	};

	typedef wns::PyConfigViewCreator<PhyModeMapperInterface> PhyModeMapperCreator;
	typedef wns::StaticFactory<PhyModeMapperCreator> PhyModeMapperFactory;

	/** @brief get a new instance of the PhyModeMapper from the Static Factory */
	inline PhyModeMapperInterface*
	createPhyModeMapper(const wns::pyconfig::View& config)
	{
		std::cout << "createPhyModeMapper() is obsolete. Use getPhyModeMapper to get a singleton!"<<std::endl;
		std::string nameInFactory = config.get<std::string>("nameInPhyModeMapperFactory");
		return PhyModeMapperFactory::creator(nameInFactory)->create(config);
	}

} // phymode
} // phy
} // service
} // wns

#endif // WNS_SERVICE_PHY_PHYMODE_PHYMODEMAPPERINTERFACE_HPP
