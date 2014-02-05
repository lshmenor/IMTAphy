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

#ifndef WNS_CONTAINER_FASTLISTNODE_HPP
#define WNS_CONTAINER_FASTLISTNODE_HPP

namespace wns { namespace container {

    // Forward Decleration of FastList
    template <typename T> class FastList;

    /**
     * @brief Node of a FastList
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    template<typename T> class FastListNode
    {
        /**
         * @brief FastList needs to modify FastListNode
         */
        friend class FastList<T>;

    public:
        /**
         * @brief Constructor - Takes an object that should be stored in
         * a FastList
         */
        explicit FastListNode(const T& x) :
            data(x),
            previousNode(this),
            nextNode(this)
        {
        };

        /**
         * @brief Destructor
         */
        ~FastListNode()
        {
        };

        /**
         * @brief Remove this FastListNode from its FastList
         */
        void removeFromList()
        {
            previousNode->nextNode = nextNode;
            nextNode->previousNode = previousNode;
        };

        /**
         * @brief Add this FastListNode to a FastList
         * @param begin The begin of the FastList where this node should
         * be added.
         */
        void addToList(FastListNode& begin)
        {
            nextNode = begin.nextNode;
            previousNode = &begin;

            begin.nextNode = this;
            nextNode->previousNode = this;
        };

        /**
         * @brief Returns the next node of the list
         */
        FastListNode* getNext() const
        {
            return nextNode;
        }

        /**
         * @brief Returns the previous node of the list
         */
        FastListNode* getPrevious() const
        {
            return previousNode;
        }

        /**
         * @brief Returns the data that this FastListNode represents in
         * the FastList
         */
        T& getData()
        {
            return data;
        };

    protected:
        /**
         * @brief this constructor will only be called for the list sentinel.
         *
         * data(T()) will not be initialized, because we don't know, if T
         * has a default constructor and data will never be used anyway.
         */
        FastListNode() :
            previousNode(this),
            nextNode(this)
        {
        }

    private:
        /**
         * @brief Data that this FastListNode represents in the FastList
         */
        T data;

        /**
         * @brief The node before this node in the list
         */
        FastListNode* previousNode;

        /**
         * @brief The node after this node in the list
         */
        FastListNode* nextNode;
    };
}
}
#endif
