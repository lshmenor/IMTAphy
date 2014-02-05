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

#ifndef WNS_CONTAINER_UNTYPEDREGISTRY_HPP
#define WNS_CONTAINER_UNTYPEDREGISTRY_HPP

#include <memory>
#include <WNS/container/Registry.hpp>
#include <WNS/Chamaeleon.hpp>

namespace wns { namespace container {

    /**
     * @brief Stores references to instances of any type with a KEY
     * @author Marc Schinnenburg <marc@schinnenburg.net>
     */
    template <typename KEY,
              typename SORTINGPOLICY = std::less<KEY> >
    class UntypedRegistry
    {
        // ATTENTION: CLEANUPPOLICY here refers to cleanup of
        // ChamaeleonBase type, which must be deleted in any case
        typedef wns::container::Registry<KEY,
                                         ChamaeleonBase*,
                                         wns::container::registry::DeleteOnErase,
                                         SORTINGPOLICY> InstanceContainer;
    public:
        typedef KEY KeyType;
        typedef typename InstanceContainer::UnknownKeyValue UnknownKeyValue;
        typedef typename InstanceContainer::DuplicateKeyValue DuplicateKeyValue;
        typedef typename InstanceContainer::KeyList KeyList;
        typedef ChamaeleonBase::BadCast BadCast;

        /**
         * @brief Empty registry
         */
        UntypedRegistry() :
            instances_()
        {
        }

        /**
         * @brief All entries in the registry will be erased
         */
        virtual
        ~UntypedRegistry()
        {
        }

        /**
         * @brief add an instance with a unique name
         */
        template<typename ELEMENT>
        void
        insert(const KEY& key, ELEMENT element)
            throw(DuplicateKeyValue)
        {
            // to avoid memory leak in case of exception we use an auto_ptr
            std::auto_ptr<ChamaeleonBase>
                chamaeleonPtr(new Chamaeleon<ELEMENT>(element));
            instances_.insert(key, chamaeleonPtr.get());
            // release the auto_ptr, everything went fine, registry takes
            // control
            chamaeleonPtr.release();
        }

        /**
         * @brief Is an instance with this name known?
         */
        virtual bool
        knows(const KEY& key) const
        {
            return instances_.knows(key);
        }

        /**
         * @brief return an instance of type T according to the name it
         * has been registerd with
         */
        template <typename T>
        T
        find(const KEY& key) const
            throw(UnknownKeyValue, BadCast)
        {

            ChamaeleonBase* cb = instances_.find(key);
            return cb->downCast<T>()->unHide();
        }

        /**
         * @brief Update the element found by key
         *
         * @pre An element with this key MUST NOT have been registered
         * before
         *
         * @exception UnkownKeyValue Thrown if element to key is not
         * found
         *
         * @todo Marc Schinnenburg: If the need arises we can also offer
         * a method "updateTypeSafe", which alows updating only, if the
         * type to be updated is the same.
         */
        template <typename ELEMENT>
        void
        update(const KeyType& key, ELEMENT element)
            throw(UnknownKeyValue)
        {
            // to avoid memory leak in case of exception we use an auto_ptr
            std::auto_ptr<ChamaeleonBase>
                chamaeleonPtr(new Chamaeleon<ELEMENT>(element));
            instances_.update(key, chamaeleonPtr.get());
            // release the auto_ptr, everything went fine, registry takes
            // control
            chamaeleonPtr.release();
        }

        /**
         * @brief remove the instance registered under the given name,
         */
        virtual void
        erase(const KEY& key)
            throw(UnknownKeyValue)
        {
            instances_.erase(key);
        }


        /**
         * @brief Clears the container
         */
        virtual void
        clear()
        {
            instances_.clear();
        }

        /**
         * @brief True if size of Registry is 0
         */
        virtual bool
        empty() const
            throw()
        {
            return instances_.empty();
        }

        /**
         * @brief Return the size of the registry
         */
        virtual size_t
        size() const
            throw()
        {
            return instances_.size();
        }

        virtual KeyList
        keys() const
            throw()
        {
            return instances_.keys();
        }

    private:
        // Disallow Copy Constructor
        UntypedRegistry(const UntypedRegistry&);

        InstanceContainer instances_;
    };

} // container
} // wns
#endif // NOT defined WNS_CONTAINER_UNTYPEDREGISTRY_HPP

