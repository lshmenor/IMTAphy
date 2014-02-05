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

#ifndef WNS_NODE_NODE_HPP
#define WNS_NODE_NODE_HPP

#include <WNS/node/Interface.hpp>
#include <WNS/node/component/FQSN.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/container/Registry.hpp>
#include <WNS/node/Registry.hpp>

#include <map>
#include <list>
#include <algorithm>
#include <functional>
#include <stdint.h>

namespace wns { namespace service {
	class Service;
}}

namespace wns { namespace node {

	namespace component {
		class Interface;
	}

	namespace tests{
		class NodeTest;
	}

	/**
	 * @brief Contains various Components to form a network element in a
	 * simulation
	 *
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 * @author Daniel Bltmann <dbn@comnets.rwth-aachen.de>
	 *
	 * For an overview please see @ref wns::node
	 *
	 */
	class Node :
		virtual public Interface
	{
		// The test needs to reset the list of registered
		// nodeIDs to properly test if duplicate ID detection
		// works.
		friend class wns::node::tests::NodeTest;

	protected:
		/**
		 * @brief Container with name of and reference to the Service
		 */
		typedef wns::container::Registry<std::string, service::Service*> ServiceContainer;

		/**
		 * @brief Container for Components
		 */
		typedef std::list<component::Interface*> ComponentContainer;

	public:
		/**
		 * @param[in] registry A node will register itself at this
		 * registry with the name provided in pyco
		 *
		 * @param[in] pyco contains the Node configuration as well as
		 * all Component configurations
		 *
		 * The Node will create all necessary Components and ask them to
		 * connect to each other.
		 */
		explicit
		Node(Registry* registry, const pyconfig::View& pyco);

		virtual
		~Node();

		/**
		 * @brief Component will be stored under the name ci->getName()
		 *
		 * @note Complexity: O(log(N)), N = number of Services
		 *
		 * @note Assures: Name of Service is unique across all nodes
		 * in a simulation.
		 */
		virtual void
		addService(const std::string& name, service::Service* si);

        /**
        * @brief Called immedeatly after constructor
        *
        * This will be called after the constructor has been
        * called. The Node can call virtual methods of itself which 
        * is forbidden in the constructor. The probing system is
        * available at this point
        */
        virtual void
        startup();

		/**
		 * @brief Inform all components that world has been created.
		 */
		virtual void
		onWorldCreated();

		/**
		 * @brief Called before simulation will finally shutdown
		 *
		 * @note All "now" events are processed will be processed event
		 * after this method is called.
		 *
		 * @note Destructor of Node will be called after all events are
		 * processed.
		 */
		virtual void
		onShutdown();

		/**
		 * @return ContextProviderCollection for Probes
		 */
		virtual probe::bus::ContextProviderCollection&
		getContextProviderCollection();

		/**
		 * @brief Name of the Node (also used in the registry)
		 *
		 * @return Name of this node
		 */
		std::string
		getName() const;

        /**
         * @brief NodeID of the Node
         *
         * @return NodeID of this node
         */
        unsigned int
        getNodeID() const;

	protected:
		/**
		 * @brief Retrieve a Service by its FQSN
		 *
		 * Usage:
		 * @includelineno Node::getComponentByName.example
		 */
		virtual wns::service::Service*
		getAnyService(const component::FQSN& name) const;

		/**
		 * @brief Components are stored in this container
		 */
		ServiceContainer localServices;

		/**
		 * @brief Container to hold the node-wide
		 */
		probe::bus::ContextProviderCollection contextProviderRegistry;

        /**
         * @brief The name of this Node
         */
		std::string name;

        /**
         * @brief The NodeID of this Node
        */
        unsigned int nodeID;

        /**
         * @brief The Python configuration of this node
        */
        wns::pyconfig::View config;

		/**
		 * @brief Logger instance
		 */
		logger::Logger log;

		/**
		 * @brief pointer to the node registry
		 */
		Registry* globalNodes;

		/**
		 * @brief Components are kept in a list in order to able to
		 * destroy them when finished
		 */
		ComponentContainer localComponents;

		/**
		 * @brief List of allocated ids
		 *
		 * NodeIds created within the configuration. This is fine,
		 * but we need to double check. If an ID is configured
		 * that is already recorded here, we throw.
		 */
		static std::list<unsigned long int> ids;

	};

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
