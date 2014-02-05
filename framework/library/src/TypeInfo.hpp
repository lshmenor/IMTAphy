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

#ifndef WNS_TYPEINFO_HPP
#define WNS_TYPEINFO_HPP

#include <WNS/IOutputStreamable.hpp>
#include <WNS/demangle.hpp>

#include <typeinfo>
#include <string>

namespace wns {
	/**
	 * @brief Serves as null marker for types
	 */
	class NullType
	{};


	/**
	 * @brief A wrapper around type_info
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @ingroup group_main_classes
     *
     * This class provides "object-oriented" access to C++ RTTI.
	 */
	class TypeInfo :
		virtual public wns::IOutputStreamable
	{
	public:
        /**
         * @brief Constructor, but consider to use the more convinient
         * TypeInfo::create() methods
         */
		explicit
		TypeInfo(const std::type_info& type) :
            IOutputStreamable(),
			type_(&type)
		{}

        /**
         * @brief Copy c'tor (due to dynamic members)
         */
        TypeInfo(const TypeInfo& other) :
            IOutputStreamable(other),
            type_(other.type_)
        {
        }

        /**
         * @brief Assignment operator (due to dynamic members)
         */
        TypeInfo&
        operator=(const TypeInfo& other)
        {
            type_ = other.type_;
            return *this;
        }


        // Default d'tor is ok

        /**
         * @name type_info interface
         */
        //@{
        /**
         * @brief Forward the before() method of type_info
         */
		bool
		before(const TypeInfo& other) const
		{
			return type_->before(*(other.type_));
		}

        /**
         * @brief returns the mangled C++ name of the type
         *
         * @note If you want a human readable string you should use
         * TypeInfo::toString(), which returns the demangled name.
         */
		const char*
		name() const
		{
			return type_->name();
		}
        //@}

        /**
         * @name Operators
         */
        //@{
		bool
		operator==(const TypeInfo& other) const
		{
			return (*type_ == *(other.type_));
		}

		bool
		operator!=(const TypeInfo& other) const
		{
			return (*type_ != *(other.type_));
		}

		bool
		operator<(const TypeInfo& other) const
		{
			return before(other);
		}

		bool
		operator<=(const TypeInfo& other) const
		{
			return ((*this < other) || (*this == other));
		}

		bool
		operator>(const TypeInfo& other) const
		{
			return !((*this < other) || (*this == other));
		}

		bool
		operator>=(const TypeInfo& other) const
		{
			return !(*this < other);
		}
        //@}

        /**
         * @name Creation of TypeInfo
         */
        //@{

        /**
         * @brief Type creation from type (in templates):
         * TypeInfo::create<Foo>()
         */
		template <typename T>
		static TypeInfo
		create()
		{
			return TypeInfo(typeid(T));
		}

        /**
         * @brief Type creation from objects:
         * TypeInfo::create(foo)
         */
		template <typename T>
		static TypeInfo
		create(const T& t)
		{
			return TypeInfo(typeid(t));
		}
        //@}

	private:
		/**
		 * @brief Returns the demangled C++ name of the class
		 */
		virtual std::string
		doToString() const
		{
            return wns::demangle(name());
		}

        /**
         * @brief Keeps the pointer to the original type_info
         */
		const std::type_info* type_;

        /**
         * @brief Private default c'tor, may not be used ...
         */
 		TypeInfo();
	};
}

#endif // NOT defined WNS_TYPEINFO_HPP
