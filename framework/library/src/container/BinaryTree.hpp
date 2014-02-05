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

#ifndef WNS_CONTAINER_BINARYTREE_HPP
#define WNS_CONTAINER_BINARYTREE_HPP

#include <WNS/container/Tree.hpp>
#include <WNS/Assure.hpp>

#include <vector>

namespace wns { namespace container {

    template<typename ValueType, typename CleanupStrategy = tree::NoneOnErase>
    class BinaryTree :
    public Tree<ValueType, CleanupStrategy>
    {
    typedef Tree<ValueType, CleanupStrategy> Super;
    public:
    typedef ValueType Value;
    typedef typename Super::Size Size;

    BinaryTree(const Value& value = Value())
           : Super(value), tree(2)
    {
    }

    virtual ~BinaryTree()
    {
        for (typename std::vector<BinaryTree*>::size_type i = 0; i < 2; ++i)
        if (tree[i] != NULL) delete tree[i];
    }

    virtual Size getNoOfSubtrees() const
    {
        return 2;
    }

    virtual void createSubTree(const Size& no, const Value& value)
    {
        setSubTree(tree[no], new BinaryTree(value));
    }

    virtual void linkSubTree(const Size& no, Super* subTree)
    {
        setSubTree(tree[no], dynamic_cast<BinaryTree*>(subTree));
    }

    virtual bool hasSubTree(const Size& no)
    {
        return getSubTree(no) != NULL;
    }

    virtual BinaryTree* getSubTree(const Size& no)
    {
        return tree[no];
    }

    virtual const BinaryTree *const getSubTree(const Size& no) const
    {
        return tree[no];
    }

    private:

    void setSubTree(BinaryTree*& tree, BinaryTree* newTree)
    {
        if (tree != NULL) delete tree;
        tree = newTree;
        tree->father = this;
    }

    std::vector<BinaryTree*> tree;
    };

} // container
} // wns
#endif // NOT defined WNS_CONTAINER_BINARYTREE_HPP
