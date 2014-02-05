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

#ifndef WNS_BIRTHMARK_HPP
#define WNS_BIRTHMARK_HPP

#include <WNS/PythonicOutput.hpp>

namespace wns {

	/**
	 * @brief Unique identifier
	 */
	class Birthmark :
		virtual public PythonicOutput
	{
	public:
		/**
		 * @brief Default constructor
		 */
		Birthmark();

		/**
		 * @brief Virtual destructor
		 */
		virtual
		~Birthmark();

		/**
		 * @brief Equality operator for two birthmarks
		 */
		virtual bool
		operator ==(const Birthmark& other) const;

		/**
		 * @brief Inequality operator for two birthmarks
		 */
		virtual bool
		operator !=(const Birthmark& other) const;

		/**
		 * @brief less operator, needed to use birthmarks as keys in STL
		 * maps
		 */
		virtual bool
		operator <(const Birthmark& other) const;

	private:
		/**
		 * @brief Debugging output (shows the internal realization)
		 */
		virtual std::string
		doToString() const;

		/**
		 * @brief Provides unique ids to make distinguish birthmarks
		 */
		static long long int globalId;

		/**
		 * @brief Local id (makes the birthmark unique)
		 */
		long long int id;
	};


	/**
	 * @brief Provide unique birthmark (identifier) for all clones of one
	 * origin.
	 *
	 * @note Always derive virtual from HasBirthmark!!
	 *
	 * If you happen to have the need to recognize the copy of an object,
	 * the HasBirthmark base class may be of interest for you.
	 *
	 * Here's how it works:
	 *
	 * If you derive as follows:
	 *
	 * @code
	 * #include <WNS/Birthmark.hpp>
	 *
	 * class Foo :
	 * public wns::HasBirthmark
	 * {};
	 * @endcode
	 *
	 * each new instance of your class Foo comes equipped with a new and
	 * unique 'Birthmark' object. You don't have to worry about its
	 * creation. Birthmark objects can be stored locally and have operators
	 * to identify them as being equal or unequal.
	 *
	 * Example:
	 *
	 * @code
	 * Foo a, b; // construct two distinct objects
	 *
	 * Foo copyOfa(a); // explicit copy of a
	 *
	 * Foo copyOfb = b; // assignment of b
	 *
	 * CPPUNIT_ASSERT(a.getBirthmark() != b.getBirthmark());
	 * CPPUNIT_ASSERT(a.getBirthmark() == copyOfa.getBirthmark());
	 * @endcode
	 * ... and so on (assignment is left as an exercise to the reader)
	 *
         *
	 * If you clone/copy an object of your class Foo, each cloned/copied
	 * object will have the same birthmark, enabling you to detect whether
	 * an object is equal to or a clone of some other object.
	 *
	 * Birthmarks are NOT:
	 *
	 * - a guarantee that two objects with the same birthmarks are otherwise
	 * identical. It only means that at some point they had a common
	 * ancestor or that one is the ancestor of the other.
	 *
	 * - a means to detect an object's age, they have no '<' and '>'
	 * operators
	 *
	 */
	class HasBirthmark
	{
	public:
		/**
		 * @brief Default constructor
		 */
		HasBirthmark();

		/**
		 * @brief Destructor
		 */
		virtual
		~HasBirthmark();


		/**
		 * @brief Retrieve the birthmark
		 */
		const Birthmark&
		getBirthmark() const;

	protected:
		/**
		 * @brief Set the birthmark of an object
		 *
		 * @warning Use this with great care! Never make it public
		 * available! This method is normally only needed if you have
		 * special clone methods for an object that don't use the copy
		 * constructor.
		 */
		void
		setBirthmark(const Birthmark& other);

	private:
		/**
		 * @brief Unique identifier
		 */
		Birthmark birthmark;
	};

} // wns

#endif // NOT defined WNS_BIRTHMARK_HPP


