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

#ifndef WNS_SMARTPTR_HPP
#define WNS_SMARTPTR_HPP

#ifdef WNS_SMARTPTR_DEBUGGING
#include <WNS/SmartPtrBase.hpp>
#endif

#include <WNS/RefCountable.hpp>
#include <WNS/TypeInfo.hpp>

#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <list>

namespace wns {
	/**
	 * @brief Intrusive reference counting
	 * @author Marc Schinnenburg <marc@schinnenburg.com>
	 * @ingroup group_main_classes
	 *
	 * SmartPtr is a template that supports automatic memory
	 * management. Once a SmartPtr is allocated you don't need to care about
	 * deallocation.
	 */
	template <typename T>
	class SmartPtr
#ifdef WNS_SMARTPTR_DEBUGGING
		: public SmartPtrBase
#endif
	{
		// the underscores are for readability in error messages
		class YOU__SHOULD__NOT_COMPARE__THIS__WITH__ANYTHING__BUT__NULL
		{
			void operator delete(void*);
		};

	public:
		/**
		 * @brief Default Construtor is equal to NULL pointer
		 */
		SmartPtr() :
#ifdef WNS_SMARTPTR_DEBUGGING
			SmartPtrBase(),
			id(++getCounter()),
			file(),
			line(0),
#endif
			ptr(NULL)
		{
		}

		/**
		 * @brief Pointer p will be stored in / handled by SmartPtr
		 */
		explicit
		SmartPtr(T* p) :
#ifdef WNS_SMARTPTR_DEBUGGING
			SmartPtrBase(),
			id(++getCounter()),
			file(""),
			line(0),
#endif
			ptr(p)
		{
			assert(ptr);
			ptr->incRefCount();
#ifdef WNS_SMARTPTR_DEBUGGING
			this->created(file, 0);
#endif
		}


		/**
		 * @brief Copy-constructor from same SmartPtr type
		 */
		SmartPtr(const SmartPtr& s) :
#ifdef WNS_SMARTPTR_DEBUGGING
			SmartPtrBase(),
			id(++getCounter()),
			file(""),
			line(0),
#endif
			ptr(s.ptr)
		{
			if(ptr) {
				ptr->incRefCount();
			}
#ifdef WNS_SMARTPTR_DEBUGGING
			this->created("", 0);
#endif
		}


		/**
		 * @brief Copy-constructor from other SmartPtr type
		 */
		template <typename OtherType>
		SmartPtr(const SmartPtr<OtherType>& s) :
#ifdef WNS_SMARTPTR_DEBUGGING
			SmartPtrBase(),
			id(++getCounter()),
			file(""),
			line(0),
#endif
			ptr(s.getPtr())
		{
			if(ptr) {
				ptr->incRefCount();
			}
#ifdef WNS_SMARTPTR_DEBUGGING
			this->created("", 0);
#endif
		}

#ifdef WNS_SMARTPTR_DEBUGGING
		/**
		 * @brief Construct NULL pointer (additional debugging support)
		 */
 		SmartPtr(const std::string& file, int line) :
			SmartPtrBase(),
			id(++getCounter()),
			file(""),
			line(0),
			ptr(NULL)
		{
			this->created(file, line);
		}

		/**
		 * @brief Pointer p will be stored in / handled by SmartPtr
		 * (additional debugging support)
		 */
		explicit
		SmartPtr(const std::string& file, int line, T* p) :
			SmartPtrBase(),
			id(++getCounter()),
			file(""),
			line(0),
			ptr(p)
		{
			assert(ptr);
			ptr->incRefCount();
			this->created(file, line);
		}

		/**
		 * @brief "Copy-constructor" from same SmartPtr type (additional
		 * debugging support)
		 */
		SmartPtr(const std::string& file, int line, const SmartPtr& s) :
			SmartPtrBase(),
			id(++getCounter()),
			file(""),
			line(0),
			ptr(s.ptr)
		{
			if(ptr) {
				ptr->incRefCount();
			}
			this->created(file, line);
		}

		/**
		 * @brief Copy-constructor from other SmartPtr type (additional
		 * debugging support)
		 */
		template <typename OtherType>
		SmartPtr(const std::string& file,  int line, const SmartPtr<OtherType>& s) :
			SmartPtrBase(),
			id(++getCounter()),
			file(""),
			line(0),
			ptr(s.getPtr())
		{
			if(ptr) {
				ptr->incRefCount();
			}
			this->created(file, line);
		}
#endif // defined WNS_SMARTPTR_DEBUGGING

		/**
		 * @brief Assign operator for same SmartPtr type
		 */
		SmartPtr&
		operator= (const SmartPtr& s)
		{
			if(ptr) {
				ptr->decRefCount();
			}
			ptr=s.ptr;
			if(ptr) {
				ptr->incRefCount();
			}
			return *this;
		}

		/**
		 * @brief Assign operator for other SmartPtr type
		 */
		template<typename OtherType>
		SmartPtr&
		operator= (const SmartPtr<OtherType>& s)
		{
			if(ptr) {
				ptr->decRefCount();
			}
			ptr=s.getPtr();
			if(ptr) {
				ptr->incRefCount();
			}
			return *this;
		}

		/**
		 * @brief Automagically deletes pointer if reference count has
		 * fallen to 0.
		 */
		~SmartPtr()
		{
			if(ptr) {
				ptr->decRefCount();
			}
#ifdef WNS_SMARTPTR_DEBUGGING
			this->deleted();
#endif
		}

		/**
		 * @brief Dereference operator
		 */
		T&
		operator* () const
		{
			assert(ptr != NULL);
			return *ptr;
		}

		/**
		 * @brief Arrow operator
		 */
		T*
		operator-> () const
		{
			assert(ptr != NULL);
			return ptr;
		}

		/**
		 * @brief Enables: if(smartPtr), if(!smartPtr) and if(smartPtr == NULL)
		 *
		 * @return NULL if pointer==NULL
		 */
		operator YOU__SHOULD__NOT_COMPARE__THIS__WITH__ANYTHING__BUT__NULL* () const
		{
			if (ptr == NULL)
			{
				return NULL;
			}
			static YOU__SHOULD__NOT_COMPARE__THIS__WITH__ANYTHING__BUT__NULL foo;
			return &foo;
		}

		/**
		 * @brief Equality
		 */
		template<typename OtherType>
		bool
		operator==(const SmartPtr<OtherType>& s) const
		{
			return ptr==s.getPtr();
		}

		/**
		 * @brief Inequality
		 */
		template<typename OtherType>
		bool
		operator!=(const SmartPtr<OtherType>& s) const
		{
			return ptr!=s.getPtr();
		}

		/**
		 * @brief Less than
		 */
		template<typename OtherType>
		bool
		operator<(const SmartPtr<OtherType>& s) const
		{
			return ptr<s.getPtr();
		}

		/**
		 * @brief Greater than
		 */
		template<typename OtherType>
		bool
		operator>(const SmartPtr<OtherType>& s) const
		{
			return ptr>s.getPtr();
		}

		/**
		 * @brief Less or equal than
		 */
		template<typename OtherType>
		bool
		operator<=(const SmartPtr<OtherType>& s) const
		{
			return ptr<=s.getPtr();
		}

		/**
		 * @brief Greater or equal than
		 */
		template<typename OtherType>
		bool
		operator>=(const SmartPtr<OtherType>& s) const
		{
			return ptr>=s.getPtr();
		}

		/**
		 * @brief Access the raw pointer (use with great care!)
		 */
		T*
		getPtr() const
		{
			return ptr;
		}

		/**
		 * @brief Current reference count
		 *
		 * @returns Number of SmartPtrs pointing to a raw pointer
		 */
		long int
		getRefCount()
		{
			assert(ptr);
			return ptr->getRefCount();
		}

		/**
		 * @brief Debugging support
		 */
		static void
		printAllExistingPointers()
		{
#ifdef WNS_SMARTPTR_DEBUGGING
			std::cout << "Currently existing SmartPtr<"
				  << wns::TypeInfo::create<T>()
				  <<  ">: " << "\n";

			for(typename std::list<SmartPtr*>::const_iterator itr = getAllPointers().begin();
			    itr != getAllPointers().end();
			    ++itr) {
				std::cout << "Pointer id: " << (*itr)->id << "\n"
					  << "Created at: " << (*itr)->file << ":" << (*itr)->line << "\n"
                                          << "Backtrace: \n" << (*itr)->getBacktrace() << "\n";

			}
#endif
		}

		/**
		 * @brief Debugging support
		 */
		static void
		printNumberOfExistingPointers()
		{
#ifdef WNS_SMARTPTR_DEBUGGING
			std::cout << "Number of currently existing SmartPtr<"
				  << wns::TypeInfo::create<T>()
				  <<  ">: " << getAllPointers().size() << "\n";
#endif
		}


	private:
#ifdef WNS_SMARTPTR_DEBUGGING
		long long int id;

		std::string file;
		int line;

		void
		created(const std::string& _file, int _line)
		{
			this->getAllPointers().push_back(this);
			this->file = _file;
			this->line = _line;
		}

		void
		deleted()
		{
			this->getAllPointers().remove(this);
		}

		static long long int&
		getCounter()
		{
			static long long int counter = 0;
			return counter;
		}

		static std::list<SmartPtr*>&
		getAllPointers()
		{
			static std::list<SmartPtr*> allPointers;
			return allPointers;
		}
		wns::TypeInfo
		getTypeInfo() const
		{
			return wns::TypeInfo::create<T>();
		}

		long long int
		getId() const
	        {
			return id;
		}
#endif // defined WNS_SMARTPTR_DEBUGGING

		/**
		 * @brief The raw pointer
		 */
		T* ptr;
	};

