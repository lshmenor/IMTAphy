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

#ifndef _MESSAGETEST_HPP
#define _MESSAGETEST_HPP

#include <WNS/logger/Message.hpp>
#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace logger {
	class MessageTest
		: public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( MessageTest );
		CPPUNIT_TEST( constructor );
		CPPUNIT_TEST( getters );
		CPPUNIT_TEST( stream );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void constructor();
		void getters();
		void stream();
	private:
	};
}
}
#endif


