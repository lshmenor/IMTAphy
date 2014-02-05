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

#ifndef WNS_REFCOUNTABLE_HPP
#define WNS_REFCOUNTABLE_HPP

#include <WNS/Assure.hpp>
#include <stdint.h>

namespace wns {
	/**
	 * @brief Part of reference counting with SmartPtr
	 *
	 * This class helps to support intrusive reference counting. If you want
	 * to use SmartPtr you should derive your class to be reference counted
	 * from this helper class.
	 *
	 * @note You have to derive virtual and must not call the default
	 * constructor!!!
	 *
	 * Example:
	 * @include wns.RefCountableDerive.example
	 */
	class RefCountable
	{
		/**
		 * @brief All SmartPtr are friend of the RefCountable
		 */
		template<class> friend class SmartPtr;
	public:
		/**
		 * @brief Return the current number of SmartPtr pointing to this
		 * object
		 */
		long int
		getRefCount() const
		{
			return count;
		}

	protected:
		/**
		 * @brief Must only be called by derived class
		 *
		 * @note It makes no sense to construct an create an instance of
		 * RefCountable
		 */
		RefCountable() :
			count(0)
		{}

		/**
		 * @brief Must only be called by derived class
		 *
		 * @internal If something that is derived from this class needs to be
		 * copied the refcount must be set to 0!!
		 */
		RefCountable(const RefCountable&) :
			count(0)
		{}

		/**
		 * @brief Destructor
		 */
		virtual
		~RefCountable()
		{}

	private:
		/**
		 * @brief Increase the reference count
		 *
		 * This will be called by SmartPtr on creation of a SmartPtr instance
		 */
		void
		incRefCount() const
		{
			++count;
		}

		/**
		 * @brief Decrease the reference count
		 *
		 * This will be called by SmartPtr on deletetion of a SmartPtr instance
		 */
		void
		decRefCount() const
		{
			assure(count>0,
			       "Can't deref object with reference count<=0. "
			       "This is normally an indication that "
			       "someone did mess around with the SmartPtr.");
			if(--count <= 0) {
				delete this;
			}
		}

		/**
		 * @brief Stores the reference count
		 */
		mutable long int count;
	};
} // wns

#endif


