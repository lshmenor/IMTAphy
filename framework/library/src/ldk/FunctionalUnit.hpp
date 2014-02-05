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

#ifndef WNS_LDK_FUNCTIONALUNIT_HPP
#define WNS_LDK_FUNCTIONALUNIT_HPP

#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/CompoundHandlerInterface.hpp>
#include <WNS/ldk/CommandTypeSpecifierInterface.hpp>
#include <WNS/ldk/HasReceptorInterface.hpp>
#include <WNS/ldk/HasConnectorInterface.hpp>
#include <WNS/ldk/HasDelivererInterface.hpp>
#include <WNS/ldk/Receptor.hpp>
#include <WNS/ldk/Connector.hpp>
#include <WNS/ldk/Deliverer.hpp>
#include <WNS/ldk/ConnectorRegistry.hpp>
#include <WNS/ldk/ReceptorRegistry.hpp>
#include <WNS/ldk/DelivererRegistry.hpp>

#include <WNS/Cloneable.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/ldk/FUNConfigCreator.hpp>
#include <WNS/PythonicOutput.hpp>


#include <string>

namespace wns { namespace ldk {
	class ILayer;

	/**
	 * @brief Abstract base class for functional units.
	 *
	 * A FunctionalUnit derives virtually from four interfaces. Each of these
	 * interfaces represents one of the four aspects of a FunctionalUnit.
	 * When implementing your own FunctionalUnit, each of these interfaces
	 * has to be implemented somewhere in the derivation hierarchy.
	 */
	class FunctionalUnit :
        public virtual CompoundHandlerInterface<FunctionalUnit>,
        public virtual HasConnectorInterface,
        public virtual HasReceptorInterface,
        public virtual HasDelivererInterface,
        public virtual ConnectorRegistry,
        public virtual ReceptorRegistry,
        public virtual DelivererRegistry,
		public virtual CommandTypeSpecifierInterface,
		public virtual CloneableInterface,
		public virtual PythonicOutput
	{
	public:
		friend class CommandProxy;

		FunctionalUnit() :
			name("None")
		{}

		virtual ~FunctionalUnit()
		{}

		/**
		 * @brief Connect functional units.
		 *
		 * @param	that	FunctionalUnit below this FunctionalUnit.
		 * @return 			that
		 *
		 * The reason for returning the parameter is to have a shortcut
		 * for building FU connect chains
		 * @code
		 *   upper
		 *        ->connect(inBetween)
		 *        ->connect(lower);
		 * @endcode
		 *
		 * All connect methods (connect, upConnect, downConnect) are performed on
		 * the upper FU, with the connectee (the parameter /that/) specifying the
		 * FU to connect below. <br>
		 *
		 * This may look odd at first when performing a->upConnect(b) since it
		 * establishes a connection in the incoming data flow only (from b to
		 * a.)  OTOH, the current choice provides us a set of connect methods
		 * (connect, upConnect, downConnect) sharing the same semantics. <br>
		 *
		 * @section impl_details Implementation Details
		 * The connect mechanism follows loosely the NVI pattern as described in
		 * Sutter, C++ Coding Standards, Rule 39 (page 68): <br>
		 *
		 * The public interface for inter FU connections consists of the
		 * (templated) methods connect, downConnect and upConnect. These are the
		 * methods you will call from outside. The virtual counterparts are
		 * _connect, _downConnect and _upConnect. Reimplement these if you need
		 * to change default behavior. <br>
		 *
		 * Currently the virtual methods are /not/ protected, but that may
		 * change (they are not virtual right now due to the laziness of the
		 * initial author of the dispatching FUs.) <br>
		 *
		 * The last method concerning FU interconnections is whenConnecting. As
		 * we have seen, the connector may reimplement the _connect, _upConnect,
		 * _downConnect methods to realize special behavior, but the connectee
		 * can't do so yet. whenConnecting enables the connectee to do so. The
		 * connectee may reimplement whenConnecting to realize special behavior
		 * when another FU is about to connect to it.
		 *
		 * <ol>
		 * <li> User calls a->connect(b)
		 * <li> a::connect calls b->whenConnecting to retrieve the
		 *      actual connection partner c. From now on, this FU c is used as connection
		 *      partner instead of b (the default implementation of whenConnecting
		 *      returns /this/.)
		 * <li> a::connect then calls a->_connect(c) to perform the actual connection.
		 * </ol>
		 *
		 * @section links connect methods and Links
		 *
		 * <ul>
		 * <li> a->connect(b) modifies a::Connector, b::Receptor and b::Deliverer.
		 * <li> a->downConnect(b) modifies a::Connector and b::Receptor.
		 * <li> a->upConnect(b) modifies b::Deliverer.
		 * </ul>
		 *
		 * @see wns::ldk::Connector, wns::ldk::Receptor and wns::ldk::Deliverer.
		 */
		template <typename T>
                T* connect(T* that, const std::string& srcPort = "SinglePort", const std::string& dstPort = "SinglePort")
		{
			FunctionalUnit* connectee = that->whenConnecting();
                        doConnect(connectee, srcPort, dstPort);
			return that;
		}

		/**
		 * @brief Connect functional units in the outgoing data flow only.
		 *
		 * @param	that	FunctionalUnits below this FunctionalUnit.
		 * @return			that
		 *
		 * @see connect for a description of FU interconnections.
		 */
		template <typename T>
		T* downConnect(T* that, const std::string& srcPort = "SinglePort", const std::string& dstPort = "SinglePort")
		{
			FunctionalUnit* connectee = that->whenConnecting();
			doDownConnect(connectee, srcPort, dstPort);
			return that;
		}

		/**
		 * @brief Connect functional units in the incoming data flow only.
		 *
		 * @param	that	FunctionalUnit below this FunctionalUnit.
		 * @return			that
		 *
		 * @see connect for a description of FU interconnections.
		 */
		template <typename T>
		T* upConnect(T* that, const std::string& srcPort = "SinglePort", const std::string& dstPort = "SinglePort")
		{
			FunctionalUnit* connectee = that->whenConnecting();
			doUpConnect(connectee, srcPort, dstPort);
			return that;
		}

		/**
		 * @brief Resolve inter-FU dependencies.
		 *
		 * Often FunctionalUnits require the collaboration with other
		 * FunctionalUnits to accomplish their task. For the collaboration
		 * between FunctionalUnits we coined the term "friendship".  <br>
		 *
		 * The onFUNCreated method is a hook being called after the complete FUN
		 * has been set up. FunctionalUnits requiring the collaboration with
		 * other FunctionalUnits should resolve these dependencies by
		 * implementing this hook accordingly.  <br>
		 *
		 * Finding friends is usually done calling
		 * wns::ldk::fun::FUN::findFriend.  <br>
		 *
		 * When building reconfigurable FUNs, this method may get called
		 * multiple times (e.g., after each FUN reconfiguration). Keep that in
		 * mind when allocating resources in your onFUNCreated implementation.
		 */
		virtual void
		onFUNCreated()
		{}

                virtual void
                onShutdown()
                {}

		/**
		 * @brief Return the FunctionalUnit holding the connector set for this FunctionalUnit.
		 *
		 * whenConnecting is used during fun::FUN setup as part of the connect
		 * mechanism. The default implementation provided here are perfectly
		 * fine for most cases.
		 *
		 * Some units need to react differently on requests of other units depending
		 * on the identity of that other unit. The only solution without extending
		 * the interfaces and without borrowing the COMEFROM statement from another
		 * popular high level programming language... is to create different contacts.
		 *
		 * A motivating example (actually the reason for all this):
		 *
		 * @section  The Dispatcher
		 *
		 *   The dispatcher is a functional unit having multiple other units in
		 *   its receptor/deliverer sets (they must be equal), but only one unit
		 *   in its connector set.  It promises to deliver every compound to the
		 *   same unit that it received it from.
		 *
		 *   Unfortunately, there is no way to tell the caller from a doSendData
		 *   method call. The dispatcher needs to store an opcode in its command
		 *   to remember to whom the command has to be delivered to.
		 *
		 *   The idea is to create an additional FU for every FU connecting from above
		 *   (remember that we only allow full connects from above). When connecting
		 *   from above, during connection setup a new FU has to be created, that
		 *   gets connected and activates the dispatcher command with the right
		 *   opcode.
		 *
		 * @section Details
		 *
		 * To allow the connectee to insert a newly created FU in between, the
		 * connect methods (connect, downConnect, upConnect) do not operate on
		 * the FUs involved in the connect method calls directly. Instead they
		 * let the connectee specify a FU to connect to using the whenConnecting
		 * method.
		 *
		 *
		 * @see connect for a description of FU interconnections.
		 */
		 virtual FunctionalUnit*
		whenConnecting()
		{
			return this;
		} //  whenConnecting

		/**
		 * @brief Set the name of the FU
		 */
		virtual void
		setName(std::string _name)
		{
			name = _name;
		}

		/**
		 * @brief Get the name of the FU
		 */
		virtual std::string
		getName() const
		{
			return name;
		}

	protected:
		/**
		 * @brief Returns the name and the type like this: name (type)
		 *
		 * @warning This might only be used for debugging (in
		 * Messages).
		 */
		virtual std::string
		doToString() const
		{
			return this->getName() + " (" + PythonicOutput::doToString() + ")";
		}

	private:
		/**
		 * @brief Connect functional units. Virtual backend.
		 *
		 * Implement this method to modify the connect behavior in both incoming
		 * and outgoing data flows.
		 *
		 * @warning Do not call this method.
		 *
		 * In almost any case it is not wise to call this method and it may be prohibited
		 * in future ldk versions.
		 *
		 * @see connect for a description of FU interconnections.
		 */
		virtual void
		doConnect(FunctionalUnit* that, const std::string& srcPort, const std::string& dstPort);

		/**
		 * @brief Connect functional units in the outgoing data flow only. Virtual backend.
		 *
		 * Implement this method to modify the connect behavior in the outgoing data
		 * flow.
		 *
		 * @warning Do not call this method.
		 *
		 * In almost any case it is not wise to call this method and it may be prohibited
		 * in future ldk versions.
		 *
		 * @see connect for a description of FU interconnections.
		 */
		virtual void
		doDownConnect(FunctionalUnit* that, const std::string& srcPort, const std::string& dstPort);

		/**
		 * @brief Connect functional units in the incoming data flow only. Virtual backend.
		 *
		 * Implement this method to modify the connect behavior in the incoming data
		 * flow.
		 *
		 * @warning Do not call this method.
		 *
		 * In almost any case it is not wise to call this method and it may be prohibited
		 * in future ldk versions.
		 *
		 * @see connect for a description of FU interconnections.
		 */
		virtual void
		doUpConnect(FunctionalUnit* that, const std::string& srcPort, const std::string& dstPort);

		std::string name;
	};
	typedef FUNConfigCreator<FunctionalUnit> FunctionalUnitCreator;
	typedef wns::StaticFactory<FunctionalUnitCreator> FunctionalUnitFactory;

