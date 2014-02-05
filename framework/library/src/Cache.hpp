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


#ifndef _CACHE_HPP
#define _CACHE_HPP

namespace wns {
	/**
	 * @brief A template that supports caching
	 * @ingroup cppgoodies
	 */
	template <class C>
	class Cache
		: public C
	{
	public:
		/**
		 * @brief The default constructor of the cache will call the
		 * default constructor of the base class as well and set the
		 * state to "clean"
		 */
		Cache()
			: C(),
			  dirty(false)
		{}

		/**
		 * @brief The copy constructor of the cache will call the copy
		 * constructor of the base class as well and set the state to
		 * "clean"
		 */
		Cache(const C& aC)
			: C(aC),
			  dirty(false)
		{}

		virtual ~Cache()
		{}

		void
		operator =(const Cache& aC)
		{
			C::operator=(aC);
			dirty=false;
		}

		void
		operator =(const C& aC)
		{
			C::operator=(aC);
			dirty=false;
		}

		template <class O>
		void
		operator +=(const O& aC)
		{
			C::operator+=(aC);
			dirty=false;
		}

		template <class O>
		void
		operator -=(const O& aC)
		{
			C::operator-=(aC);
			dirty=false;
		}

		template <class O>
		void
		operator *=(const O& aC)
		{
			C::operator*=(aC);
			dirty=false;
		}

		template <class O>
		void
		operator /=(const O& aC)
		{
			C::operator/=(aC);
			dirty=false;
		}

		template <class O>
		void
		operator %=(const O& aC)
		{
			C::operator%=(aC);
			dirty=false;
		}

		template <class O>
		void
		operator ^=(const O& aC)
		{
			C::operator^=(aC);
			dirty=false;
		}

		template <class O>
		void
		operator &=(const O& aC)
		{
			C::operator&=(aC);
			dirty=false;
		}

		template <class O>
		void
		operator |=(const O& aC)
		{
			C::operator|=(aC);
			dirty=false;
		}

		template <class O>
		void
		operator <<=(const O& aC)
		{
			C::operator<<=(aC);
			dirty=false;
		}

		template <class O>
		void
		operator >>=(const O& aC)
		{
			C::operator>>=(aC);
			dirty=false;
		}

		/**
		 * @brief Tell the cache that it is dirty and needs an update
		 *
		 * Some events might cause your cache to become dirty. If such
		 * an event occurs you have to call setDirty() on your cache in
		 * order to notify it of this change.
		 */
		void setDirty() {dirty=true;};

		/**
		 * @brief Tell the cache that it is clean
		 *
		 * Normally, if you use one the opertors of: =, +=, -=, *=, /=,
		 * %=, ^=, &=, |=, <<= or >>= the cache is automatically set
		 * clean. However if you want to tell the cache that it is clean
		 * (needs no update) you can use this function to change the
		 * cache state.
		 */
		void setClean() {dirty=false;};

		/**
		 * @brief Ask the cache if it's dirty
		 *
		 * If the cache is dirty you might want to do something to make it
		 * clean that is update it. Example:
		 *  @verbatim
		 *  if(theCache.isDirty()) {
		 *      theCache = calcSomething();
		 *  }
		 *  return theCache;
		 *  @endverbatim
		 */
		bool isDirty() {return dirty;};
	private:
		bool dirty;
	};

	/**
	 * @brief Helper class, that turns a POD into a class
	 * @ingroup cppgoodies
	 */
	template <class POD>
	class Class
	{
	public:
		Class()
			: data()
		{}

		Class(const POD& pod)
			: data(pod)
		{}

		virtual ~Class()
		{}

		void
		operator =(const Class& aC)
		{
			data = aC.data;
		}

		void
		operator =(const POD& pod)
		{
			data=pod;
		}

		template <class O> void operator +=(const O& aC) {data+=aC;}
		template <class O> void operator -=(const O& aC) {data+=aC;}
		template <class O> void operator *=(const O& aC) {data+=aC;}
		template <class O> void operator /=(const O& aC) {data+=aC;}
		template <class O> void operator %=(const O& aC) {data+=aC;}
		template <class O> void operator ^=(const O& aC) {data+=aC;}
		template <class O> void operator &=(const O& aC) {data+=aC;}
		template <class O> void operator |=(const O& aC) {data+=aC;}
		template <class O> void operator <<=(const O& aC) {data+=aC;}
		template <class O> void operator >>=(const O& aC) {data+=aC;}

		template <class O> void operator +=(const Class<O>& aC) {data+=O(aC);}
		template <class O> void operator -=(const Class<O>& aC) {data+=O(aC);}
		template <class O> void operator *=(const Class<O>& aC) {data+=O(aC);}
		template <class O> void operator /=(const Class<O>& aC) {data+=O(aC);}
		template <class O> void operator %=(const Class<O>& aC) {data+=O(aC);}
		template <class O> void operator ^=(const Class<O>& aC) {data+=O(aC);}
		template <class O> void operator &=(const Class<O>& aC) {data+=O(aC);}
		template <class O> void operator |=(const Class<O>& aC) {data+=O(aC);}
		template <class O> void operator <<=(const Class<O>& aC) {data+=O(aC);}
		template <class O> void operator >>=(const Class<O>& aC) {data+=O(aC);}

		virtual
		operator const POD &() const
		{
			return data;
		}

	private:
		POD data;
	};
}

#endif

/**
 * @file
 */