	template <typename T, typename P>
	T*
	dynamicCast(P* p)
	{
		return dynamic_cast<T*>(p);
	}

	template <typename T, typename P>
	SmartPtr<T>
	dynamicCast(const SmartPtr<P>& p)
	{
		return SmartPtr<T>(dynamic_cast<T*>(p.getPtr()));
	}

	template <typename T, typename P>
	T*
	staticCast(P* p)
	{
		return static_cast<T*>(p);
	}

	template <typename T, typename P>
	SmartPtr<T>
	staticCast(const SmartPtr<P>& p)
	{
		return SmartPtr<T>(static_cast<T*>(p.getPtr()));
	}

	/**
	 * @name Member function adapter for smart pointer held classes
	 */
	//@{
	template <class RETURNTYPE, class POINTERTYPE>
	class smart_ptr_mem_fun_t :
		public std::unary_function<SmartPtr<POINTERTYPE>, RETURNTYPE>
	{
	public:
		explicit
		smart_ptr_mem_fun_t(void (POINTERTYPE::*__pf)()) :
			_M_f(__pf)
		{}

		RETURNTYPE
		operator()(SmartPtr<POINTERTYPE>& __p) const
		{
			(*__p.*_M_f)();
		}

	private:
		RETURNTYPE (POINTERTYPE::*_M_f)();
	};

