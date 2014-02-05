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

#ifndef WNS_OBJECT_HPP
#define WNS_OBJECT_HPP

#include <WNS/TypeInfo.hpp>
#include <string>


namespace wns {

	/**
	 * @brief Enables dynamic_cast and offers method to retrieve class name
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 *
	 * Deriving your class @b virtual from this class will enable
	 * dynamic_cast and allows you to retrieve the class name. This class
	 * has no data member at all and will thus not add any overhead if you
	 * derive your class from this class. Only if your class has @b no virtual
	 * functions, this class will add sizeof(void*) Bytes to your class.
	 */
	class Object
	{
	public:
		/**
		 * @brief Enabled dynamic_cast
		 */
		virtual
		~Object();

		/**
		 * @brief Return class name (e.g., "wns::Position")
		 */
		std::string
		getClassName() const;
	};
}

#endif // NOT defined WNS_OBJECT_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
