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

#ifndef WNS_PYCONFIG_PARSER_TEST_HPP
#define WNS_PYCONFIG_PARSER_TEST_HPP

#include <WNS/pyconfig/Parser.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace pyconfig { namespace tests {

	class ParserTest :
		public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( ParserTest );
		CPPUNIT_TEST( testLoadString );
		CPPUNIT_TEST( testFromString );
		CPPUNIT_TEST( testAppendPath );
		CPPUNIT_TEST_SUITE_END(); // FIXME(fds): TEST_EXCEPTION TestCases missing
	public:
		void setUp();
		void tearDown();

		void testLoadString();
		void testFromString();
		void testAppendPath();
	};

}}}

#endif // NOT defined WNS_PYCONFIG_PARSER_TEST_HPP



