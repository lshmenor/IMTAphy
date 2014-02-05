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

#ifndef WNS_FUNCTOR_HPP
#define WNS_FUNCTOR_HPP

namespace wns {
	/**
	 * @brief The base class of the Functor without arguments
	 */
	template<typename RetVal = void>
	class Functor
	{
	public:
		/**
		 * @brief The operator to be called to execute the functor
		 */
		virtual RetVal
		operator()() const = 0;

		virtual
		~Functor()
		{}
	};

	/**
	 * @brief The base class of the Functor with one argument
	 */
	template<typename Arg, typename RetVal = void>
	class ArgumentFunctor
	{
	public:
		/**
		 * @brief The operator to be called to execute the functor
		 */
		virtual RetVal
		operator()(Arg) const = 0;

		virtual
		~ArgumentFunctor()
		{}
	};

	/**
	 * @brief namespace for TFunctor helper classes
	 */
	namespace tfunctor {

		/**
		 * @brief enum with a value for const and for non const functions
		 */
		enum funcType { constFunc, nonConstFunc };

		/**
		 * @brief Construct a function pointer type for non const functions
		 */
		template<typename T, typename RetVal, funcType F>
		struct ConstructFPtrType
		{
			typedef RetVal (T::*fPtr)();
		};

		/**
		 * @brief Construct a function pointer type for const functions
		 */
		template<typename T, typename RetVal>
		struct ConstructFPtrType<T, RetVal, constFunc>
		{
			typedef RetVal (T::*fPtr)() const;
		};
	}

	/**
	 * @brief The template version of the Functor without arguments
	 *  This will create a functor for member functions in class T, returning
	 *  RetVal and being either non-const (default) or const.
	 *  The last template parameter should always have its default value.
	 *  @ingroup MISC
	 */
	template<
		typename T,
		typename RetVal = void,
		tfunctor::funcType funcType = tfunctor::nonConstFunc,
		typename fPtr = typename tfunctor::ConstructFPtrType<T, RetVal, funcType>::fPtr >
	class TFunctor : public Functor<RetVal>
	{
	public:
		typedef fPtr functionPointer;
		/**
		 * @brief A constructor taking the object to be called with the function pointer
		 */
		TFunctor(T* Obj, fPtr F) : obj(Obj), f(F) {}

		/**
		 * @brief The operator to be called to execute the functor
		 */
		virtual RetVal operator()() const { return (*obj.*f)(); }

	private:
		/**
		 * @brief The object to be called with the function pointer
		 */
		T* obj;
		/**
		 * @brief The function pointer
		 */
		fPtr f;
	};

	/**
	 * @brief namespace for TArgumentFunctor helper classes
	 */
	namespace targumentfunctor {
		using tfunctor::funcType;
		using tfunctor::constFunc;
		using tfunctor::nonConstFunc;

		/**
		 * @brief Construct a function pointer type for non const functions
		 */
		template<typename T, typename Arg, typename RetVal, funcType F>
		struct ConstructFPtrType
		{
			typedef RetVal (T::*fPtr)(Arg);
		};

		/**
		 * @brief Construct a function pointer type for const functions
		 */
		template<typename T, typename Arg, typename RetVal>
		struct ConstructFPtrType<T, Arg, RetVal, constFunc>
		{
			typedef RetVal (T::*fPtr)(Arg) const;
		};
	}

	/**
	 * @brief The template version of the Functor with one argument
	 * This will create a functor for member functions in class T, taking
	 * Arg and returning RetVal and being either non-const (default) or const.
	 * The last template parameter should always have its default value.
	 */
	template<
		typename T,
		typename Arg,
		typename RetVal = void,
		targumentfunctor::funcType funcType = targumentfunctor::nonConstFunc,
		typename fPtr = typename targumentfunctor::ConstructFPtrType<T, Arg, RetVal, funcType>::fPtr >
	class TArgumentFunctor : public ArgumentFunctor<Arg, RetVal>
	{
	public:
		/**
		 * @brief A constructor taking the object to be called with the function pointer
		 */
		TArgumentFunctor(T* Obj, fPtr F) : obj(Obj), f(F) {};

		/**
		 * @brief The operator to be called to execute the functor
		 */
		virtual RetVal operator()(Arg a) const { return (*obj.*f)(a); };

	private:
		/**
		 * @brief The object to be called with the function pointer
		 */
		T* obj;
		/**
		 * @brief The function pointer
		 */
		fPtr f;
	};
}

#endif // WNS_FUNCTOR_HPP


