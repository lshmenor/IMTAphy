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

#ifndef _CLONEABLETEST_HPP
#define _CLONEABLETEST_HPP

#include <WNS/Cloneable.hpp>
#include <cppunit/extensions/HelperMacros.h>

namespace wns {

	class CloneableTest	:
		public CppUnit::TestFixture
	{
		class CloneableTestClass :
			public Cloneable<CloneableTestClass>
		{
		public:
			CloneableTestClass() :
				Cloneable<CloneableTestClass>()
			{
				value = new int( 42 );
			}

			CloneableTestClass( const CloneableTestClass& rhs )	:
				CloneableInterface(),
				Cloneable<CloneableTestClass>()
			{
				value = new int( *(rhs.value) );
			}

			virtual ~CloneableTestClass()
			{
				delete value;
			}

			bool operator==( const CloneableTestClass& rhs )
			{
				return *value == *(rhs.value);
			}

		private:
			int* value;
		};

		class NotCloneableTestClass :
			public NotCloneable
		{
		};

		CPPUNIT_TEST_SUITE( CloneableTest );
		CPPUNIT_TEST( clone );
		CPPUNIT_TEST( cloneTypeSafe );
		CPPUNIT_TEST( cloneAutoPtrTypeSafe );
		CPPUNIT_TEST( notCloneableException );
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp(){}
		void tearDown(){}
		void clone();
		void cloneTypeSafe();
		void cloneAutoPtrTypeSafe();
		void notCloneableException();
	};

}

#endif


