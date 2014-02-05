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

#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>

#ifndef WNS_SERVICE_PHY_PHYMODE_MI2PER_HPP
#define WNS_SERVICE_PHY_PHYMODE_MI2PER_HPP

namespace wns { namespace service { namespace phy { namespace phymode {

	/**
	 * @brief Interface for a MI2PER mapper.
	 * Returns the PER value corresponding to the Mutual Information
	 * and the used coding scheme
	 */
	class MI2PERInterface
	{
	public:
		virtual ~MI2PERInterface(){}

		virtual double mapMI2PER(double mi,
								 unsigned int blockLength) const = 0;
	};

	/** @brief Interface for providing Basic Info about the Coder */
	class CoderSpecInterface
	{
	public:
		virtual
		~CoderSpecInterface() {};

		virtual Coding
		getCoding() const = 0;

		virtual double
		getRate() const = 0;

		virtual bool
		hasNoCoding() const = 0;

		virtual const std::string&
		getName() const = 0;
	};

	/** @brief Interface for Basic Coder Info Mapping */
	class CoderMappingInterface
	{
	public:
		virtual ~CoderMappingInterface() {};

		/* @brief get coding rate for coder given by name */
		virtual double
		getRate(const std::string& s) const = 0;

		/* @brief get coding rate */
		virtual double
		getRate(Coding coding) const = 0;

		/* @brief get coding (int) from name */
		virtual Coding
		fromString(const std::string& s) const = 0;

		/* @brief get coder name */
		virtual const std::string&
		getString(Coding c) const = 0;

		/* @brief get coder specification */
		virtual const CoderSpecInterface&
		getCoderSpecification(Coding c) const = 0;

		/** @brief static Helper method to retrieve the Mapping Object from the Broker Singleton */
		static CoderMappingInterface*
		getCoderMapping(const wns::pyconfig::View& config);

		/** @brief number of elements in table */
		virtual unsigned int
		getMaxCodeIndex() const = 0;
	};

	/** @brief Defining the external Interface of the MI2PER Mapping classes */
	class CoderFullSpecInterface :
		virtual public CoderSpecInterface,
		virtual public MI2PERInterface
	{
	public:
		virtual ~CoderFullSpecInterface() {}
	};

	class MI2PERMappingInterface
	{
	public:
		virtual ~MI2PERMappingInterface() {}

		/** @brief get PER for given Coding, MIB [0..1] and blockLength (=payload[net], not CWL[gross]) */
		virtual double
		mapMI2PER(double mi,
				  unsigned int blockLength,
				  Coding coding) const = 0;
	};

	typedef wns::PyConfigViewCreator<MI2PERMappingInterface, MI2PERMappingInterface> MI2PERMappingCreator;
	typedef wns::StaticFactory<MI2PERMappingCreator> MI2PERMappingFactory;

	/** @brief Helper Function to create MI2PER Mapping object from StaticFactory */
	inline MI2PERMappingInterface*
	createMI2PERMappingInterface(const wns::pyconfig::View& config)
	{
		std::string name = config.get<std::string>("mapperName");
		return MI2PERMappingFactory::creator(name)->create(config);
	}

	/** @brief Interface to provide Access to the Full Coder Mapping Data */
	class CoderFullMappingInterface :
		virtual public MI2PERMappingInterface,
		virtual public CoderMappingInterface
	{
	public:
		virtual ~CoderFullMappingInterface() {}

		/* @brief get coder full specification */
		virtual const CoderFullSpecInterface&
		getCoderFullSpecification(Coding c) const = 0;

		/** @brief to retrieve the Mapping Object from the Broker Singleton */
		static CoderFullMappingInterface*
		getCoderFullMapping(const wns::pyconfig::View& config);
	};

	typedef PyConfigViewCreator<MI2PERMappingInterface> MI2PERMappingCreator;

} // phymode
} // phy
} // service
} // wns

#endif // WNS_SERVICE_PHY_PHYMODE_MI2PER_HPP
