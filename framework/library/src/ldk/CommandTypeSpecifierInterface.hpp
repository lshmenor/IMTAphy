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

#ifndef WNS_LDK_COMMANDTYPESPECIFIERINTERFACE_HPP
#define WNS_LDK_COMMANDTYPESPECIFIERINTERFACE_HPP

#include <WNS/Assure.hpp>
#include <WNS/ldk/CommandProxy.hpp>

namespace wns { namespace ldk {

	class CommandPool;
	class CommandReaderInterface;

	class CopyCommandInterface
	{
	public:
		virtual
		~CopyCommandInterface(){};

		virtual Command*
		copy(const Command*) const = 0;
	};


	namespace fun {
		class FUN;
	}

	/**
	 * @defgroup commandtypespecifier Command Provider Interface
	 * @ingroup ldkaspects
	 * @brief Provide information about the Command required by the FunctionalUnit.
	 *
	 */

	/**
	 * @brief Abstract interface to be implemented by a CommandTypeSpecifier.
	 * @ingroup commandtypespecifier
	 *
	 * Strictly spoken, this is not an Interface, since it
	 * decorates the FunctionalUnit with the PCI-id tagging methods.
	 *
	 * PCI-id tagging methods are nothing you should have to care about. They are just
	 * part of the CommandProxy/Pool internals. Have a look into CommandProxy
	 * to find out more.
	 * <p>
	 * See CommandTypeSpecifier for a templated implementation.
	 *
	 */
	class CommandTypeSpecifierInterface
	{
		friend class CommandProxy;

	public:
		CommandTypeSpecifierInterface() :
			id(invalidID)
		{}

		virtual fun::FUN* getFUN() const = 0; // FIXME(fds) - doesn't belong here. well, maybe it does.

		virtual ~CommandTypeSpecifierInterface() {};

		virtual Command* getCommand(const CommandPool* commandPool) const = 0;
		virtual Command* activateCommand(CommandPool* commandPool) const = 0;

		/**
		 * @brief Create a reply to the given CommandPool.
		 *
		 * A lower layer asks us to create a reply to the given CommandPool. Some
		 * functional units (such as ARQ) need to inject compounds (ACKs) back into the stack as reply
		 * to compounds they received.
		 * <p>
		 * Unfortunately, not every layer has enough information to create a PCI that codes
		 * a valid reply to the original PCI. But it may ask the CommandProxy to construct
		 * a reply for it. The CommandProxy in turn delegates the reply creation to the
		 * functional units right above the questioner.
		 * This is what the default implementation of createReply does, as implemented by
		 * CommandTypeSpecifier<>.
		 * <p>
		 * Layers that do know, how to create a reply can simply create a CommandPool, activate
		 * their Command and fill in the fields to form the reply.
		 */
		virtual CommandPool* createReply(const CommandPool* original) const = 0;

		virtual CopyCommandInterface*
		getCopyCommandInterface() const = 0;

#ifndef NDEBUG
		virtual size_t
		getCommandObjSize() const = 0;
#endif

	protected:
		void setPCIID(unsigned long _id)
		{
			assure( id == invalidID, "this FunctionalUnit has been registered at a Proxy before!");
			id = _id;
		}
	public:
		unsigned long getPCIID() const
		{
			assure( id != invalidID, "this FunctionalUnit has not been registered at a Proxy yet!");
			return id;
		}
	protected:
		bool registeredAtProxy() const
		{
			return id != invalidID;
		}

		/**
		 * @brief Calculate the sizes of the PDU.
		 *
		 * During size calculation, each FunctionalUnit is asked for the size of the PCI and SDU.
		 * Calculation begins with the layer the PDU last visited. To be able to calculate the
		 * sizes, a FunctionalUnit may in turn need to know the sizes as calculated by the FunctionalUnit
		 * right above itself.
		 * <p>
		 * It can do that by simply asking the CommandProxy, giving itself as questioner. The CommandProxy
		 * will then delegate the calculation request to the next higher FunctionalUnit in the path
		 * of the PDU.
		 */
		virtual void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const = 0;
		virtual void commitSizes(CommandPool* commandPool) const = 0;
	private:
		virtual Command* createCommand() const = 0;
		virtual Command* copyCommand(const Command* src) const = 0;
		virtual CommandReaderInterface* getCommandReader(CommandProxy*) = 0;

		unsigned long id;
		static const unsigned long invalidID;
	};

} // ldk
} // wns

#endif // NOT defined WNS_LDK_COMMANDTYPESPECIFIERINTERFACE_HPP


