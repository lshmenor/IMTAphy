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

#ifndef WNS_LDK_COMMAND_HPP
#define WNS_LDK_COMMAND_HPP

#include <WNS/simulator/Bit.hpp>
#include <WNS/Assure.hpp>

#include <cstdlib>

namespace wns { namespace ldk {

	/**
	 * @brief Base class for the PCI of a CommandTypeSpecifier.
	 * @ingroup commandtypespecifier
	 *
	 * Several Commands are grouped together using a CommandPool to form
	 * the PCI of a Layer.
	 * <p>
	 * Derive from Command if you are implementing a CommandTypeSpecifier, that needs additional
	 * PCI elements.
	 * <p>
	 * The destructor of a Command will be called, when the containing
	 * CommandPool is deleted.
	 */
	class Command
	{
		friend class CommandProxy;

	public:
		Command() :
			commited(false),
			commandPoolSize(0),
			payloadSize(0)
		{}

		virtual
		~Command()
		{}

		virtual Bit
		getSize() const
		{
			return 0;
		}

	private:
		bool sizeCommited() const
	        {
			return commited;
		}

		Bit getCommandPoolSize() const
		{
			assure(commited, "You may only retrieve the size of the command pool after committing!");
			return commandPoolSize;
		}

		void setCommandPoolSize(const Bit size)
		{
			assure(!commited, "You may not modify the size after commiting!");
			commandPoolSize = size;
		}

		Bit getPayloadSize() const
		{
			assure(commited, "You may only retrieve the size of the payload after committing!");
			return payloadSize;
		}

		void setPayloadSize(const Bit size)
		{
			assure(!commited, "You may not modify the size after commiting!");
			payloadSize = size;
		}
		/**
		 * @brief Calculate the size of the Command.
		 *
		 * Return the size of the Command in bit. Override this when deriving
		 * from Command.
		 */
		void commit()
		{
			assure(!commited, "You may not commit the size of the command twice!");
			commited = true;
		}

		bool commited;
		Bit commandPoolSize;
		Bit payloadSize;

	};

	class EmptyCommand :
		public Command
	{
	public:
		EmptyCommand() :
			local(),
			peer(),
			magic()
		{}

		/**
		 * @brief Local control information that is not transmitted to the peer.
		 *
		 */
		struct {} local;

		/**
		 * @brief Control information that gets encoded and transmitted to the peer.
		 *
		 */
		struct {} peer;

		/**
		 * @brief Information that may only be used to ease simulation implementation.
		 *
		 */
		struct {} magic;
	};

} // ldk
} // wns

#endif // NOT defined WNS_LDK_COMMAND_HPP


