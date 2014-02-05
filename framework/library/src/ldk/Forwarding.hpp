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

#ifndef WNS_LDK_FORWARDING_HPP
#define WNS_LDK_FORWARDING_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/Processor.hpp>

#include <list>

namespace wns { namespace ldk {

		/**
		 * @brief Implement the Processor interface, forwarding packets without mutation.
		 * @ingroup compoundhandler
		 */
	template <typename USER>
	class Forwarding :
		public virtual Processor<USER>
	{
	public:
		Forwarding() :
				Processor<USER>()
		{}


		~Forwarding()
		{}


		//
		// Processor interface
		//
		virtual void
		processIncoming(const CompoundPtr& /* compound */)
		{
		} // processIncoming



		virtual void
		processOutgoing(const CompoundPtr& /* compound */)
		{
		} // processOutgoing
	};
}}


#endif // NOT defined WNS_LDK_FORWARDING_HPP


