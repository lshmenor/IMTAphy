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

#ifndef WNS_CONTAINER_FASTLISTENABLER_HPP
#define WNS_CONTAINER_FASTLISTENABLER_HPP

#include <WNS/container/FastList.hpp>
#include <WNS/NonCopyable.hpp>
#include <map>

namespace wns { namespace container {
    /**
     * @brief Derive from this class be able to put pointers of the child
     * class into FastList containers
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    template<typename T>
    class FastListEnabler :
        private NonCopyable
    {
        /**
         * @brief Map with FastList as key and FastListNode as data
         */
        typedef std::map<FastList<T>*, FastListNode<T>*> MapType;
    public:
        /**
         * @brief Default constructor
         */
        FastListEnabler()
            : listsAndNodes(MapType())
        {
        }

        /**
         * @brief Destructor - Removes this objects from all lists
         */
        virtual ~FastListEnabler()
        {
            removeFromAllLists();
        }

        /**
         * @brief Return the FastListNode belonging to this objects and a
         * specific FastList
         */
        FastListNode<T>* getFastListNode(FastList<T>* f) const
        {
            assure(listsAndNodes.find(f)!=listsAndNodes.end(), "Node not in that list");
            return listsAndNodes.find(f)->second;
        };

        /**
         * @brief Set FastListNode belonging to this objects and a
         * specific FastList
         */
        void setFastListNode(FastList<T>* f, FastListNode<T>* FLN)
        {
            assure(f, "Inavlid list");
            assure(FLN, "Invalid node");
            assure(listsAndNodes.find(f)==listsAndNodes.end(), "Node already in that list");
            listsAndNodes[f] = FLN;
        };

        /**
         * @brief Remove this object from a specific FastList
         */
        void removeFastListNode(FastList<T>* f)
        {
            assure(listsAndNodes.find(f)!=listsAndNodes.end(), "Node not in that list");
            listsAndNodes.erase(f);
        };

        /**
         * @brief Remove this Node from all lists where it is contained
         */
        void removeFromAllLists()
        {
            typename MapType::iterator itr;
            itr = listsAndNodes.begin();
            while(itr != listsAndNodes.end())
            {
                itr->first->remove(itr->second->getData());
                itr = listsAndNodes.begin();
            }
            assure(listsAndNodes.empty(), "Destructor executet, but list not empty");
        }

        /**
         * @brief Is this object in  FastList "f"
         */
        bool isInList(FastList<T>* f) const
        {
            return listsAndNodes.find(f)!=listsAndNodes.end();
        }

    private:
        /**
         * @brief Map with all FastLists and FastListNodes of this object
         */
        MapType listsAndNodes;
    };


    /**
     * @brief Derive from this class be able to put pointers of the child class into
     * FastList containers
     *
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     *
     * @note This Enabler is considerably faster than the one above. As drawback you
     * may only be in one list at a time
     */
    template<typename T>
    class SingleFastListEnabler :
        private NonCopyable
    {
    public:
        /**
         * @brief Default constructor
         */
        SingleFastListEnabler()
            : list(NULL),
              node(NULL)
        {
        };

        /**
         * @brief Destructor - removes from object from the FastList
         */
        virtual ~SingleFastListEnabler()
        {
            removeFromAllLists();
        };

        /**
         * @brief Return the FastListNode
         */
#ifndef WNS_NDEBUG
        FastListNode<T>* getFastListNode(FastList<T>* l) const
#else
        FastListNode<T>* getFastListNode(FastList<T>* /*l*/) const
#endif
        {
            assure(list==l, "Node not in list");
            assure(node, "Inavlid node");
            return node;
        };

        /**
         * @brief Set the FastListNode for s FastList
         */
        void setFastListNode(FastList<T>* f, FastListNode<T>* FLN)
        {
            assure(f, "Inavlid node");
            assure(FLN, "Inavlid list");
            assure(!list, "Node with SingleFastListEnabler can't be in more than one list");
            assure(!node, "Node with SingleFastListEnabler can't be in more than one list");
            list = f;
            node = FLN;
        };

        /**
         * @brief Remove this object from the FastList
         */
#ifndef WNS_NDEBUG
        void removeFastListNode(FastList<T>* f)
#else
        void removeFastListNode(FastList<T>* /*f*/)
#endif
        {
            assure(list, "Node not in list");
            assure(list==f, "Node not in this list");
            assure(node, "Node invalid");
            list=NULL;
            node=NULL;
        };

        /**
         * @brief Remove this Node from all lists where it is contained
         */
        void removeFromAllLists()
        {
            if(list || node)
            {
                assure(list, "Invalid list");
                assure(node, "Invalid node");
                list->remove(node->getData());
            }
        }
        /**
         * @brief Is this object in  FastList "f"
         */
        bool isInList(FastList<T>* f) const
        {
            return list==f;
        }

    private:
        /**
         * @brief The list this object is part of
         */
        FastList<T>* list;

        /**
         * @brief The FastListNode belonging to the FastList this object
         * is part of
         */
        FastListNode<T>* node;
    };
}
}
#endif
