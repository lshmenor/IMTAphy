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

#ifndef WNS_CONTAINER_TREE_HPP
#define WNS_CONTAINER_TREE_HPP

namespace wns { namespace container {

    namespace tree 
    {
        /**
         * @brief This cleanup policy does nothing
         */
        class NoneOnErase
        {
        public:
            template <typename ANY>
            void
            cleanup(ANY&) const
            {
            }
        };
    }

    template<typename ValueType, typename CleanupStrategy = tree::NoneOnErase>
    class Tree
    {
    public:
        typedef ValueType Value;
        typedef std::size_t Size;

    virtual ~Tree()
    {
        CleanupStrategy cleaner;
        cleaner.cleanup( value );
    }

    virtual void setValue(const Value& value)
    {
        this->value = value;
    }

    virtual const Value& getValue() const
    {
        return value;
    }

    virtual Size getNoOfSubtrees() const = 0;

    virtual void createSubTree(const Size& no, const Value& value) = 0;
    virtual void linkSubTree(const Size& no, Tree* subTree) = 0;

    virtual bool hasSubTree(const Size& no) = 0;
    virtual Tree* getSubTree(const Size& no) = 0;
    virtual const Tree *const getSubTree(const Size& no) const = 0;

    protected:

    Tree(const Value& value)
        : father(NULL), value(value)
    {
    }


    Tree* father;

    private:
    Value value;
    };

} // container
} // wns

#endif // NOT defined WNS_CONTAINER_TREE_HPP
