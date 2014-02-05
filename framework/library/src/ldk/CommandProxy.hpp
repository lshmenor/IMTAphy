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

#ifndef WNS_LDK_COMMANDPROXY_HPP
#define WNS_LDK_COMMANDPROXY_HPP

#include <WNS/container/Registry.hpp>
#include <WNS/Singleton.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/osi/PCI.hpp>

#include <vector>
#include <cstdlib>

namespace wns { namespace ldk {

	class Command;
	class CommandPool;
	class CommandTypeSpecifierInterface;
	class CopyCommandInterface;
	class CommandReaderInterface;

	namespace fun {
		class FUN;
	}

	/**
	 * @brief Manage Commands within a CommandPool and make them accessible.
	 * @ingroup commandtypespecifier
	 * @author Fabian Debus <fds@comnets.rwth-aachen.de>
	 *
	 * Every Functional Unit Network has a CommandProxy that encapsulates
	 * the knowledge about the layout of Commands within a CommandPool.
	 *
	 * @note For a discussion on why the current design is not optimal see
	 * CommandProxy.hpp
	 */
 	class CommandProxy
	{
	public:
		typedef unsigned long int CommandIDType;
	private:

		// During registration, each FunctionalUnit will be tagged with
		// the position within the CommandPool using
		// CommandTypeSpecifierInterface::setPCIID().
		//
		// Later on, each FunctionalUnit can be queried for its Command
		// position within the commandPool by calling
		// CommandTypeSpecifierInterface::getPCIID().

		friend class CommandReaderInterface;
		friend class CommandPool;

		/**
		 * @brief Container for CommandTypeSpecifiers
		 */
		typedef std::vector<CommandTypeSpecifierInterface*>
		CommandTypeSpecifierContainer;

		/**
		 * @brief Container for bool
		 */
		typedef std::vector<bool>
		BoolContainer;

 		/**
		 * @brief Mapping of role name to CommandIDType
		 */
		typedef wns::container::Registry<std::string, CommandIDType>
		CommandIDRegistry;

 		/**
		 * @brief Mapping of CommandIDType to cloned CopyCommand Object
		 */
		typedef wns::container::Registry<CommandIDType, CommandReaderInterface*, wns::container::registry::DeleteOnErase>
		CommandReaderRegistry;

 		/**
		 * @brief Mapping of CommandIDType to cloned CommandTypeSpecifier
		 */
		typedef wns::container::Registry<CommandIDType, CopyCommandInterface*, wns::container::registry::DeleteOnErase>
		CopyCommandInterfaceRegistry;

		wns::logger::Logger logger;

	public:
		CommandProxy() :
			logger("default","default"),
			commandTypeSpecifiers(),
			commandTypeSpecifierCloned()
		{
            wns::simulator::getResetSignal()->connect(&wns::ldk::CommandProxy::clearRegistries); 
		}

		CommandProxy(const wns::pyconfig::View& config);

		~CommandProxy();

		/**
		 * @brief Add a CommandTypeSpecifier with a certain role name
		 *
		 * @pre Functional Unit must be registered at a FUN
		 *
		 * This makes a Functional Unit known to the CommandProxy and
		 * the global CommandIDRegistry with its role name.
		 */
		void
		addFunctionalUnit(
			const std::string& commandName,
			CommandTypeSpecifierInterface* functionalUnit);

		/**
		 * @brief Remove a CommandTypeSpecifier
		 *
		 * De-register a Functional Unit from the CommandProxy. The FU
		 * can't operate on the CommandPool after this.
		 */
		void
		removeFunctionalUnit(const std::string& commandName);

		/**
		 * @brief Create a fresh CommandPool for the Functional Unit
		 * Network.
		 *
		 * A CommandPool contains the Commands of all Functional Units
		 * registered at the CommandProxy.
		 * <p>
		 * The returned CommandPool has a reference count of 0.
		 */
		CommandPool*
		createCommandPool(const fun::FUN* origin = NULL) const;

