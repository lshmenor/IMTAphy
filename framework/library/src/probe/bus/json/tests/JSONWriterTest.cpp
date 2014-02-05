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

#include <WNS/probe/bus/json/writer.hpp>
#include <WNS/probe/bus/json/reader.hpp>
#include <WNS/probe/bus/json/elements.hpp>

#include <WNS/TestFixture.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace probe  { namespace bus { namespace json { namespace tests {

class JSONWriterTest:
    public wns::TestFixture
{
    CPPUNIT_TEST_SUITE( JSONWriterTest );
    CPPUNIT_TEST( testMethod );
    CPPUNIT_TEST_SUITE_END();

public:

    void
    prepare();

    void
    cleanup();

    void
    testMethod();
};

CPPUNIT_TEST_SUITE_REGISTRATION( JSONWriterTest );

void JSONWriterTest::prepare()
{

}

void
JSONWriterTest::cleanup()
{
}

void
JSONWriterTest::testMethod()
{
   // we can build a document piece by piece...
    wns::probe::bus::json::Object objAPA;
    objAPA["Name"] = wns::probe::bus::json::String("Schlafly American Pale Ale");
    objAPA["Origin"] = wns::probe::bus::json::String("St. Louis, MO, USA");
    objAPA["ABV"] = wns::probe::bus::json::Number(3.8);
    objAPA["BottleConditioned"] = wns::probe::bus::json::Boolean(true);

    wns::probe::bus::json::Array arrayBeer;
    arrayBeer.Insert(objAPA);

    wns::probe::bus::json::Object objDocument;
    objDocument["Delicious Beers"] = arrayBeer;

    wns::probe::bus::json::Number numDeleteThis = objDocument["AnotherMember"];

   // ...or, we can use UnknownElement's chaining child element access to quickly
   //  construct the remainder

    objDocument["Delicious Beers"][1]["Name"] = wns::probe::bus::json::String("John Smith's Extra Smooth");
    objDocument["Delicious Beers"][1]["Origin"] = wns::probe::bus::json::String("Tadcaster, Yorkshire, UK");
    objDocument["Delicious Beers"][1]["ABV"] = wns::probe::bus::json::Number(3.8);
    objDocument["Delicious Beers"][1]["BottleConditioned"] = wns::probe::bus::json::Boolean(false);

    std::stringstream stream;
    wns::probe::bus::json::Writer::Write(objDocument, stream);

    wns::probe::bus::json::Object elemRootFile;
    wns::probe::bus::json::Reader::Read(elemRootFile, stream);

    CPPUNIT_ASSERT(objDocument == elemRootFile);
}

} // tests
} // json
} // bus
} // probe
} // wns
