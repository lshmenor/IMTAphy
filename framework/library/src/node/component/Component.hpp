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

#ifndef WNS_NODE_COMPONENT_COMPONENT_HPP
#define WNS_NODE_COMPONENT_COMPONENT_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/node/component/FQSN.hpp>
#include <WNS/node/component/Interface.hpp>

#include <string>

namespace wns { namespace node { namespace component {
	/**
	 * @brief Implementation of the ComponentInterface
	 *
	 * For an overview please see @ref wns::node
	 *
	 */
	class Component :
		virtual public Interface
	{
	public:
		/**
		 * @param[in] _name The name of this component as used in the
		 * Node
		 *
		 * @param[in] _node The Node this Component belongs to
		 */
		explicit Component(
			wns::node::Interface* _node,
			const pyconfig::View& _pyco);

		/**
		 * @brief Is called in Node's destructor
		 */
		virtual
		~Component();

		/**
		 * @return The name of this component as used in a Node
		 *
		 * @note This is used to address Components in a Node
		 */
		virtual std::string
		getName() const;

		/**
		 * @return The Node this Component belongs to.
		 */
		virtual wns::node::Interface*
		getNode() const;

	protected:
		const pyconfig::View&
		getConfig() const;

	private:
		/**
		 * @brief Component belongs to this Node
		 */
		wns::node::Interface* node;

		/**
		 * @brief Name of this Component's service (like "IPRouter")
		 */
		std::string name;

		/**
		 * @brief Configuration of this Component
		 */
		pyconfig::View pyco;
	};
} // component
} // node
} // wns

#endif

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
