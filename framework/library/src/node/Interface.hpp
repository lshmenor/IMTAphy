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

#ifndef WNS_NODE_INTERFACE_HPP
#define WNS_NODE_INTERFACE_HPP

#include <WNS/node/component/FQSN.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Assure.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>

namespace wns{ namespace service {
	class Service;
}}


namespace wns { namespace node {


	/**
	 * @brief Interface of Node
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 */
	class Interface
	{
	public:
		Interface()
		{}

		virtual
		~Interface()
		{}

		/**
		 * @brief Add a Component to the Node
		 *
		 * @note Assures: Name of Component is unique across all nodes
		 * in a simulation.
		 */
		virtual void
		addService(const std::string& name, service::Service* si) = 0;

		/**
		 * @brief Called immedeatly after constructor
		 *
		 * This will be called after the constructor has been
		 * called. The Node can call virtual methods of itself which
		 * is forbidden in the constructor. The probing system is
		 * available at this point
		 */
		virtual void
		startup() = 0;

		/**
		 * @brief Inform all components that world has been created.
		 */
		virtual void
		onWorldCreated() = 0;

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
		onShutdown() = 0;

		/**
		 * @brief Retrieve a (local) Service by its name (automatic typecast)
		 *
		 * Usage:
		 * @includelineno Node::getComponentByName.example
		 */
		template <typename SERVICETYPE>
		SERVICETYPE
		getService(const std::string& serviceName) const
		{
			return getRemoteService<SERVICETYPE>(wns::node::component::FQSN(getName(), serviceName));
		}

		/**
		 * @brief Retrieve a (remote) Service by its name (automatic typecast)
		 *
		 * Usage:
		 * @includelineno Node::getComponentByName.example
		 */
		template <typename SERVICETYPE>
		SERVICETYPE
		getRemoteService(const component::FQSN& fqsn) const
		{
			wns::service::Service* si = getAnyService(fqsn);
			assureType(si, SERVICETYPE);
			// we can't use C-Style downcasts here!
			return dynamic_cast<SERVICETYPE>(si);
		}

		/**
		 * @return ContextProviderCollection for Probes
		 */
		virtual probe::bus::ContextProviderCollection&
		getContextProviderCollection() = 0;

		/**
		 * @brief Name of the Node (also used in the registry)
		 *
		 * @return Name of this node
		 */
		virtual std::string
		getName() const = 0;

		/**
		 * @brief NodeID of the Node
		 *
		 * @return NodeID of this node
		 */
		virtual unsigned int
		getNodeID() const = 0;

		/**
		 * @brief true if both Interfaces are the same (have the same number)
		 *
		 * @return true/false
		 */
		virtual bool
		isEqual(Interface* second) const
		{ assure(second!=NULL,"operand==NULL");
		  return getNodeID()==second->getNodeID(); };

	private:
		/**
		 * @brief Retrieve a Service by its name
		 *
		 * Usage:
		 * @includelineno Node::getComponentByName.example
		 */
		virtual wns::service::Service*
		getAnyService(const component::FQSN& fqsn) const = 0;

		// Disallow Copy Constructor
		Interface(const Interface&);
	};

} // node
} // wns

