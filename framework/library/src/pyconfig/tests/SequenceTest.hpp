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

#ifndef WNS_PYCONFIG_SEQUENCE_TEST_HPP
#define WNS_PYCONFIG_SEQUENCE_TEST_HPP

#include <WNS/pyconfig/Parser.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns {	namespace pyconfig {
	class SequenceTest :
		public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( SequenceTest );
		CPPUNIT_TEST( testEmpty );
		CPPUNIT_TEST( testSize );
		CPPUNIT_TEST( testAt );
		CPPUNIT_TEST( testTypedEmpty );
		CPPUNIT_TEST( testTypedIter );
		CPPUNIT_TEST( testTypedDeref );
		CPPUNIT_TEST_SUITE_END(); // FIXME(fds): TEST_EXCEPTION TestCases missing
	public:
		void setUp();
		void tearDown();

		void testEmpty();
		void testSize();
		void testAt();
		void testTypedEmpty();
		void testTypedIter();
		void testTypedDeref();
	};

}}

#endif // NOT defined WNS_PYCONFIG_SEQUENCE_TEST_HPP



