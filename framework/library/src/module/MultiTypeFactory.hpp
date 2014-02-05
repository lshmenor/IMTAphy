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

#ifndef WNS_MODULE_MULTITYPEFACTORY_HPP
#define WNS_MODULE_MULTITYPEFACTORY_HPP

#include <WNS/Chamaeleon.hpp>

namespace wns { namespace module {
	/**
	 * @brief Factory that can return objects of different types
	 * @ingroup cppgoodies
	 */
	template<typename I>
	class MultiTypeFactory
	{
	public:
		typedef wns::ChamaeleonBase::BadCast BadCast;
		MultiTypeFactory()
		{}

		virtual
		~MultiTypeFactory()
		{}

		template <typename T>
		T
		get(const I& i)
		{
			ChamaeleonBase* v = create(i);
			if(!v) {
				return NULL;
			}

			Chamaeleon<T>* ct = 0;
			T t = 0;
			try {
				ct = v->template downCast<T>();
				t = ct->unHide();
				delete ct;
			} catch(...) {
				if(ct)
					delete ct;
				    // FIXME(msg): we can free the chamaeleon, but not the
				    // obj hidden within the chamaeleon.
				delete v;

				throw;
			}

			return t;
		}
	protected:
		virtual ChamaeleonBase*
		create(const I& i) =0;
	};
}
}
#endif // NOT defined WNS_MODULE_MULTITYPEFACTORY_HPP
