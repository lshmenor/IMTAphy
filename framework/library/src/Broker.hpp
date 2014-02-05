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

#ifndef WNS_BROKER_HPP
#define WNS_BROKER_HPP

#include <WNS/container/Registry.hpp>
#include <WNS/pyconfig/View.hpp>

namespace wns {

	/**
	 * @brief Able to create ELEMENTs from respective pyconfig::View
	 *
	 * If an object for a specific View has already been created this object is
	 * returned (instead of creating a new one).
	 */
	template <typename ELEMENT>
	class Broker
	{
		/**
		 * @brief Store the objects created from pyconfig::View
		 */
		typedef container::Registry<pyconfig::View, ELEMENT*, container::registry::DeleteOnErase> Registry;

	public:

		/**
		 * @brief Will delete all elements
		 */
		~Broker()
		{
			r.clear();
		}

		/**
		 * @brief Returns an object configured with pyco.
		 */
		ELEMENT*
		procure(const pyconfig::View& pyco)
		{
			if(r.knows(pyco))
			{
				return r.find(pyco);
			}
			else
			{
				ELEMENT* element = new ELEMENT(pyco);
				r.insert(pyco, element);
				return element;
			}
		}

	private:
		/**
		 * @brief ELEMENTs are stored here
		 */
		Registry r;
	};

} // wns

#endif // NOT defined WNS_BROKER_HPP


