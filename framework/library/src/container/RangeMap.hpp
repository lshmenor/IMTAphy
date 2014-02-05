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

#ifndef WNS_CONTAINER_RANGEMAP_HPP
#define WNS_CONTAINER_RANGEMAP_HPP

#include <WNS/Interval.hpp>
#include <WNS/Ttos.hpp>
#include <WNS/container/BinaryTree.hpp>

#include <utility>

namespace wns { namespace container {

    /** @brief Maps with intervals.
     *
     *  RangeMap lets you define intervals, for which you want to return certain
     *  values. Say you want to get a 42 for indices between 0.0 and 23.0
     *  and a 5 for indices between 23.8 and 151.6, you can use a
     *  RangeMap<float, int> for this. Intervals must not overlap.
     *
     *  You can create RangeMaps of abstract classes (e.g. RangeMap<float, ArgumentFunctor<float> >),
     *  you just need to insert objects of derived classes.
     *
     *  RangeMap uses a tree internally, but currently does not concern about its balance.
     *  Thus if you insert n intervals in order, get() will need O(n) comparisons, as the
     *  tree will be a list effectively.
     */
    template<typename RangeType, typename ValueType, typename CleanupStrategy = tree::NoneOnErase>
    class RangeMap
    {
    public:

	/** @brief This exception is thrown by RangeMap members
	 */
	class Exception : public wns::Exception
	{
	public:
	    Exception(const std::string& s)
		: wns::Exception(s)
	    {}
        ~Exception() throw() {}
	};


	/** @brief Type of the index used by get().
	 */
	typedef RangeType Index;

	/** @brief Type of the interval used by insert().
	 */
	typedef Interval<Index> IntervalType;

	/** @brief Type of the values stored.
	 */
	typedef ValueType Value;

	/** @brief Default constructor.
	 */
	RangeMap()
	    : root(NULL)
	{}

	/** @brief Destructor.
	 */
	virtual ~RangeMap()
	{
	    delete root;
	}

	/** @brief Insert a value for a given interval.
	 *
	 *  insert() assures, that interval does not
	 *  overlap with an existing interval, and that
	 *  interval is not empty.
	 *
	 *  @param interval Define value for this interval.
	 *  @param value Define this valus.
	 *
	 *  @returns Reference to *this.
	 */
	RangeMap& insert(const IntervalType& interval, const ValueType& value)
	{
	    IntervalValuePair intervalValue = IntervalValuePair(interval, value);
	    assure(!interval.isEmpty(), "RangeMap: Can not insert empty intervals!");
	    if (root == NULL) {
		root = new RangeTree(intervalValue);
	    }
	    else insert(intervalValue, root);
	    return *this;
	}

	/** @brief Get the value for an index.
	 *
	 *  get() throws Exception, when an interval has
	 *  not been defined for index.
	 *
	 *  @param index The index.
	 *  @returns Value for index.
	 */
	const Value& get(const Index& index) const
	{
	    RangeTree* node = root;
	    while (node != NULL) {
		if (node->getValue().first.contains(index)) return node->getValue().second;
		else {
		    if (node->getValue().first.isAbove(index))
			node = node->getSubTree(0);
		    else
			node = node->getSubTree(1);
		}
	    }
	    throw Exception("RangeMap: No value for index " + Ttos(index) + "!");
	}

	/** @brief Check if an interval for index has been defined.
	 *
	 *  @param index The index.
	 *  @returns True, if a range containing index was found, false otherwise.
	 */
	bool has(const Index& index) const
	{
	    try {
		get(index);
	    }
	    catch (Exception) {
		return false;
	    }
	    return true;
	}

    private:

	typedef std::pair<IntervalType, Value> IntervalValuePair;
	/// @todo (swen) balanced tree
	typedef BinaryTree<IntervalValuePair, CleanupStrategy > RangeTree;

	void insert(const IntervalValuePair& intervalValue, RangeTree* node)
	{
	    assure(node != NULL, "RangeMap: Attempt to insert into an empty tree");
	    assure(!intervalValue.first.overlaps(node->getValue().first), "RangeMap: Can only insert disjoint intervals!");

	    const int treeNo = (intervalValue.first.isBelow(node->getValue().first))?0:1;

	    if (node->hasSubTree(treeNo)) insert(intervalValue, node->getSubTree(treeNo));
	    else node->createSubTree(treeNo, intervalValue);
	}

	RangeTree* root;
    };

} // container
} // wns

#endif // NOT defined WNS_CONTAINER_RANGEMAP_HPP
