/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#ifndef WNS_ROUNDROBIN_HPP
#define WNS_ROUNDROBIN_HPP

#include <WNS/Assure.hpp>

#include <vector>
#include <algorithm>

namespace wns {

	template <typename T>
	class RoundRobin
	{
		typedef T Element;
		typedef std::vector<Element> Container;
	public:
		explicit
		RoundRobin() :
			elements(),
			first(0),
			inRound(false),
			cursor(0)
		{
		}

		/**
		 * @brief Start a new round
		 *
		 * This effectively seals the container. In debug mode no add()
		 * or remove() is possible.
		 */
		void
		startRound() const
		{
			assure(!inRound, "May only be called after endRound or after constructor");
			inRound = true;
			countdown = size();
			cursor = first;
		}

		/**
		 * @brief End a round
		 *
		 * Call this basically to unseal the container (see startRound())
		 */
		void
		endRound()
		{
			assure(inRound, "May only be called after startRound");
			inRound = false;
			first = cursor;
		}

		/**
		 * @brief Cancel an already started round
		 */
		void
		cancelRound() const
		{
			assure(inRound, "May only be called after startRound");
			inRound = false;
			cursor = first;
		}

		/**
		 * @brief Return if currently in round
		 */
		bool
		isInRound() const
		{
			return(inRound);
		}

		/**
		 * @brief Returns the next element to operate on without advancing to the
		 * element after the next element
		 *
		 * @note May also be called while not in round
		 */
		const Element
		current() const
		{
			assure(cursor < size(), "Cursor beyond bounds");
			return elements[cursor];
		}


		/**
		 * @brief Returns the next element to operate on
		 *
		 * @return The next element to operate on
		 */
		const Element
		next() const
		{
			assure(inRound, "May only be called after startRound");
			assure(countdown > 0, "No more Elements");
			assure(cursor < size(), "Cursor beyond bounds");
			--countdown;
			int current = cursor;
			advanceCursor();
			return elements[current];
		}

		/**
		 * @brief True as long as there elements left in one round
		 *
		 * True as long as there elements left in one round, i.e. not
		 * all elements have been visited.
		 */
		bool
		hasNext() const
		{
			assure(inRound, "May only be called after startRound");
			return countdown > 0;
		}

		/**
		 * @brief Number of elements in the container
		 */
		long int
		size() const
		{
			return elements.size();
		}

		/**
		 * @brief True if container empty
		 */
		bool
		empty() const
		{
			return elements.empty();
		}

		/**
		 * @brief Add an element to the container
		 *
		 * @note Elements must be unique
		 *
		 * Calling add will insert the new value just before the current
		 * first position.
		 */
		void
		add(Element e)
		{
			assure(std::find(elements.begin(),
					 elements.end(),
					 e) == elements.end(),
			       "element to be added must be unique");

			assure(!inRound, "May only be called when not in round");
			if(first==0) {
				elements.push_back(e);
			} else {
				elements.insert(elements.begin()+first-1, e);
			}
		}

		/**
		 * @brief Remove an element from the container
		 *
		 * Removing an element will keep the first pointing to the same
		 * element after removing. If the element is removed the first
		 * is currently pointing to, the first is advanced to the next
		 * element.
		 */
		void
		remove(Element e)
		{
			assure(!inRound, "May only be called when not in round");
			assure(std::find(elements.begin(),
					 elements.end(),
					 e) != elements.end(),
			       "element to be removed not contained");

			for(size_t ii = 0;
			    ii < elements.size();
			    ++ii)
			{
				if(elements[ii] == e)
				{
					elements.erase(elements.begin()+ii);
					if(ii < static_cast<size_t>(first))
					{
						--first;
					}
				}
			}
		}

		void
		clear()
		{
			assure(!inRound, "May only be called when not in round");
			elements.clear();
		}

		/**
		 * @brief Returns all elements
		 *
		 * @note May be used for introspection only.
		 */
		Container
		getAllElements() const
		{
			return elements;
		}

	private:
		void advanceCursor() const
		{
			++cursor %= size();
		}
		Container elements;
		long int first;
		mutable bool inRound;
		mutable long int cursor;
		mutable long int countdown;
	};
} // wns

#endif // not defined WNS_ROUNDROBIN_HPP



