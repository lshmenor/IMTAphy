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

#ifndef WNS_CLONEABLE_HPP
#define WNS_CLONEABLE_HPP

#include <WNS/Assure.hpp>
#include <WNS/TypeInfo.hpp>
#include <WNS/Exception.hpp>
#include <ostream>
#include <memory>

namespace wns {

	/**
	 * @brief Support to (polymorphically) clone objects
	 */
	class CloneableInterface
	{
	public:
		/**
		 * @brief Destructor (implementation empty)
		 */
		virtual
		~CloneableInterface()
		{}

		/**
		 * @brief Called if a copy of the object is requested
		 */
		virtual CloneableInterface*
		clone() const = 0;
	}; // class CloneableInterface

	/**
	 * @brief Default implementation of CloneableInterface
	 *
	 * This implementation simply calls the copy-constructor of the
	 * respective class.
	 */
	template< class T >
	class Cloneable :
		public virtual CloneableInterface
	{
	public:
		/**
		 * @brief Performs typecheck (in debugging mode) and calls the
		 * copy-constructor of the respective class.
		 */
		virtual CloneableInterface*
		clone() const
		{
			assure(
				TypeInfo::create(*this) == TypeInfo::create<T>(),
				"wns::Cloneable: " <<
				"Cloned and orignal type are different! \n" <<
				"Orignal type: " << TypeInfo::create(*this) <<
				"   <->   Clone type: " << TypeInfo::create<T>());

			T* t = new T( *(static_cast<const T*>(this) ) );

			return t;
		}
	}; // class Cloneable


	/**
	 * @brief Last resort: Throw this if your class is not cloneable
	 *
	 * If a class is forced to derive from CloneableInterface but can't be
	 * cloned, you can throw this Exceptio in "Class::clone()", to indicate
	 * the class can't be cloned.
	 *
	 * @warning This is really the last resort and should be avoided under
	 * any circumstances.
	 */
	class CloneNotSupported :
		public Exception
	{
	public:
		CloneNotSupported() :
			Exception("CloneableInterface not supported.")
		{}

		template <typename T>
		CloneNotSupported(T* t) :
			Exception("CloneableInterface not supported for: ")
		{
			(*this) << TypeInfo::create(*t);
		}

        ~CloneNotSupported() throw() {}
	};

	/**
	 * @brief Default implementation of a not cloneable class
	 *
	 * A class can be derived from this class to throw an exception showing
	 * the type of the class which is not cloenable
	 *
	 * @warning This is really the last resort and should be avoided under
	 * any circumstances.
	 */
	class NotCloneable :
		virtual public CloneableInterface
	{
	public:
		virtual CloneableInterface*
		clone() const
		{
			throw CloneNotSupported(this);
		}
	};

	/**
	 * @brief Type safe cloning
	 *
	 * Instead of calling
	 * @include Cloneable_unsafe.example
	 * you can call
	 * @include Cloneable_typesafe.example
	 * which performs automatic type checking and conversion
	 */
	template <typename T>
 	T*
 	clone(T* t)
 	{
 		CloneableInterface* ci = t->clone();
		assureType(ci, T*);
		// can't use static cast here
 		return dynamic_cast<T*>(ci);
 	}

	/**
	 * @brief Type safe cloning of clonable auto_ptr
	 *
	 * Type safe cloning (see clone(T* t)) works also for std::auto_ptr:
	 * @include Cloneable_autoptr.example
	 */
 	template <typename T>
 	std::auto_ptr<T>
 	clone(const std::auto_ptr<T>& t)
 	{
 		CloneableInterface* ci = t->clone();
		assureType(ci, T*);
		// can't use static cast here
 		return std::auto_ptr<T>(dynamic_cast<T*>(ci));
 	}
} // namespace wns

#endif // WNS_CLONEABLE_HPP


