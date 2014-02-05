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


#ifndef WNS_DOUBLEDISPATCHER_HPP
#define WNS_DOUBLEDISPATCHER_HPP
#include <WNS/TypeInfo.hpp>
#include <WNS/TypeTraits.hpp>
#include <WNS/SmartPtr.hpp>
#include <string>
#include <map>
#include <iostream>

namespace wns
{
	/**
	 * @brief Base class for the DoubleDispatcher
	 */
	template<typename PlainBaseRhs,
		 typename CallbackType>
	class DoubleDispatcherBase
	{
		friend class DoubleDispatcherTest;
	public:
		typedef std::map<TypeInfo, CallbackType> CallbackMap;
		typedef typename CallbackMap::iterator CMI;

		DoubleDispatcherBase()
			: cm(CallbackMap()),
			  defaultCallback(0)
		{
		}

		virtual ~DoubleDispatcherBase()
		{}

	protected:
		CallbackType
		getCallback(const TypeInfo& rhs)
		{
			CMI res = cm.find(rhs);
			if(res!=cm.end()) {
				return res->second;
			} else {
				assert(defaultCallback);
				return defaultCallback;
			}
		}

		void
		addCallback(const TypeInfo& rhs, const CallbackType& cb)
		{
			if(rhs == TypeInfo::create<PlainBaseRhs>()) {
				defaultCallback = cb;
			} else {
				assert(!cm.count(rhs));
				cm[rhs] = cb;
			};
		}

		void
		addDefaultCallback(const CallbackType& cb) {
			defaultCallback = cb;
		}

	private:
		CallbackMap cm;
		CallbackType defaultCallback;
	};

	/**
	 * @brief Multimethods for C++
	 *
	 * This is the interface of DoubleDispatcher
	 *
	 * @note This class has intentionally been left blank
	 */
	template<class ConcreteLhs,
		 class BaseRhs,
		 typename ResultType = void,
		 int Argument = TypeTraits<BaseRhs>::propNumber>
	class DoubleDispatcher
	{
	};

	template<class ConcreteLhs,
		 class BaseRhs,
		 typename ResultType>
	class DoubleDispatcher<ConcreteLhs,
			       BaseRhs,
			       ResultType,
			       TypeTraits<NullType>::Reference>
				       : public DoubleDispatcherBase<typename TypeTraits<BaseRhs>::ReferenceType,
								     ResultType (*)(ConcreteLhs&, BaseRhs)>
	{
	public:
		DoubleDispatcher()
			: DoubleDispatcherBase<typename TypeTraits<BaseRhs>::ReferenceType, ResultType (*)(ConcreteLhs&, BaseRhs)>()
			{
			}
		virtual ~DoubleDispatcher()
			{}

		template <typename ConcreteRhs,
			ResultType (ConcreteLhs::*callback)(ConcreteRhs&)>
			void
			addMemberFunction()
			{
				struct Local
				{
					static ResultType Trampoline(ConcreteLhs& lhs, BaseRhs rhs)
					{
						return ((lhs).*callback)(static_cast<ConcreteRhs&>(rhs));
					}
				};
				addCallback(TypeInfo::create<ConcreteRhs>(), &Local::Trampoline);
			}

 		ResultType dispatch(ConcreteLhs& lhs, BaseRhs rhs)
 			{
 				return (*getCallback(TypeInfo::create(rhs)))(lhs, rhs);
 			}

 		ResultType dispatch(ConcreteLhs* lhs, BaseRhs rhs)
 			{
 				return (*getCallback(TypeInfo::create(rhs)))(*lhs, rhs);
 			}
	};

