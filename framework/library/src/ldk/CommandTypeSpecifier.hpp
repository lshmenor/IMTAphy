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

#ifndef WNS_LDK_COMMANDTYPESPECIFIER_HPP
#define WNS_LDK_COMMANDTYPESPECIFIER_HPP

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Connector.hpp>
#include <WNS/ldk/CommandReaderInterface.hpp>

#include <WNS/Assure.hpp>

#include <WNS/osi/PCI.hpp>

#include <vector>
#include <cstdlib>				// NULL, size_t
#include <limits>


namespace wns { namespace ldk {
	class CommandPool;

	/**
	 * @brief Equip a FunctionalUnit with functionality that depends on the type of Command.
	 * @ingroup commandtypespecifier
	 *
	 * Implements CommandTypeSpecifierInterface.
	 * <p>
	 * Decorate a FunctionalUnit with the interface defined by CommandTypeSpecifierInterface
	 * and some state-safe, Command-type aware Command getter methods.
	 * <p>
	 * @param	CommandType	Type of Command for this CommandTypeSpecifier.
	 *
	 */
	template <typename T = EmptyCommand>
	class CommandTypeSpecifier :
		public virtual CommandTypeSpecifierInterface
	{
	public:
		typedef T COMMANDTYPE;

		CommandTypeSpecifier(fun::FUN* _fun) :
				fuNet(_fun)
		{
			assure(fuNet, "No fuNet, my friend. No fuNet.");
			// assure, that every Command contains the following entries
			int satisfyCompiler;
			satisfyCompiler = sizeof(&COMMANDTYPE::local);
			satisfyCompiler = sizeof(&COMMANDTYPE::peer);
			satisfyCompiler = sizeof(&COMMANDTYPE::magic);
		}

		virtual ~CommandTypeSpecifier()
		{}

		CommandTypeSpecifier(const CommandTypeSpecifier& other) :
			CommandTypeSpecifierInterface(),
			fuNet(other.fuNet)
		{
		}

		CommandTypeSpecifier&
		operator=(const CommandTypeSpecifier& other)
		{
			assure(fuNet == other.fuNet, "Assignment is only supported within the same fun");
			fuNet = other.fuNet;
		}

		/**
		 * @brief Extract a reference to the Command of this CommandTypeSpecifier.
		 *
		 * Assure that the Command has been initialized before. If you are implementing a
		 * FunctionalUnit and want to get your Command for the first time, use activateCommand instead.
		 *
		 * @param	commandPool	CommandPool that contains the Command to get
		 *						extracted.
		 * @return	Reference to Command of this CommandTypeSpecifier.
		 *
		 */
		COMMANDTYPE* getCommand(const CommandPool* commandPool) const
		{
			assure(commandPool != NULL, "Invalid argument.");

			Command* command = getFUN()->getProxy()->getCommand(commandPool, this);

			assureType(command, COMMANDTYPE*);
			return static_cast<COMMANDTYPE*>(command);
		}

		/**
		 * @brief Like getCommand(const CommandPool* commandPool) const
		 * but with CompoundPtr instead
		 */
		COMMANDTYPE* getCommand(const CompoundPtr& compound) const
		{
			assure(compound != CompoundPtr(), "Invalid argument.");
			return this->getCommand(compound->getCommandPool());
		}

		/**
		 * @brief Extract a reference to the Command of this CommandTypeSpecifier for initialisation.
		 *
		 * Assure that the Command has not been activated before. Every FunctionalUnit providing a Command
		 * has to activate its Command before anyone (including the FunctionalUnit itself) may access it via
		 * getCommand.
		 *
		 * Calling activateCommand for a given Command will mark it initialised.
		 *
		 * @param	commandPool	CommandPool that contains the Command to get
		 *						extracted.
		 * @return	Reference to Command of this CommandTypeSpecifier.
		 *
		 */
		COMMANDTYPE* activateCommand(CommandPool* commandPool) const
		{
			assure(commandPool != NULL, "Invalid argument.");
			Command* command = getFUN()->getProxy()->activateCommand(commandPool, this);
			assureType(command, COMMANDTYPE*);
			return static_cast<COMMANDTYPE*>(command);
		}

		/**
		 * @brief Commit Command Size.
		 *
		 * calling this instructs the commandProxy to calculate the
		 * current Size of the commandPool and stamp the result into the
		 * command. You will not be able to alter the command's size
		 * afterwards. It also recursively causes all previous commands in the
		 * chain to be "written in stone"
		 *
		 * @param	commandPool	CommandPool that contains the Command to be fixed.
		 *
		 * @return	Reference to Command of this CommandTypeSpecifier.
		 *
		 */
		void commitSizes(CommandPool* commandPool) const
		{
			assure(commandPool, "Invalid argument.");
			getFUN()->getProxy()->commitSizes(commandPool, this);
		}
	public:
		/**
		 * @brief Default createReply implementation.
		 *
		 * The default behaviour is to delegate creation of replies to upper functional units.
		 */
		virtual CommandPool*
		createReply(const CommandPool* original) const
		{
			return getFUN()->getProxy()->createReply(original, this);
		} // createReply

		/**
		 * @brief Default calculateSizes implementation.
		 *
		 * The default implementation delegates calculation
		 * to upper functional units, modifying the result to include the size of
		 * the provided Command.
		 */
		virtual void
		calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
		{
			getFUN()->getProxy()->calculateSizes(commandPool, commandPoolSize, dataSize, this);
			Command* command = getFUN()->getProxy()->getCommand(commandPool, this);

			commandPoolSize += command->getSize();
		} // calculateSizes

		virtual Bit
		getLengthInBits(const CompoundPtr& compound) const
		{
			Bit commandPoolSize;
			Bit sduSize;

			getFUN()->getProxy()->calculateSizes(compound->getCommandPool(), commandPoolSize, sduSize, this);
			return commandPoolSize + sduSize;
		} // getLengthInBits

		virtual fun::FUN*
		getFUN() const
		{
			return fuNet;
		} // getFUN

		virtual CopyCommandInterface*
		getCopyCommandInterface() const
		{
			return new CopyCommand<COMMANDTYPE>;
		}

#ifndef NDEBUG
		virtual size_t
		getCommandObjSize() const
		{
			return sizeof( COMMANDTYPE );
		}
#endif

	private:
		COMMANDTYPE*
		createCommand() const
		{
			return new COMMANDTYPE();
		} // createCommand

		COMMANDTYPE*
		copyCommand(const Command* src) const
		{
			CopyCommand<COMMANDTYPE> f;
			return f.copy(src);
		} // copyCommand


		template<typename COMMAND>
		class CopyCommand :
			virtual public CopyCommandInterface
		{
		public:
			virtual
			~CopyCommand(){}

			virtual COMMAND*
			copy(const Command* src) const
			{
				assureType(src, const COMMAND*);
				return new COMMAND(*(dynamic_cast<const COMMAND*>(src)));
			}
		};

		class CommandReader :
			virtual public CommandReaderInterface
		{
			const unsigned long int id;
			CommandProxy* proxy;

			virtual CommandProxy*
			getProxy() const
			{
				return proxy;
			}

			virtual const unsigned long int
			getPCIID() const
			{
				return id;
			}

		public:
			CommandReader(const unsigned long int _id, CommandProxy* _proxy) :
				id(_id),
				proxy(_proxy)
			{}

			virtual
			~CommandReader(){}

			virtual bool
			commandIsActivated(const CommandPool* commandPool) const
			{
				assure(commandPool != NULL, "Invalid argument.");
				return proxy->commandIsActivated(commandPool, id);
			}

		};

		virtual CommandReaderInterface*
		getCommandReader(CommandProxy* proxy)
		{
			return new CommandReader(this->getPCIID(), proxy);
		}

		/**
		 * @brief fun::FUN we are part of. This will vanish.
		 *
		 */
		fun::FUN* fuNet;
	};

} // ldk
} // wns

#endif // NOT defined WNS_LDK_COMMANDTYPESPECIFIER_HPP


