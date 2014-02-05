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

#ifndef WNS_LDK_KEY_HPP
#define WNS_LDK_KEY_HPP

#include <WNS/SmartPtr.hpp>

#include <WNS/ldk/Compound.hpp>

#include <WNS/ldk/FUNConfigCreator.hpp>

#include <string>

namespace wns { namespace ldk {
	class ILayer;

	class Key :
		virtual public RefCountable
	{
	public:
		virtual bool
		operator<(const Key& other) const = 0;

		virtual std::string
		str() const = 0;

		virtual
		~Key()
		{}
	};

	typedef SmartPtr<Key> KeyPtr;
	typedef SmartPtr<const Key> ConstKeyPtr;

	class KeyBuilder
	{
	public:
		virtual void
		onFUNCreated() = 0;

		virtual ConstKeyPtr
		operator () (const CompoundPtr& compound, int direction) const = 0;

		virtual
		~KeyBuilder()
		{}
	};

	typedef FUNConfigCreator<KeyBuilder> KeyBuilderCreator;
	typedef wns::StaticFactory<KeyBuilderCreator> KeyBuilderFactory;
} // namespace ldk
} // namespace wns


#endif // NOT defined WNS_LDK_KEY_HPP