/**
 * @namespace wns::node
 * @brief Basic building blocks for a simulation : The Node
 *
 * A simulation consists of a number of Nodes (Node), which are
 * communicating with each other. As a simple example consider two Nodes
 * (Node), a Server (WebServer) and a Client (WebBrowser):
 *
 * @dot
 * digraph stacks {
 *    rankdir=LR;
 *    concentrate=true
 *    subgraph cluster1 {
 *      node [shape=record, fontname=Helvetica, fontsize=12];
 *      server [ label = "WebServer|TCP|IP|<e> WiMAX" ];
 *      label = "Node1";
 *    }
 *    subgraph cluster2 {
 *      node [shape=record, fontname=Helvetica, fontsize=12];
 *      client [ label = "WebBrowser|TCP|IP|<e> WiMAX" ];
 *      label = "Node2";
 *    }
 *    server:e -> client:e
 *    client:e -> server:e
 *  }
 * @enddot
 *
 * Each node contains several components that enable communication. In this
 * example components are WiMAX, IP, TCP, Webserver, WebBrowser. Before we
 * continue we will define what we mean when talking about nodes and components:
 *
 * @em Node
 * @li A Node contains components.
 * @li A Node is responsible of creating, destroying and managing its
 * components.
 * @li A Node enables components to connect to each other.
 * @li A Node has a unique name within a simulation.
 *
 * @em Component
 *
 * The component semantics have been borrowed from UML 2.0. In "The Unified
 * Modeling Language Reference Manual, Second Edition" Rumbaugh et.al. state
 * that a component is
 *
 * <em>"A modular part of a system design that hides its implementation behind a
 * set of external interfaces. Within a a system, components satisfying the same
 * interfaces may be substituted freely."</em>
 *
 * This is in accordance with our understanding of a component. In particular we
 * have the following specializations:
 *
 * @li All Components have one parent node.
 * @li All Components have a unique name within the context of their parent
 * node.
 * @li Specific components realize and/or uses services.
 * @li Specific components know how to connect to other specific components that
 * realize required services.
 *
 * A node is only little more than a container for components. The minimal (but
 * very useless) component only has a parent node and a name. It does not offer
 * or require any services. However, a component that does not do anything is
 * not very usefull. Therefore components realize services. These services are
 * realized by depending on other services that are offered by other components
 * (remember ISO/OSI?). How these services look like is out of scope of the
 * component concept. The only requirement on all components is that they
 * implement the component::findOtherComponents method which is called after all
 * components within a node have been created. Here the component needs to find
 * its required services (details see below). But now back to the interesting
 * stuff:
 *
 * The nodes (Node) are communicating via TCP/IP over WiMAX
 * (802.16). Packets will be generated in either WebBrowser or WebServer
 * Component and will travel down the node through the TCP, IP and WiMAX
 * Component in order to be sent to the other Node where they travel the
 * Node up again. The question is now: How to achieve such a setup? What
 * do the config files look like and what are the respective classes in
 * the C++ world?
 *
 * To simplify the example a little further we're going to focus on TCP
 * and IP only. First of all let's have a look how the TCP and the IP
 * Component are going to exchange information/data.
 *
 * @dot
 * digraph structs {
 *   rankdir = LR;
 *   node [shape=record, fontname=Helvetica, fontsize=12];
 *   server [ label = "TCP|IP" ];
 * }
 * @enddot
 *
 * For our example the TCP and IP are going to form a Node. How can we
 * do that? Have a look at the following code snippet:
 *
 * @includelineno Node::TCPIP.example
 *
 * First we're creating a Node (node) and two components (tcp,
 * ip). After that we're putting the two components into the Node. The
 * node contains now the two components (tcp and ip). The three
 * variables nodeConfig, tcpConfig and ipConfig contain the
 * configuration (see @ref wns::pyconfig) for these classes.
 *
 * Let's explore how the Node and those Components work together.
 * The Node has the following interface:
 * - Node::getName() const
 * - Node::addComponent(component::Interface*)
 * - Node::getComponentByName(const std::string&) const
 * - Node::getComponentByOthersComponentsName(COMPONENTTYPE) const
 * - Node::getProbeIDProviderRegistry() const
 *
 * If we take a closer look at the method
 * Node::addComponent(component::Interface*) we can observe that it takes
 * a pointer to a component::Interface as argument. The code detective
 * inside us suggests that TCP and IP must be of type
 * component::Interface. Bingo! So the class hierachy looks something like
 * this:
 *
 * @dot
 * digraph structs {
 *   rankdir = BT;
 *   node [shape=record, fontname=Helvetica, fontsize=12, style=filled, fillcolor=white];
 *   componentInterface [ label = "component::Interface", fillcolor=red ];
 *   ip  [ label = "IP" ];
 *   tcp [ label = "TCP" ];
 *   tcp -> componentInterface [arrowhead = empty];
 *   ip -> componentInterface [arrowhead = empty];
 * }
 * @enddot
 *
 * What does the component::Interface look like?
 * - component::Interface::getName() const
 * - component::Interface::getNode() const
 * - component::Interface::onNodeCreated()
 * - component::Interface::onWorldCreated()
 *
 * However this is only half the truth, because as we can see from the
 * code snippet above the TCP instancs is assigned to a pointer of type
 * TCPInterface. This means there is not only a component::Interface but
 * also a TCPInterface. TCPInterface must be derived from
 * component::Interface otherwise Node::addComponent can't be called with
 * this type.Further, TCP must be derived from TCPInterface otherwise the
 * assignment to TCPInterface would not be possible. Hence, our hierachy looks
 * now like this:
 *
 * @dot
 * digraph structs {
 *   rankdir = BT;
 *   node [shape=record, fontname=Helvetica, fontsize=12, style=filled, fillcolor=white];
 *   componentInterface [ label = "component::Interface" ];
 *   tcpInterface [ label = "TCPInterface", fillcolor=red ];
 *   ipInterface [ label = "IPInterface", fillcolor=red ];
 *   ip  [ label = "IP" ];
 *   tcp [ label = "TCP" ];
 *   tcp -> tcpInterface [arrowhead = empty];
 *   ip -> ipInterface [arrowhead = empty];
 *   tcpInterface -> componentInterface [arrowhead = empty];
 *   ipInterface -> componentInterface [arrowhead = empty];
 * }
 * @enddot
 *
 * The "Interfaces" (IPInterface, TCPInterface, component::Interface) are
 * abstract (which means they contain only definitions of the methods
 * but no implementation). What is the benefit of these Interfaces?
 * Consider the following: TCP will very likely use an IP instance. But
 * it should not rely on one specific implementation. So it will use the
 * IPInterface instead of IP direct:
 *
 * @dot
 * digraph structs {
 *   rankdir = BT;
 *   node [shape=record, fontname=Helvetica, fontsize=12, style=filled, fillcolor=white];
 *   componentInterface [ label = "component::Interface" ];
 *   tcpInterface [ label = "TCPInterface" ];
 *   ipInterface [ label = "IPInterface" ];
 *   ip  [ label = "IP" ];
 *   tcp [ label = "TCP" ];
 *   tcp -> tcpInterface [arrowhead = empty];
 *   tcp -> ipInterface [taillabel = "uses", fontname=Helvetica, fontsize=9, color=red];
 *   ip -> ipInterface [arrowhead = empty];
 *   tcpInterface -> componentInterface [arrowhead = empty];
 *   ipInterface -> componentInterface [arrowhead = empty];
 * }
 * @enddot
 *
 * And we can have more than one IP implementation. As long as it
 * conforms to the IPInterface everything is fine:
 *
 * @dot
 * digraph structs {
 *   rankdir = BT;
 *   node [shape=record, fontname=Helvetica, fontsize=12, style=filled, fillcolor=white];
 *   componentInterface [ label = "component::Interface" ];
 *   tcpInterface [ label = "TCPInterface" ];
 *   ipInterface [ label = "IPInterface" ];
 *   ipv4  [ label = "IPv4", fillcolor=red ];
 *   ipv6  [ label = "IPv6", fillcolor=red ];
 *   tcp [ label = "TCP" ];
 *   tcp -> tcpInterface [arrowhead = empty];
 *   tcp -> ipInterface [taillabel = "uses", fontname=Helvetica, fontsize=9];
 *   ipv4 -> ipInterface [arrowhead = empty ];
 *   ipv6 -> ipInterface [arrowhead = empty ];
 *   tcpInterface -> componentInterface [arrowhead = empty];
 *   ipInterface -> componentInterface [arrowhead = empty];
 * }
 * @enddot
 *
 * Still with me? Good! We still have room for optimization! The classes
 * TCP, IPv4 and IPv6 are derived from component::Interface (through
 * TCPInterface and IPInterface). Since the component::Interface is
 * abstract we have to implement all the methods required by
 * component::Interface (see above which methods are required by
 * component::Interface). They are very similar for all three classes
 * (TCP, IPv4, IPv6). To be exact: they are the same. Lucky as we are,
 * we don't need to implement these methods. There is a default
 * implementation available: Component. To get the implementation from
 * Component we derive TCP, IPv4 and IPv6 from Component:
 *
 * @dot
 * digraph structs {
 *   rankdir = BT;
 *   node [shape=record, fontname=Helvetica, fontsize=12, style=filled, fillcolor=white];
 *   componentInterface [ label = "component::Interface" ];
 *   tcpInterface [ label = "TCPInterface" ];
 *   ipInterface [ label = "IPInterface" ];
 *   ipv4  [ label = "IPv4" ];
 *   ipv6  [ label = "IPv6" ];
 *   tcp [ label = "TCP" ];
 *   component [ label = "Component", fillcolor=red ];
 *   tcp -> tcpInterface [arrowhead = empty];
 *   tcp -> ipInterface [taillabel = "uses", fontname=Helvetica, fontsize=9];
 *   ipv4 -> ipInterface [arrowhead = empty];
 *   ipv6 -> ipInterface [arrowhead = empty];
 *   tcpInterface -> componentInterface [arrowhead = empty];
 *   ipInterface -> componentInterface [arrowhead = empty];
 *   component -> componentInterface [arrowhead = empty];
 *   tcp -> component [arrowhead = empty];
 *   ipv4 -> component [arrowhead = empty];
 *   ipv6 -> component [arrowhead = empty];
 * }
 * @enddot
 *
 * Et voila! We've a way to define interfaces for different types of
 * Components and they can be stored in a Node. Now, how does all this
 * look like in C++? We're going to examine only the TCP class. Let's
 * look a the class hierachy again:
 *
 * @dot
 * digraph structs {
 *   rankdir = BT;
 *   node [shape=record, fontname=Helvetica, fontsize=12, style=filled, fillcolor=white];
 *   componentInterface [ label = "component::Interface" ];
 *   tcpInterface [ label = "TCPInterface" ];
 *   tcp [ label = "TCP" ];
 *   component [ label = "Component" ];
 *   tcp -> tcpInterface [arrowhead = empty];
 *   tcpInterface -> componentInterface [arrowhead = empty];
 *   component -> componentInterface [arrowhead = empty];
 *   tcp -> component [arrowhead = empty];
 * }
 * @enddot
 *
 * <b>A simple TCPInterface</b><br>
 * Assume the following interface has been defined for an TCP component:
 * @includelineno Component::TCPInterface.example
 * Note how we derive from component::Interface here and compare this to
 * the class diagram above. Note further the two new methods that have
 * been defined in the TCPInterface:
 * - TCPInterface::getFreePort()
 * - TCPInterface::getAddressOfIPInstance()
 *
 * These methods are again abstract and need to be implemented.
 *
 * <b>An TCP implementation</b><br>
 * The implementation of TCP might then look like this:
 * @includelineno Component::TCP.example
 * Note how we derive from Component
 * additionally here in order to
 * "implement" the component::Interface. The only thing left is
 * the implementation of TCP::getFreePort(),
 * TCP::getAddressOfIPInstance(), TCP::onNodeCreated() and TCP::onWorldCreated
 * from the component::Interface.
 *
 * Let's see what we can learn from the three methods that have been
 * implemented here:
 *
 * TCP::getFreePort():<br>
 * This one is easy. For testing purpose we return an integer. Nothing
 * special.
 *
 * TCP::getAddressOfIPInstance():<br>
 * This one is a little bit more difficult. From the name we can guess
 * that we need an instance of something conforming to the IPInterface
 * since we need to ask this instance for its address.
 *
 * The question is now: How do we find such an instance? We know that
 * the instance must be part of the same Node that the instance of TCP
 * is currently being part of.
 *
 * At this point the method TCP::onNodeCreated() comes into
 * play. Normally components are not added from outside to a Node but
 * the Node creates all necessary components in its constructor. After
 * all components have been created the method
 * component::Interface::onNodeCreated() of each component is
 * called. Hence, this is the point where a component can try to find
 * other components within its parent Node. You resolve inter Node
 * dependencies only after all Nodes have been constructed and all components
 * have executed their onNodeCreated method. This is the time instance
 * when the onWorldCreated() method is called.
 *
 * The Node provides special methods to retrieve components from itself.

 * <b>Retrieve a Component from a Node</b><br>
 * A Component can be retrieved from a Node by its name (like a
 * IP instance):
 * @includelineno Node::getComponentByName.example
 * Note how the method is informed about the type to be retrieved from
 * the Node. This is necessary since the Node has no idea of the type
 * (it stores component::Interfaces). This is mostly used in
 * component::Interface::onNodeCreated(), i.e. if one component
 * tries to find another component with another type in the same Node.
 */
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
