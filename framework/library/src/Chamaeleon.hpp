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

#ifndef WNS_CHAMAELEON_HPP
#define WNS_CHAMAELEON_HPP

#include <WNS/TypeInfo.hpp>
#include <WNS/Exception.hpp>

namespace wns {

    template <typename T> class Chamaeleon;

    /**
     * @brief Base class for Chamaeleon<T>
     * @author Marc Schinnenburg <marc@schinnenburg.net>
     */
    class ChamaeleonBase
    {
    public:
        /**
         * @brief Thrown in case of a bad cast
         */
        class BadCast :
            public Exception
        {
        public:
            /**
             * @brief Constructor which takes the TypeInfo of actual and
             * expected type as input
             */
            BadCast(
                const TypeInfo& expectedType,
                const TypeInfo& actualType) throw();

            /**
             * @brief Destructor
             */
            virtual
            ~BadCast() throw();
        };

        // Default copy constructor is ok

        /**
         * @brief Virtual destructor to support dynamic_cast
         */
        virtual
        ~ChamaeleonBase() throw();

        /**
         * @brief Returns the TypeInfo that has been provided at contruction
         * time
         */
        TypeInfo
        getTypeInfo() const throw();

        /**
         * @brief Type safe downcast
         */
        template <typename T>
        Chamaeleon<T>*
        downCast() throw(BadCast)
        {
            Chamaeleon<T>* chamaeleon = dynamic_cast<Chamaeleon<T>*>(this);
            if(NULL == chamaeleon)
            {
                throw BadCast(TypeInfo::create<T>(), getTypeInfo());
            }
            return chamaeleon;
        }

    protected:
        /**
         * @brief Constructor taking the TypeInfo of the object from
         * Chamaeleon<T>
         *
         * Is protected, since it doesn't make sense to have an instance of this
         * type
         */
        explicit
        ChamaeleonBase(const TypeInfo& typeInfo) throw();

    private:
        /**
         * @brief Stores the TypeInfo object of Chamaeleon<T>
         */
        TypeInfo typeInfo_;
    };

    /**
     * @brief Helps to put objects of different type into one container
     * @author Marc Schinnenburg <marc@schinnenburg.net>
     *
     * The idea is to use this template to give any type the same base class
     * (ChamaeleonBase). This can be done without changing the definition of
     * the type itself.
     *
     * @note Regarding storage the Chamaeleon behaves like a STL-Container ->
     * References are not delete, value types are copied around
     *
     * @note Before you use this template always think twice. Maybe there
     * is a better way. Probably there is a better way ;)
     *
     * @param VALUETYPE The type you want chamaelonize
     *
     * The classes A, B and C used in the examples here are defined as
     * follows:
     * @include wns.ChamaeleonTestClasses.example
     *
     * Example:
     * @include wns.Chamaeleon.example
     */
    template <typename VALUETYPE>
    class Chamaeleon :
        public ChamaeleonBase
    {
        /**
         * @brief Chamaeleon<T> is friend of any Chamaeleon<U>
         */
        template<class> friend class Chamaeleon;
    public:
        /**
         * @brief Public access to VALUETYPE as ValueType
         */
        typedef VALUETYPE ValueType;

        /**
         * @brief Takes an instance of ValueType
         *
         * @param data the data you want to "chamaelonize"
         *
         * Example:
         * @include wns.ChamaeleonConstructor.example
         */
        explicit
        Chamaeleon(ValueType data) throw() :
            ChamaeleonBase(TypeInfo::create<ValueType>()),
            data_(data)
        {
        }

        // Default copy c'tor is ok

        /**
         * @brief Constructor to support conversion for upcasts
         */
        template <typename U>
        explicit
        Chamaeleon(const Chamaeleon<U>& other) throw() :
            ChamaeleonBase(other),
            data_(other.data_)
        {
        }

        /**
         * @brief Destructor
         */
        virtual
        ~Chamaeleon() throw()
        {
        }

        /**
         * @brief Get back the "chamaelonized" data
         *
         * @return data that is "chamaelonized"
         *
         * Example:
         * @include wns.ChamaeleonUnHide.example
         */
        ValueType
        unHide() const throw()
        {
            return data_;
        }

        /**
         * @brief Assignment operator
         */
        template <typename U>
        Chamaeleon&
        operator =(const Chamaeleon<U>& other) throw()
        {
            data_ = other.data_;
            return *this;
        }

    private:
        /**
         * @brief Data that is "chamaelonized"
         */
        ValueType data_;
    };
}

#endif // NOT defined WNS_CHAMAELEON_HPP
