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

#ifndef WNS_NODE_COMPONENT_INTERFACE_HPP
#define WNS_NODE_COMPONENT_INTERFACE_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/node/component/ConfigCreator.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/service/Service.hpp>

#include <string>

namespace wns { namespace node {
	class Interface;
} // node
} // wns

namespace wns { namespace node { namespace component {
	/**
	 * @brief Components are part of a Node
	 *
	 * This is the interface. An implementation of this is available as:
	 * Component.
	 *
	 * For an overview please see @ref wns::node
	 */
	class Interface
	{
	public:
		/**
		 * @brief Is called in Node's destructor
		 */
		virtual
		~Interface()
		{}

		/**
		 * @return The name of this component as used in a Node
		 *
		 * @note This name is used to address Components in a Node
		 */
		virtual std::string
		getName() const = 0;

		/**
		 * @brief Called immedeatly after constructor
		 *
		 * This will be called immedeatly after the constructor has been
		 * called. The Component can call virtual methods of itself
		 * (e.g. addService) which is forbidden in the constructor. The
		 * Component may @bnot resolve any dependencies here to other
		 * Components!
		 *
		 * @todo <msg@comnets.rwth-aachen.de> In the future, this
		 * function will be able to return a list of functors which
		 * contain member functions pointers. These functors can be
		 * called by a Component 'Dependency Resolver' to, well,
		 * resolve all dependencies.
		 *
		 * @note If a component is constructed stand-alone (e.g. in a
		 * test) this must be called manually to ensure proper
		 * initialization of the component.
		 */
		void
		startup()
		{
			return this->doStartup();
		}

		/**
		 * @brief Called by the parent node after all components of the
		 * parent node have been created.
		 *
		 * @note Intra-Node dependencies like connecting components
		 * within the node to each other must be handled here. It is
		 * forbidden to try to resolve dependencies to other nodes
		 * here. Not all nodes have been setup yet.
		 */
		virtual void
		onNodeCreated() = 0;

		/**
		 * @brief Called by the parent node after all nodes have been
		 * setup and all components have executed onNodeCreated().
		 *
		 * @note Resolve Inter-Node dependencies here. There are no
		 * restrictions on what you do here.
		 */
		virtual void
		onWorldCreated() = 0;

		/**
		 * @brief Called by the parent node after simulation end event
		 * occured
		 */
		virtual void
		onShutdown() = 0;

		/**
		 * @brief Retrieve a Component by its name (automatic typecast)
 		 */
		template <typename SERVICETYPE>
		SERVICETYPE
		getService(const std::string& serviceName) const
		{
			return getNode()->getService<SERVICETYPE>(serviceName);
		}

		/**
		 * @brief Retrieve a Component by its name (automatic typecast)
 		 */
		template <typename SERVICETYPE>
		SERVICETYPE
		getRemoteService(const component::FQSN& fqsn) const
		{
			return getNode()->getRemoteService<SERVICETYPE>(fqsn);
		}

		/**
		 * @brief Add a service
		 */
		virtual void
		addService(const std::string& name, service::Service* service)
		{
			getNode()->addService(name, service);
		}

		/**
		 * @brief Retrieve ContextProviderCollection
		 */
		virtual wns::probe::bus::ContextProviderCollection&
		getContextProviderCollection() const
		{
			return getNode()->getContextProviderCollection();
		}

	protected:
		/**
		 * @return The Node this Component belongs to.
		 */
		virtual wns::node::Interface*
		getNode() const = 0;

		/**
		 * @brief Virtual backend of startup()
		 */
		virtual void
		doStartup() = 0;
	};

	typedef
	ConfigCreator<component::Interface, component::Interface>
	Creator;

	typedef
	wns::StaticFactory<component::Creator>
	Factory;
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