	template<class ConcreteLhs,
		 class BaseRhs,
		 typename ResultType>
	class DoubleDispatcher<ConcreteLhs,
			       BaseRhs,
			       ResultType,
			       TypeTraits<NullType>::Pointer>
				       : public DoubleDispatcherBase<typename TypeTraits<BaseRhs>::PointeeType,
								     ResultType (*)(ConcreteLhs&, BaseRhs)>
	{
	public:
		DoubleDispatcher()
			: DoubleDispatcherBase<typename TypeTraits<BaseRhs>::PointeeType, ResultType (*)(ConcreteLhs&, BaseRhs)>()
			{
			}
		virtual ~DoubleDispatcher()
			{}

		template <typename ConcreteRhs,
			ResultType (ConcreteLhs::*callback)(ConcreteRhs*)>
			void
			addMemberFunction()
			{
				struct Local
				{
					static ResultType Trampoline(ConcreteLhs& lhs, BaseRhs rhs)
					{
						return ((lhs).*callback)(staticCast<ConcreteRhs>(rhs));
					}
				};
				addCallback(TypeInfo::create<ConcreteRhs>(), &Local::Trampoline);
			}

 		ResultType dispatch(ConcreteLhs& lhs, BaseRhs rhs)
 			{
 				return (*getCallback(TypeInfo::create(*rhs)))(lhs, rhs);
 			}

 		ResultType dispatch(ConcreteLhs* lhs, BaseRhs rhs)
 			{
 				return (*getCallback(TypeInfo::create(*rhs)))(*lhs, rhs);
 			}
	};

	template<class ConcreteLhs,
		 class BaseRhs,
		 typename ResultType>
	class DoubleDispatcher<ConcreteLhs,
			       BaseRhs,
			       ResultType,
			       TypeTraits<NullType>::ConstReference>
				       : public DoubleDispatcherBase<typename TypeTraits<BaseRhs>::ReferenceType,
								     ResultType (*)(ConcreteLhs&, BaseRhs)>
	{
	public:
		DoubleDispatcher()
			: DoubleDispatcherBase<typename TypeTraits<BaseRhs>::ReferenceType, ResultType (*)(ConcreteLhs&, BaseRhs)>()
			{
			}
		virtual ~DoubleDispatcher()
			{}

		template <typename ConcreteRhs,
			ResultType (ConcreteLhs::*callback)(const ConcreteRhs&)>
			void
			addMemberFunction()
			{
				struct Local
				{
					static ResultType Trampoline(ConcreteLhs& lhs, BaseRhs rhs)
					{
						return ((lhs).*callback)(static_cast<const ConcreteRhs&>(rhs));
					}
				};
				addCallback(TypeInfo::create<ConcreteRhs>(), &Local::Trampoline);
			}

 		ResultType dispatch(ConcreteLhs& lhs, BaseRhs rhs)
 			{
 				return (*getCallback(TypeInfo::create(rhs)))(lhs, rhs);
 			}

 		ResultType dispatch(ConcreteLhs* lhs, BaseRhs rhs)
 			{
 				return (*getCallback(TypeInfo::create(rhs)))(*lhs, rhs);
 			}
	};


	template<class ConcreteLhs,
			 class BaseRhs,
			 typename ResultType>
	class DoubleDispatcher<ConcreteLhs,
			       const SmartPtr<BaseRhs>&,
			       ResultType,
			       TypeTraits<NullType>::ConstReference>
				       : public DoubleDispatcherBase<BaseRhs,
								     ResultType (*)(ConcreteLhs&, const SmartPtr<BaseRhs>&)>
	{
	public:
		DoubleDispatcher()
			: DoubleDispatcherBase<BaseRhs, ResultType (*)(ConcreteLhs&, const SmartPtr<BaseRhs>&)>()
			{
			}
		virtual ~DoubleDispatcher()
			{}

		template <typename ConcreteRhs,
			ResultType (ConcreteLhs::*callback)(const SmartPtr<ConcreteRhs>&)>
			void
			addMemberFunction()
			{
				struct Local
				{
					static ResultType Trampoline(ConcreteLhs& lhs, const SmartPtr<BaseRhs>& rhs)
						{
							return ((lhs).*callback)(staticCast<ConcreteRhs>(rhs));
						}
				};
				addCallback(TypeInfo::create<ConcreteRhs>(), &Local::Trampoline);
			}

 		ResultType dispatch(ConcreteLhs& lhs, const SmartPtr<BaseRhs>& rhs)
 			{
 				return (*getCallback(TypeInfo::create(*rhs)))(lhs, rhs);
 			}

 		ResultType dispatch(ConcreteLhs* lhs, const SmartPtr<BaseRhs>& rhs)
 			{
 				return (*getCallback(TypeInfo::create(*rhs)))(*lhs, rhs);
 			}
	};
}

#endif // NOT defined WNS_DOUBLEDISPATCHER_HPP


