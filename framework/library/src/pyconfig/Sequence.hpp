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

#ifndef WNS_PYCONFIG_SEQUENCE_HPP
#define WNS_PYCONFIG_SEQUENCE_HPP

#include <WNS/pyconfig/Object.hpp>
#include <WNS/pyconfig/Converter.hpp>

#include <WNS/Assure.hpp>
#include <WNS/Exception.hpp>

namespace wns { namespace pyconfig {

	template <typename T, typename ITER>
	class TypedIterator :
		public Converter<T>,
		public ITER
	{
	public:
		typedef ITER IteratorPolicy;
		typedef T ConvertTarget;
		typedef TypedIterator<T, ITER> MyKind;

		TypedIterator() :
				Converter<T>(),
				ITER()
		{
		}

		explicit
		TypedIterator(Object thang) :
				Converter<T>(),
				ITER(thang)
		{
		}

		bool
		operator==(const MyKind& other) const
		{
			return IteratorPolicy::obj() == other.obj();
		} // ==

		bool
		operator!=(const MyKind& other) const
		{
			return IteratorPolicy::obj() != other.obj();
		} // !=

		T operator*()
		{
			assure(!IteratorPolicy::obj().isNull(), "This is the end, my friend.\n");

			T value;
			convert(value, IteratorPolicy::obj());
			return value;
		} // *

		TypedIterator<T, ITER>&
		operator++()
		{
			assure(!IteratorPolicy::obj().isNull(), "This is the end, my friend.\n");

			IteratorPolicy::next();
			return *this;
		} // pre++

		TypedIterator<T, ITER>
		operator++(int)
		{
			assure(!IteratorPolicy::obj().isNull(), "This is the end, my friend.\n");

			MyKind that = *this;
			IteratorPolicy::next();
			return that;
		} // post++

	};


	class Sequence
	{
		friend class View;
	public:
		explicit
		Sequence(Object _sequence);

		~Sequence();

		Sequence(Object _sequence, const std::string& pathName);

		Sequence(const Sequence& other);

		Sequence&
		operator=(const Sequence& other);


		bool
		empty() const;

		int
		size() const;

		template <typename T>
		T
		at(int n) const
		{
			Object obj = sequence.getItem(n);
			assure(!obj.isNull(), "This is the end, my friend.");

			T value;
			Converter<T> converter;
			bool ok = converter.convert(value, obj);
			obj.decref();

			if(!ok)
				throw Exception("couldn't convert.");

			return value;
		} // at

		bool
		isSequenceAt(int n) const;

		Sequence
		getSequenceAt(int n) const;

		//
		// iteration protocol
		//
		class IterPolicy
		{
		protected:
			IterPolicy();
			IterPolicy(Object sequence);

			virtual
			~IterPolicy();

			IterPolicy(const IterPolicy& other);

			IterPolicy&
			operator=(const IterPolicy& other);

			virtual void
			next();

			virtual Object
			obj() const;

		private:
			Object iter;
			Object nextObject;
		};

		template <typename T>
		class iterator :
			public TypedIterator<T, IterPolicy>
		{
		public:
			iterator() :
				TypedIterator<T, IterPolicy>()
			{
			}

			iterator(Object thang) :
				TypedIterator<T, IterPolicy>(thang)
			{
			}
		};

		template <typename T>
		iterator<T>
		begin()
		{
			return iterator<T>(sequence);
		} // begin

		template <typename T>
		iterator<T>
		end()
		{
			return iterator<T>();
		} // end

		static Sequence
		fromString(const std::string& s);

	private:
		Object sequence;
		std::string pathName;
	};

}}

#endif // NOT defined WNS_PYCONFIG_SEQUENCE


