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

#ifndef WNS_CONTAINER_REGISTRY_HPP
#define WNS_CONTAINER_REGISTRY_HPP

#include <WNS/Assure.hpp>
#include <WNS/TypeInfo.hpp>
#include <WNS/Exception.hpp>

#include <map>
#include <list>

namespace wns { namespace container {

    /**
     * @brief Registry policies
     */
    namespace registry {
        /**
         * @brief This cleanup policy does nothing
         */
        class NoneOnErase
        {
        public:

            template <typename ANY>
            void
            operator()(ANY&) const
            {
            }
        };

        /**
         * @brief This cleanup policy deletes pointers in the Registry
         *
         * May only be used if pointers are used as values
         */
        class DeleteOnErase
        {
        public:

            template <typename ANY>
            void
            operator()(ANY any) const
            {
                delete any;
            }
        };
    }

    /**
     * @brief Stores elements of type ELEMENT with a key of type KEY
     * @author Marc Schinnenburg <marc@schinnenburg.net>
     * @ingroup group_main_classes
     *
     * The key to the elements must be unique. Keys must provide strict weak
     * ordering.
     *
     * To add an element to the Registy use insert(). To overwrite an
     * existing element use update(). To remove() an element use erase(). To
     * retrieve and element use find().
     *
     * Note, that Registry can automatically manage cleanup of erased
     * elements for you. Then you need to use "DeleteOnErase" as third
     * template parameter. If the ELEMENT type is a pointer type the
     * Registry will then call "delete element" every time an element is
     * erased from the Registy. This happens when using one of the following
     * three methods:
     *  - clear()
     *  - erase()
     *  - update()
     *
     * Registry has the important property that inserting a new element into
     * a Registry does not invalidate iterators that point to existing
     * elements. Erasing an element from a Registry also does not invalidate
     * any iterators, except, of course, for iterators that actually point
     * to the element that is being erased.
     */
    template<
        typename KEY,
        typename ELEMENT,
        typename CLEANUPPOLICY = registry::NoneOnErase,
        typename SORTINGPOLICY = std::less<KEY> >
    class Registry:
        public IOutputStreamable
    {

        /**
         * @brief Container type for elements
         */
        typedef std::map<KEY, ELEMENT, SORTINGPOLICY> ElementContainer;
        typedef typename ElementContainer::iterator iterator;

    public:
        typedef KEY KeyType;
        typedef ELEMENT ElementType;
        typedef std::list<KEY> KeyList;

        /**
         * @brief Thrown if a duplicate key is detected in the Registry
         */
        class DuplicateKeyValue :
            public Exception
        {
        public:
            explicit
            DuplicateKeyValue(const KeyType& key) :
                Exception("Duplicate key in registry detected.\n")
            {
                (*this) << TypeInfo::create<ElementType>()
                        << " with this key already registered.\n"
                        << "Key: " << key;
            }

            virtual
            ~DuplicateKeyValue() throw()
            {
            }
        };

        /**
         * @brief Thrown if the key the Registry is asked is not
         * available
         */
        class UnknownKeyValue :
            public Exception
        {
        public:
            UnknownKeyValue(const KeyType& key, const Registry& reg) :
                Exception()
            {
                (*this) << "No " << TypeInfo::create<ElementType>()
                        << " with this key registered.\n"
                        << "Key: " << key << "\n"
                        << reg;
            }

            virtual
            ~UnknownKeyValue() throw()
            {
            }
        };

        /**
         * @brief Exports const_iterator for external usage
         */
        typedef typename ElementContainer::const_iterator const_iterator;

        /**
         * @brief Creates an empty registry
         */
        Registry() :
            elements_()
        {
        }

        /**
         * @brief All elements/keys are removed
         *
         * The elements are removed according to the CLEANUPPOLICY
         */
        virtual
        ~Registry()
            throw()
        {
            this->clear();
        }

        /**
         * @brief Clears the container
         */
        virtual void
        clear()
        {
            CLEANUPPOLICY cleanup;
            while (this->elements_.empty() == false)
            {
                cleanup(this->elements_.begin()->second);
                this->elements_.erase(this->elements_.begin());
            }
        }

        /**
         * @brief add an element with using a unique key
         *
         * @pre An element with this key MAY NOT have been registered before
         */
        virtual void
        insert(const KeyType& key, ElementType element)
            throw(DuplicateKeyValue)
        {
            // Store size of map before potentially adding
            size_t sizeBeforeInserting = this->size();
            // This should add an element, if not the key was
            // already used
            ElementType& slotInContainer = this->elements_[key];
            // Store size of map after potentially adding
            size_t sizeAfterInserting = this->size();

            if (sizeAfterInserting - sizeBeforeInserting == 0)
            {
                // The key was already in use
                throw DuplicateKeyValue(key);
            }
            slotInContainer = element;
        }


        /**
         * @brief Update the element found by key
         *
         * @pre An element with this key MUST NOT have been registered
         * before
         *
         * @exception UnkownKeyValue Thrown if element to key is not
         * found
         */
        virtual void
        update(const KeyType& key, ElementType element)
            throw(UnknownKeyValue)
        {
            iterator itr = this->elements_.find(key);
            if (itr == this->elements_.end())
            {
                throw UnknownKeyValue(key, *this);
            }
            CLEANUPPOLICY cleanup;
            cleanup(itr->second);
            itr->second = element;

        }

        /**
         * @brief erase the element according to its key it has been
         * registerd with
         *
         * @pre An element with this key MUST have been registered
         * before
         *
         * @exception UnkownKeyValue Thrown if element to key is not
         * found
         *
         */
        virtual void
        erase(const KeyType& key)
            throw(UnknownKeyValue)
        {
            iterator itr = this->elements_.find(key);
            if (itr == this->elements_.end())
            {
                throw UnknownKeyValue(key,*this);
            }

            CLEANUPPOLICY cleanup;
            cleanup(itr->second);
            this->elements_.erase(itr);
        }

        /**
         * @brief Return an element according to the key it has been
         * registerd with
         *
         * @pre An element with this key MUST have been registered
         * before
         *
         * @exception UnkownKeyValue Thrown if element to key is not
         * found
         */
        virtual const ElementType&
        find(const KeyType& key) const
            throw(UnknownKeyValue)
        {
            const_iterator itr = this->elements_.find(key);
            if (itr == this->elements_.end())
            {
                throw UnknownKeyValue(key, *this);
            }
            return itr->second;
        }

        /**
         * @brief Return an element according to the key it has been
         * registerd with
         *
         * @pre An element with this key MUST have been registered
         * before
         *
         * @exception UnkownKeyValue Thrown if element to key is not
         * found
         */
        virtual ElementType&
        find(const KeyType& key)
            throw(UnknownKeyValue)
        {
            iterator itr = this->elements_.find(key);
            if (itr == this->elements_.end())
            {
                throw UnknownKeyValue(key, *this);
            }
            return itr->second;
        }

        /**
         * @brief STL-style iterator interface
         */
        virtual const_iterator
        begin() const
            throw()
        {
            return this->elements_.begin();
        }

        /**
         * @brief STL-style iterator interface
         */
        virtual const_iterator
        end() const
            throw()
        {
            return this->elements_.end();
        }

        /**
         * @brief Is element known?
         */
        virtual bool
        knows(const KeyType& key) const
            throw()
        {
            return this->elements_.find(key) != elements_.end();
        }

        /**
         * @brief True if size of Registry is 0
         */
        virtual bool
        empty() const
            throw()
        {
            return this->elements_.empty();
        }

        /**
         * @brief Return the size of the registry
         */
        virtual size_t
        size() const
            throw()
        {
            return this->elements_.size();
        }

        /**
         * @brief Return list with copy of all keys
         */
        virtual KeyList
        keys() const
            throw()
        {
            KeyList retVal;
            for (const_iterator iter = begin(); iter != end(); ++iter)
            {
                retVal.push_back(iter->first);
            }
            return retVal;
        }

    private:
        std::string
        doToString() const
        {
            std::stringstream repr;

            repr << "Elements with the following keys are registered:\n";

            for (typename Registry::const_iterator itr = elements_.begin();
                 itr != elements_.end();
                 ++itr)
                {
                    repr << " - " << itr->first << "\n";
                }

            return repr.str();
        }

        /**
         * @brief Strores the elements of the Registry
         */
        ElementContainer elements_;
    }; // Registry
} // container
} // wns

#endif // NOT defined WNS_REGISTRY_HPP
