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

#ifndef WNS_LDK_FUNCONFIGCREATOR_HPP
#define WNS_LDK_FUNCONFIGCREATOR_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>

namespace wns { namespace ldk {

	namespace fun {
		class FUN;
	}

    /**
	 * @brief Creator implementation to be used with StaticFactory.
	 *
	 * Useful for constructors with a fun::FUN and pyconfig::View parameter.
	 *
	 */
	template <typename T, typename KIND = T>
	struct FUNConfigCreator :
		public FUNConfigCreator<KIND, KIND>
	{
		virtual KIND* create(fun::FUN* fuNet, const wns::pyconfig::View& config)
		{
			return new T(fuNet, config);
		}
	};

	template <typename KIND>
	struct FUNConfigCreator<KIND, KIND> //:
	// public wns::Creator<KIND, KIND>
	{
	public:
		virtual KIND* create(fun::FUN*, const wns::pyconfig::View&) = 0;

		virtual ~FUNConfigCreator()
		{}
	};

}}


#endif // NOT defined WNS_LDK_FUNCONFIGCREATOR_HPP