		/**
		 * @brief Ask functional units to create a reply for their PCIs.
		 *
		 * Some functional units need to send a reply to their
		 * peer-entity, but they have no information on how routing of
		 * compounds is done, since routing may need partial
		 * modification of the Commands of other functional units.
		 * <p>
		 * One example for such a FunctionalUnit is an ARQ
		 * implementation, that needs to inject ACK compounds back into
		 * the stack. It does not have any information itself on how to
		 * send a PDU to the originator of the received PDU.
		 * <p>
		 * createReply asks the CommandProxy to create a PCI as a reply
		 * to the original CommandPool for the questioner. The
		 * CommandProxy itself will ask the FunctionalUnit which
		 * activated its Command before the questioner to create a reply
		 * (which in turn could ask the proxy and so on).
		 * <p>
		 * If none of the functional units allocated a new CommandPool,
		 * the proxy, when asked by the FunctionalUnit which activated
		 * its Command at first, will create a CommandPool. This
		 * CommandPool will then be passed back through all the
		 * createReply calls and each of the involved functional units
		 * should activate the Command and fill it accordingly to make
		 * it a reply to the original Command.
		 */
		CommandPool*
		createReply(
			const CommandPool* original,
			const CommandTypeSpecifierInterface* questioner) const;

		/**
		 * @brief Calculate the size of a PDU from a given point of view.
		 *
		 * Every FunctionalUnit in a Layer stack may mutate the PDU it
		 * receives and change its sizes (PCI and SDU size). Thus if you
		 * ask for the size of a PDU, the answer may depend on the
		 * position and kind of the FunctionalUnit you ask.
		 * <p>
		 * Every FunctionalUnit in turn only knows how to calculate the
		 * sizes of a PDU based on the sizes the next higher
		 * FunctionalUnit provides.
		 * <p>
		 * To ask for the sizes of a PDU, you may provide a
		 * FunctionalUnit as questioner to describe the point of
		 * view. The answer will be the sizes as calculated by the
		 * FunctionalUnit right above the questioner.
		 * <p>
		 * @note Internally, CommandProxy simply delegates the size
		 * calculation request to the FunctionalUnit right above the
		 * questioner (using
		 * CommandTypeSpecifierInterface::calculateSizes). The
		 * FunctionalUnit in turn may choose to delegate the size
		 * calculation request back to the proxy with itself as
		 * questioner.
		 */
		void
		calculateSizes(
			const CommandPool* commandPool,
			Bit& commandPoolSize, Bit& sduSize,
			const CommandTypeSpecifierInterface* questioner = NULL) const;

		/**
		 * @brief 
		 */
		void
		commitSizes(CommandPool* commandPool,
			    const CommandTypeSpecifierInterface* commiter = NULL) const;

        /**
         * @brief Commit the current sizes
         */
        void
        commitSizes(CommandPool* commandPool,
                    const CommandIDType& id) const;

		/**
		 * @brief Return a reference to a Command instance within a CommandPool.
		 *
		 * Only activated commands may be retrieved. Trying to retrieve a Command that has not been
		 * activated causes the program to abort.
		 */
		Command*
		getCommand(
			const CommandPool* commandPool,
			const CommandTypeSpecifierInterface* kind) const;

		/**
		 * @brief Return the nth Command within a CommandPool.
		 */
		Command*
		getCommand(const CommandPool* commandPool, CommandIDType n) const;

		/**
		 * @brief Return a reference to a Command instance within a CommandPool.
		 *
		 * Only activated commands may be retrieved. Trying to retrieve a Command that has not been
		 * activated causes the program to abort.
		 */
		template <typename COMMANDTYPE>
		COMMANDTYPE*
                getCommand(const CommandPool* commandPool, const std::string& role) const
		{
			assure(this->getCommandIDRegistry().knows(role), "Argument for unknown role="<<role<<" requested.");
			Command* theCommand = this->getCommand(commandPool, this->getCommandIDRegistry().find(role));
			assureType(theCommand, COMMANDTYPE*);
			return dynamic_cast<COMMANDTYPE*>(theCommand);
		} // getCommand

		/**
		 * @brief Return a reference to a Command Reader for the given commandName
		 *
		 */
		CommandReaderInterface*
                getCommandReader(const std::string& role) const;

