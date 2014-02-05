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

#ifndef WNS_CONTAINER_FASTLIST_HPP
#define WNS_CONTAINER_FASTLIST_HPP

#include <WNS/Assure.hpp>
#include <WNS/container/FastListNode.hpp>
#include <WNS/NonCopyable.hpp>
#include <memory>

namespace wns { namespace container {

    /**
     * @brief List supporting removing and inserting elements with O(1)
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     * @ingroup group_main_classes
     *
     * Enable a class to be used by FastLists:
     * @include wns.container.FastListEnabler.example
     *
     * Define a FastList:
     * @include wns.container.FastListDefinition.example
     *
     * Push some elements to the front of the list:
     * @include wns.container.FastListPushBack.example
     *
     * @todo Marc Schinnenburg: If the element T is derived from
     * SingleFastListEnabler, it would be nice to generate a compile-time error,
     * since copying the list is not allowed in this case.
     */
    template <typename T>
    class FastList
    {
    public:
        /**
         * @brief An STL-sytle iterator for the FastList
         * @author Marc Schinnenburg <marc@schinnenburg.com>
         */
        class iterator
        {
            /**
             * @brief FastList needs to access the fln
             */
            friend class FastList;
        public:
            /**
             * @brief Default Constructor
             *
             * The iterator is invalid after construction
             */
            iterator()
                : fln(0)
            {}

            /**
             * @brief Constructor that takes a FastListNode
             *
             * The iterator is valid after construction
             */
            explicit iterator(FastListNode<T>* f)
                : fln(f)
            {}

            /**
             * @brief Equality
             */
            bool operator==(const iterator& i) const
            {
                return fln==i.fln;
            }

            /**
             * @brief Inequality
             */
            bool operator!=(const iterator& i) const
            {
                return fln!=i.fln;
            }

            /**
             * @brief Dereference operator (acces the data behind
             * the iterator)
             */
            T& operator*()
            {
                return fln->getData();
            }

            /**
             * @brief Pointer operator (acces the data behind
             * the iterator)
             */
            T& operator->()
            {
                return fln->getData();
            }

            /**
             * @brief Prefix increment operator.
             */
            iterator& operator++()
            {
                assure(fln, "Nothing to iterate on");
                fln = fln->getNext();
                return *this;
            };

            /**
             * @brief Postfix increment operator.
             */
            iterator operator++(int)
            {
                assure(fln, "Nothing to iterate on");
                iterator i = *this;
                fln = fln->getNext();
                return i;
            }

            /**
             * @brief Prefix decrement operator.
             */
            iterator& operator--()
            {
                assure(fln, "Nothing to iterate on");
                fln = fln->getPrevious();
                return *this;
            }

            /**
             * @brief Postfix decrement operator.
             */
            iterator operator--(int)
            {
                assure(fln, "Nothing to iterate on");
                iterator i = *this;
                fln = fln->getPrevious();
                return i;
            }

        private:
            /**
             * @brief The node this iterator points to
             */
            FastListNode<T>* fln;
        };

        /**
         * @brief Default Constructor
         */
        FastList()
            : b(FastListNode<T>()),
              s(0)
        {
        }

        /**
         * @brief Destructor - removes all element from the list
         */
        ~FastList()
        {
            clear();
        }

        /**
         * @brief Copy Constructor
         */
        FastList(const FastList& f)
            : b(FastListNode<T>()),
              s(0)
        {
            copy(f);
        }

        /**
         * @brief Assignment operator
         */
        void operator=(const FastList& f)
        {
            clear();
            copy(f);
        }

        /**
         * @brief Iterator to the begin of the list
         */
        iterator begin() const
        {
            return iterator(b.getNext());
        }

        /**
         * @brief Iterator to the end of the list
         *
         * @note This iterator is never valid. Never dereference it!
         */
        iterator end() const
        {
            return iterator(&b);
        }

        /**
         * @brief Get a reference to the first element.
         */
        T& front() const
        {
            return b.getNext()->getData();
        }

        /**
         * @brief Is list empty?
         */
        bool empty() const
        {
            return s==0;
        }

        /**
         * @brief Returns the size of the list
         */
        int size() const
        {
            return s;
        }

        /**
         * @brief Remove alle element from the list
         */
        void clear()
        {
            iterator itr, itrEnd;
            itr = begin();
            itrEnd = end();
            while(itr!=itrEnd)
            {
                erase(itr);
                itr = begin();
                itrEnd = end();
            };
        }

        /**
         * @brief Erase element at iterator's position
         */
        void erase(const iterator& i)
        {
            assure(i!=end(), "Can't erase this->end()");
            assure(s>0, "No elements in list");
            --s;
            FastListNode<T>* fln = i.fln;
            fln->removeFromList();
            fln->getData()->removeFastListNode(this);
            delete fln;
        }

        /**
         * @brief Remove element with value x
         */
        void remove(const T& x)
        {
            assure(s>0, "No elements in list");
            --s;
            FastListNode<T>* fln = x->getFastListNode(this);
            fln->removeFromList();
            x->removeFastListNode(this);
            delete fln;
        }

        /**
         * @brief Remove element with value x
         */
        void remove(T* x)
        {
            assure(s>0, "No elements in list");
            --s;
            FastListNode<T>* fln = x->getFastListNode(this);
            fln->removeFromList();
            x->removeFastListNode(this);
            delete fln;
        }

        /**
         * @brief Add value x at the front of the list
         */
        void push_front(const T& x)
        {
            std::auto_ptr< FastListNode<T> > fln(new FastListNode<T>(x));
            x->setFastListNode(this, fln.get());
            fln->addToList(b);
            ++s;
            // everything ok, release the poiner, now handled by FastList
            fln.release();
        }

        /**
         * @brief Add value x at the front of the list
         */
        void push_front(T* x)
        {
            push_front(&x);
        };

        /**
         * @brief Add value x at the end of the list
         */
        void push_back(const T& x)
        {
            std::auto_ptr< FastListNode<T> > fln(new FastListNode<T>(x));
            x->setFastListNode(this, fln.get());
            fln->addToList(*b.getPrevious());
            ++s;
            // everything ok, release the poiner, now handled by FastList
            fln.release();
        }

        /**
         * @brief Add value x at the end of the list
         */
        void push_back(T* x)
        {
            push_back(&x);
        };

        void pop_front()
        {
            remove(front());
        }

        /**
         * @brief Does the list contain value x?
         */
        bool contains(const T& x)
        {
            return x->isInList(this);
        }

    private:
        /**
         * @brief Special copy for lists
         */
        void copy(const FastList& f)
        {
            iterator itr;
            for(itr=f.begin(); itr!=f.end(); ++itr)
            {
                push_front(*itr);
            }
        }

        /**
         * @brief Sentinel
         */
        mutable FastListNode<T> b;

        /**
         * @brief Size of the list
         */
        int s;
    };
}
}
#endif