	/**
	 *
	 * @page FunctionalUnitPage How to implement a FunctionalUnit
	 *
	 * @section functionalunitcommandpool Providing a custom PCI.
	 * FunctionalUnits in the WNS are the componets of a FUN. Several functionalUnits
	 * arranged and connected together become the FUN (see also
	 * \ref LayerPage). Usually a functionalUnit needs to send functionalUnit specific
	 * information like sequence numbers to its counterpart it is communicating
	 * with. This is performed by embedding information in the Command of the
	 * functionalUnit. Every FUN holds a CommandProxy, which is the common access
	 * point of a CommandPool, which is literally the PCI of the
	 * Layer. Every functionalUnit that is derived from the CommandTypeSpecifier may ask
	 * the CommandProxy to activate its Command entry in the CommandPool and
	 * access functionalUnit specific data in the Command.
	 *
	 * To define a custom functionalUnit we declare our custom Command that we
	 * will use later for our functionalUnit.
	 *
	 * @include FunctionalUnit:FunkyPCIDeclaration.example
	 *
	 * In this example the Command contains two members: somethingFunky and a
	 * pointer to a long, destructorCalled. Further a constructor, a destructor
	 * and the assignment operator are declared and defined. The getSize()
	 * method returns the size of the Command.
	 *
	 * This FunkyCommand shall be the Command that is handled by the FunkyFunctionalUnit:
	 *
	 * @include FunctionalUnit:FunkyFunctionalUnitDeclaration.example
	 *
	 * The FunkyFunctionalUnit is derived from the CommandTypeSpecifier and the functionalUnit has
	 * to handle FunkyCommands. Like almost all functionalUnits the FunkyFunctionalUnit has an
	 * upper and a lower interface. Incoming and outgoing Compounds are handled by
	 * the methods doOnData() and doSendData(), respectively. Here you have the
	 * possibility to alter the Compound and especially the previously defined
	 * FunkyCommand. This is the place to write or read custom data like sequence
	 * numbers. At the end of the doOnData() method the Compound is given to the
	 * next higher functionalUnit by calling its doOnData(). Resepctively downgoing
	 * packets are given to the next lower functionalUnit by calling doSendData() of the
	 * next lower functionalUnit. To learn more about interacting functionalUnits see \ref
	 * LayerPage.
	 *
	 * The wakeup() method is called by lower functionalUnits if they are
	 * able to receive new compounds from higher functionalUnits. This is only the case if
	 * the lower functionalUnit is able to block the packet flow. Finally the
	 * isAccepting() method is called immediately before a new PDU arrives at
	 * the doSendData() method. Here the functionalUnit is able to block the packet
	 * flow.
	 *
	 * @section functionalunitinterface Choosing alternative interfaces.
	 *
	 * In many cases functionalUnits have similar functions. Therefore a set of
	 * functionalUnits is already defined that provides a framework that limits your
	 * implementation to the essential operations of your functionalUnit. Before
	 * implementing a FunctionalUnit, you may choose from a set
	 * of interfaces that matches the behaviour of your FunctionalUnit best.
	 *
	 * There are currently three different interfaces to choose from:
	 *
	 * @li Processor
	 * @li Delayed
	 * @li (PDUHandler)
	 *
	 * Use the Processor interface, whenever your FunctionalUnit has no
	 * internal storage for compounds, performs mutation only and forwards
	 * compounds without delay.
	 *
	 * Use the Delayed interface, whenever your FunctionalUnit has internal
	 * storage, wants to inject new compounds into the FunctionalUnit stack or
	 * wants to drop compounds.
	 *
	 * @note When starting to work with functional units, you usually will not need
	 * to implement the CompoundHandlerInterface directly. Internally,
	 * Processor and Delayed are only adaptors, that implement the
	 * CompoundHandlerInterface for you. Especially the burden of implementing
	 * intra-FUN flow control is relieved from you.
	 *
	 * For a documentation of the methods to implement for each of the
	 * interfaces, have a look at \ref compoundhandler "PDU Handler".
	 *
	 * @section functionalunitsize Participating in size calculations.
	 * @todo documentation on how to influence size calculation
	 *
	 * @section functionalunitreply Creating a reply.
	 * @todo documentation on how to create replies
	 *
	 */


/**
 *@page BuildingSub Example for How to Build a FunctionalUnit
 *
 * @section Example How to build a FunctionalUnit in Winprost

 * In order to be used by Winprost your functionalUnit or FU should be added in
 * $TESTBED/modules/dll/WinProSt--main--0.2/pyconfig::Parser/winprost/Layer2.py
 * Then it has to be created and connected with the other FU's, for example in the
 * buildFUN() :
 * \code
 * newFunctionalUnit = Node( 'NewFunctionalUnit', self.newFunctionalUnit)
 * \endcode
 * Then in .fuNet.setFunctionalUnits() your functionalUnit needs to be added:
 * \code
 * crc,
 * newFunctionalUnit,
 * lowerSAR,
 * \endcode
 * and connected :
 *\code
 * crc.connect(newFunctionalUnit)
 * newFunctionalUnit.connect(lowerSAR)
 * lowerSAR.connect(padding)
 * \endcode
 *
 * Here the CRC-FU is connected with your newly created functionalUnit, which after that is connected to the lowerSAR-FU
 * Also this lines schould exist:
 * \code
 * #
 * # NewFunctionalUnit
 * #
 * newFunctionalUnit = wns.ldk.NewFunctionalUnit.NewFunctionalUnit()
 * \endcode
 *
 * The implementation of fun::FUN can be found in
 * $TESTBED/wns/libwns--main--1.0/src/ldk/fun::FUN.cpp.
 * Where one can find out how fun::FUN works, what kind of methods it has and so on.
 *
 * For your help you can use
 * $TESTBED/wns/libwns--main--1.0/src/ldk/NewFunctionalUnit/NewFunctionalUnit.cpp
 * as a template to create your FunctionalUnit.  After that you can
 * create your *.cpp and *.hpp files and also the tests for them.
 * You should include them in the namespace wns::ldk because in this way it can be used
 * by any module.

 * As a template for the tests you can use
 * $TESTBED/wns/libwns--main--1.0/src/ldk/NewFunctionalUnit/tests/NewFunctionalUnitTest.cpp.
 * But there are some important things to be added like in
 * NewFunctionalUnitTest::testUplink(),
 * where you should add these line :
 * \code
 * newFU->activateCommand(compound->getCommandPool());
 * \endcode
 * and thus activate the command in order to use it, e.g. write in it.
 * All this is needed, because the commands are activated automatically only
 * when a top FU wants to use them.
 * When a compound comes from another fun::FUN the commands are not activated automatically.

 * For compiling first use  the command ./playground.py --install=dbg
 * This has to be written in order to create the so called sandbox.
 * Then with Scons you can compile faster.
 * In the file
 * $TESTBED/wns/libwns--main--1.0/config/libfiles.py
 * all the files that need to be compiled have to be checked in.

 * Everything that you need for the tests is in
 * $TESTBED/tests/unitTests. For a final you have to  made
 * $TESTBED/wns/libwns--main--1.0/pyconfig::Parser/wns/NewFunctionalUnit.py
 * which all in all contains the following important lines:
 * \code
 *  __plugin__ =
 * "wns.ldk.NewFunctionalUnit" name = "NewFunctionalUnit"
 * \endcode
 * This is your configuration file.

 * Enjoy!!
 */





}}


#endif // NOT defined WNS_LDK_FUNCTIONALUNIT_HPP