		/**
		 * @brief Activate and return a Command.
		 *
		 * Commands can only get activated once. Activating a Command
		 * twice will cause the program to abort.
		 */
		static Command*
		activateCommand(
			CommandPool* commandPool,
			const CommandTypeSpecifierInterface* kind);

        /**
         * @brief Activate and return a Command.
         *
         * Commands can only get activated once. Activating a Command
         * twice will cause the program to abort.
         */
        Command*
        activateCommand(
            CommandPool* commandPool,
            const CommandIDType& id);

		/**
		 * @brief Return true if the given Command is activated.
		 *
		 * If in doubt, do \b not use this method. A real protocol
		 * implementation has no means of retrieving such
		 * information. The protocol has to provide enough information
		 * to infer whether a command is activated.
		 */
		bool
		commandIsActivated(
			const CommandPool* commandPool,
			const CommandTypeSpecifierInterface* kind) const;

		/** @brief */
		bool
		commandIsActivated(
			const CommandPool* commandPool,
			const CommandIDType& id) const;

		/**
		 * @brief Copy the content a CommandPool to another CommandPool.
		 */
		void
		copy(CommandPool* dst, const CommandPool* src) const;

		/**
		 * @brief Perform a partial copy of the content of a CommandPool to another CommandPool.
		 *
		 * Copy only those Commands that have been activated upto the
		 * time where the initiator activated its Command.
		 * <p>
		 * This method has to be used, when bridging to avoid an invalid
		 * reactivation of Commands that have been activated already in
		 * an earlier run through a protocol stack.  Suppose
		 * FunctionalUnit A receives an incoming PDU and wants to inject
		 * a copy of the PDU back into itself. If FunctionalUnit A would
		 * make a full copy of the CommandPool, a FunctionalUnit B that
		 * is below A would have seen the PDU before and the Command of
		 * FunctionalUnit B would have been already activated. When the
		 * PDU reaches FunctionalUnit B the second time, there is no way
		 * (and should not be) for FunctionalUnit B to tell if it has
		 * activated the Command already. Since activation of an already
		 * activated Command is considered a defect, an exception is
		 * raised.
		 */
		void
		partialCopy(
			const CommandTypeSpecifierInterface* initiator,
			CommandPool* dst, const CommandPool* src) const;

		/** @brief This method should ONLY be used from within
		 * unitTests.
		 *
		 * It resets the static registries administered in the
		 * CommandProxy to avoid name clashes when different tests
		 * register different commandTypeSpecifiers under the same
		 * commandName. 
		 */
		static void
		clearRegistries();

            /**
             * @brief Show list of all known command names.
             */
            std::string
            printCommandIDRegistryKeys()
            {
                return dumpCommandIDRegistry();
            }

#ifndef NDEBUG
		size_t
		getCommandObjSize(const CommandIDType& id) const;
#endif
	private:
		/**
		 * @brief Call destructors of Commands within the CommandPool.
		 */
		void
		cleanup(CommandPool* commandPool) const;

		/** @brief */
		wns::ldk::Command*
		commit(CommandPool* commandPool,
		       const CommandTypeSpecifierInterface* commiter = NULL) const;

		/**
		 * @brief Returns a CommandTypeSpecifier according to its ID
		 */
		const CommandTypeSpecifierInterface*
		getCommandTypeSpecifier(CommandIDType id) const;

		/**
		 * @brief Returns the CommandTypeSpecifier above the questioner.
		 */
		const CommandTypeSpecifierInterface*
		getNext(
			const CommandPool* commandPool,
			const CommandTypeSpecifierInterface* questioner) const;

		CommandTypeSpecifierContainer commandTypeSpecifiers;

		BoolContainer commandTypeSpecifierCloned;

		static CommandIDType serial;

		static CommandIDRegistry&
		getCommandIDRegistry();

		static std::string
		dumpCommandIDRegistry();

		static CopyCommandInterfaceRegistry&
		getCopyCommandRegistry();

		static CommandReaderRegistry&
		getCommandReaderRegistry();

	};

} // ldk
} // wns

#endif // NOT defined WNS_LDK_COMMANDPROXY_HPP


