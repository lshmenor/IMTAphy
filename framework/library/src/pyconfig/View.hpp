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

#ifndef WNS_PYCONFIG_VIEW_HPP
#define WNS_PYCONFIG_VIEW_HPP

#include <WNS/pyconfig/Sequence.hpp>
#include <WNS/pyconfig/Object.hpp>
#include <WNS/TypeInfo.hpp>

#include <sstream>
#include <string>

namespace wns { namespace pyconfig {

	namespace tests {
		class ViewTest;
	}

	/**
	 * @brief Node in a configuration tree
	 * @ingroup group_main_classes
	 */
	class View
	{
		friend class tests::ViewTest;

	public:
		View(const View& other, const std::string& newViewExpression);
		View(const View& other, const std::string& newViewExpression, int at);
		View(const Sequence& other, int at);

		virtual
		~View();

		/**
		 * @brief Creates a new pyconfig::View as sub view.
		 *
		 * @deprecated get<pyconfig::View>("viewName") should be used instead.
		 */
		pyconfig::View
		getView(const std::string& newViewExpression) const;

		/**
		 * @brief Create a sequence from a python expression
		 *
		 * @deprecated get<pyconfig::Sequence>("sequenceName") should be used instead.
		 *
		 */
		pyconfig::Sequence
		getSequence(const std::string& sequenceExpression) const;

		/**
		 * @brief Creates a new View as sub view for a sequence/dict item.
		 *
		 * @deprecated get<pyconfig::Sequence>(...).at(n) should be used instead.
		 */
		View
		getView(const std::string& newViewExpression, int at) const;

		/**
		 * @brief Create stl container from a python sequence.
		 *
		 * @deprecated use the pyconfig::Sequence protocol instead.
		 */
		template <typename S>
		S
		getSequence(const std::string& optionExpression) const
		{
			S container;
			for(int i = 0; i < len(optionExpression); ++i) {
				container.push_back(get<typename S::value_type>(optionExpression, i));
			}
			return container;
		} // get


		template <typename T>
		bool
		get(T& value, const std::string& optionExpression) const
		{
			Object o = getObject(optionExpression);
			if(o.isNull())
			{
				return false;
			}

			convert(value, o, optionExpression);

			o.decref();
			return true;
		} // get

		template <typename T>
		bool
		get(T& value, const std::string& optionExpression, int at) const
		{
			Object o = getObject(optionExpression, at);
			if(o.isNull())
			{
				return false;
			}

			convert(value, o, optionExpression);

			o.decref();
			return true;
		} // get[at]


		bool
		get(View& value, const std::string& optionExpression) const
		{
			Object o = getObject(optionExpression);
			if(o.isNull())
			{
				return false;
			}

			checkIsNotNone<View>(o, optionExpression);

			o.decref();

			value = View(*this, optionExpression);
			return true;
		} // get


		bool
		get(View& value, const std::string& optionExpression, int at) const
		{
			Object o = getObject(optionExpression, at);
			if(o.isNull())
			{
				return false;
			}

			o.decref();

			std::stringstream element;
			element << optionExpression << "[" << at << "]";
			checkIsNotNone<View>(o, element.str());

			value = View(*this, optionExpression, at);
			return true;
		} // get


		template <typename T>
		T
		get(const std::string& optionExpression) const
		{
			T t;
			if(!get(t, optionExpression)) {
				couldntRetrieve(optionExpression);
				showdown("Unknown thang in pyconfig::View::get.");
			}

			return t;
		} // get


		template <typename T>
		T
		get(const std::string& optionExpression, int at) const
		{
			T t;
			if(!get(t, optionExpression, at)) {
				couldntRetrieve(optionExpression);
				showdown("Unknown thang in pyconfig::View::get.");
			}

			return t;
		} // get[at]

		pyconfig::View
		get(const std::string& optionExpression) const
		{
			return get<View>(optionExpression);
		} // get


		pyconfig::View
		get(const std::string& optionExpression, int at) const
		{
			return get<View>(optionExpression, at);
		} // get[at]

		bool
		len(int& result, const std::string& optionExpression) const;

		int
		len(const std::string& optionExpression) const;

		/**
		 * @brief Returns an ID identifying a View
		 */
		long long int
		getId() const;

		bool
		knows(const std::string& optionExpression) const;

		bool
		isNone(const std::string& expression) const;

		bool
		isSequence(const std::string& expression) const;

		std::string
		context() const;

		void
		patch(const std::string& expression);

		// reference counting copy constructor / assignment operator
		View(const View& other);
		View& operator=(const View& other);

		// Operators
		bool
		operator==(const View& other) const;

		bool
		operator!=(const View& other) const;

		bool
		operator<(const View& other) const;

		friend std::ostream&
		operator<<(std::ostream& str, const View& other)
		{
			str << other.context() << ":\n"
			    << other.asString();
			return str;
		}

		Object
		getObject(const std::string& optionExpression) const;

		Object
		getObject(const std::string& optionExpression, int at) const;

        protected:

                void
                init(View& other, const std::string& viewExpression);

		template <typename T>
		void
		checkIsNotNone(Object o, const std::string& optionExpression) const
		{
			if(o.isNone()) {
				Exception e;
				e << "Tried to convert "
				  << this->context() << "::"
				  << optionExpression
				  << " to " << TypeInfo::create<T>()
				  << ", but value is 'None'\n";
				throw e;
			}
		}

		template <typename T>
		bool
		convert(T& value, Object o, const std::string& optionExpression) const
		{
			checkIsNotNone<T>(o, optionExpression);
			return this->doConvert(value, o);
		}

		template <typename T>
		bool
		doConvert(T &value, Object o) const
		{
			if (!o.isConvertibleToString())
			{
				return false;
			}

			std::istringstream os(o.toString());
			os >> value;

			return true;
		} // convert

		// boolean converter
		bool
		doConvert(bool& value, Object o) const;

		// std::string converter
		bool
		doConvert(std::string& value, Object o) const;

		/**
		 * @brief Display an error message saying that optionExpression could not be resolved.
		 *
		 */
		void
		couldntRetrieve(const std::string& optionExpression) const;

		/**
		 * @brief Display the current python error and raise an exception.
		 *
		 * An exception is throwns iff raise is true. Reason is the string that is used
		 * as parameter to construct the Exception object.
		 */
		void
		showdown(const std::string& reason, bool raise = true) const;


		/**
		 * @brief Convert to a string
		 */
		std::string
		asString() const;

		Object dict;

		std::string viewExpression;

		View();

	private:
		/**
		 * @brief automatic startup of python interpreter
		 */
		void
		initializePython();

		/**
		 * @brief automatic shutdown of python interpreter
		 */
		void
		finalizePython();

		/**
		 * @brief internal reference counting to support automatic
		 * startup and shutdown of python interpreter
		 */
		int&
		getCount();
	};
}}


#endif // NOT defined WNS_PYCONFIG_VIEW_HPP