	template <class RETURNTYPE, class POINTERTYPE>
	class const_smart_ptr_mem_fun_t :
		public std::unary_function<SmartPtr<POINTERTYPE>, RETURNTYPE>
	{
	public:
		explicit
		const_smart_ptr_mem_fun_t(void (POINTERTYPE::*__pf)() const) :
			_M_f(__pf)
		{}

		RETURNTYPE
		operator()(SmartPtr<POINTERTYPE>& __p) const
		{
			(*__p.*_M_f)();
		}

	private:
		RETURNTYPE (POINTERTYPE::*_M_f)() const;
	};
	//@}

	/**
	 * @name Helper function for member funtion adapters
	 */
	//@{
	/**
	 * @brief This helper function may be used like the mem_fun and
	 * mem_fun_ref functions from the STL.
	 */
	template <class RETURNTYPE, class POINTERTYPE>
	inline smart_ptr_mem_fun_t<RETURNTYPE, POINTERTYPE>
	smart_ptr_mem_fun(RETURNTYPE (POINTERTYPE::*__f)())
	{
		return smart_ptr_mem_fun_t<RETURNTYPE, POINTERTYPE>(__f);
	}

	template <class RETURNTYPE, class POINTERTYPE>
	inline const_smart_ptr_mem_fun_t<RETURNTYPE, POINTERTYPE>
	smart_ptr_mem_fun(RETURNTYPE (POINTERTYPE::*__f)() const )
	{
		return const_smart_ptr_mem_fun_t<RETURNTYPE, POINTERTYPE>(__f);
	}
	//@}

} // wns

#endif // NOT defined WNS_SMARTPTR_HPP


