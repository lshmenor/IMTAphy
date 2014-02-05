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

#ifndef WNS_NODE_COMPONENT_FQSN_HPP
#define WNS_NODE_COMPONENT_FQSN_HPP

#include <WNS/pyconfig/View.hpp>

#include <ostream>
#include <string>

namespace wns { namespace node { namespace component {
	/**
	 * @brief Fully Qualified Service Name.
	 *
	 * This is used to identify services within a simulation. The
	 * node + service name is unique.
	 */
	class FQSN
	{
	public:
		/**
		 * @brief Construct from pyconfig::View
		 */
		explicit
		FQSN(const pyconfig::View& pyco);

		/**
		 * @brief Construct from two strings
		 */
		FQSN(const std::string& _nodeName,
		     const std::string& _serviceName);

		/**
		 * @brief Retrieve the node name
		 */
		std::string
		getNodeName() const;

		/**
		 * @brief Retrieve the component name
		 */
		std::string
		getServiceName() const;

		/**
		 * @brief output stream operator (for nice Message, etc)
		 */
		friend
		std::ostream&
		operator <<(std::ostream& str, const FQSN& fqsn)
		{
			str << fqsn.getNodeName() << "." << fqsn.getServiceName();
			return str;
		}

	private:
		std::string nodeName;
		std::string serviceName;
	};

} // component
} // node
} // wns


#endif //WNS_NODE_COMPONENT_FQSN_HPP
