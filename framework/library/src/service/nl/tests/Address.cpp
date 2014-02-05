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

#include <cmath>

#include <iostream>

#include <WNS/service/nl/Address.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace service { namespace nl { namespace tests {
	class AddressTest
		: public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( AddressTest );
		CPPUNIT_TEST( Conversion );
		CPPUNIT_TEST( StreamOperator );
		CPPUNIT_TEST( EqualityOperator );
		CPPUNIT_TEST( InequalityOperator );
		CPPUNIT_TEST( AssignOperator );
		CPPUNIT_TEST( LessThanOperator );
		CPPUNIT_TEST( GreaterThanOperator );
		CPPUNIT_TEST( LessEqualOperator );
		CPPUNIT_TEST( GreaterEqualOperator );
		CPPUNIT_TEST( IncrementPrefixOperator );
		CPPUNIT_TEST( IncrementPostfixOperator );
		CPPUNIT_TEST( DecrementPrefixOperator );
		CPPUNIT_TEST( DecrementPostfixOperator );
		CPPUNIT_TEST( AndOperator );
		CPPUNIT_TEST( OrOperator );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void Conversion();
		void StreamOperator();
		void EqualityOperator();
		void InequalityOperator();
		void AssignOperator();
		void LessThanOperator();
		void GreaterThanOperator();
		void LessEqualOperator();
		void GreaterEqualOperator();
		void IncrementPrefixOperator();
		void IncrementPostfixOperator();
		void DecrementPrefixOperator();
		void DecrementPostfixOperator();
		void AndOperator();
		void OrOperator();

	private:
	};
} // tests
} // nl
} // service
} // wns

using namespace wns::service::nl::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( AddressTest );

void AddressTest::setUp()
{}

void AddressTest::tearDown()
{}

void AddressTest::Conversion()
{
	nl::Address a("127.1.1.1");
	nl::Address b( (127<<24) + (1<<16) + (1<<8) + 1 );
	// Is it equal
	CPPUNIT_ASSERT(a == b);
	b = nl::Address ( (127<<24) + (1<<16) + (1<<8) + 2 );
	// it should not be equal to 127.1.1.2
	CPPUNIT_ASSERT(a !=  b);
	// But this should
	a = nl::Address("127.1.1.2");
	CPPUNIT_ASSERT(a == b);
	// Start with numerical 127.1.1.2
	a = nl::Address(b);
	CPPUNIT_ASSERT(a == nl::Address("127.1.1.2"));
	CPPUNIT_ASSERT(a != nl::Address("127.22.1.2"));
}


void AddressTest::StreamOperator()
{
	nl::Address a("127.1.1.1");
	std::stringstream tmp;
	tmp << a;
	std::string s;
	tmp >> s;
	CPPUNIT_ASSERT(s == "127.1.1.1");
}

void AddressTest::EqualityOperator()
{

	nl::Address a("34.67.23.4");
	nl::Address b("34.67.23.4");
	nl::Address c("137.226.4.220");

	CPPUNIT_ASSERT(a==b);
	CPPUNIT_ASSERT(b==a);
	CPPUNIT_ASSERT(not (a==c));
	CPPUNIT_ASSERT(not (b==c));
}

void AddressTest::InequalityOperator()
{

	nl::Address a("34.67.23.4");
	nl::Address b("34.67.23.4");
	nl::Address c("137.226.4.220");

	CPPUNIT_ASSERT(a!=c);
	CPPUNIT_ASSERT(b!=c);
	CPPUNIT_ASSERT(c!=a);
	CPPUNIT_ASSERT(c!=b);
	CPPUNIT_ASSERT(not (a!=b));
}

void AddressTest::AssignOperator()
{
	nl::Address a("17.2.17.4");
	nl::Address b("4.4.2.3");
	CPPUNIT_ASSERT(a!=b);
	b = a;
	CPPUNIT_ASSERT(a==b);
}

void AddressTest::LessThanOperator()
{
	nl::Address a("137.226.4.220");
	nl::Address b("137.226.4.220");
	nl::Address c("137.226.4.221");

	CPPUNIT_ASSERT(a<c);
	CPPUNIT_ASSERT(not (a<b));
}

void AddressTest::GreaterThanOperator()
{
	nl::Address a("137.226.4.220");
	nl::Address b("137.226.4.220");
	nl::Address c("137.226.4.221");

	CPPUNIT_ASSERT(c>a);
	CPPUNIT_ASSERT(not (a>b));
}

void AddressTest::LessEqualOperator()
{
	nl::Address a("137.226.4.220");
	nl::Address b("137.226.4.220");
	nl::Address c("137.226.4.221");

	CPPUNIT_ASSERT(a<=c);
	CPPUNIT_ASSERT(a<=b);
	CPPUNIT_ASSERT(not(c<=a));
}

void AddressTest::GreaterEqualOperator()
{
	nl::Address a("137.226.4.220");
	nl::Address b("137.226.4.220");
	nl::Address c("137.226.4.221");

	CPPUNIT_ASSERT(c>=a);
	CPPUNIT_ASSERT(a>=b);
	CPPUNIT_ASSERT(not(a>=c));
}

void AddressTest::IncrementPrefixOperator()
{
	nl::Address a("80.13.17.4");
	nl::Address b("80.13.17.5");

	CPPUNIT_ASSERT(a!=b);
	CPPUNIT_ASSERT(b==++a);
}

void AddressTest::IncrementPostfixOperator()
{
	nl::Address a("80.13.17.4");
	nl::Address b("80.13.17.5");

	CPPUNIT_ASSERT(a!=b);
	CPPUNIT_ASSERT(b!=a++);
	CPPUNIT_ASSERT(b==a);
}

void AddressTest::DecrementPrefixOperator()
{
	nl::Address a("80.13.27.18");
	nl::Address b("80.13.27.19");

	CPPUNIT_ASSERT(a!=b);
	CPPUNIT_ASSERT(a==--b);
}

void AddressTest::DecrementPostfixOperator()
{
	nl::Address a("80.13.27.18");
	nl::Address b("80.13.27.19");

	CPPUNIT_ASSERT(a!=b);
	CPPUNIT_ASSERT(a!=b--);
	CPPUNIT_ASSERT(a==b);
}

void AddressTest::AndOperator()
{
	nl::Address a("127.1.1.1");
	nl::Address m("255.255.255.0");
	nl::Address net = a&m;
	CPPUNIT_ASSERT(net==nl::Address("127.1.1.0"));
}

void AddressTest::OrOperator()
{
	nl::Address a("127.1.1.1");
	nl::Address m("255.255.255.0");
	nl::Address net = a|m;
	CPPUNIT_ASSERT(net==nl::Address("255.255.255.1"));
}


/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
