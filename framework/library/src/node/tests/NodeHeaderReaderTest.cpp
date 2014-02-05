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

#include <WNS/node/Registry.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>

#include <WNS/TestFixture.hpp>

namespace wns { namespace node { namespace tests {
   class Node
   {
       typedef wns::container::Registry<std::string, boost::any> ContainerType;
       ContainerType converters;

   public:

       template<typename R, typename V>
       void
       registerConverter(std::string converter, boost::function1<R, V> func)
           {
               boost::any wrapper = func;
               converters.insert(converter, wrapper);
           }

       template<typename R, typename V>
       R
       convert(std::string converter, V value)
           {
               assure(converters.knows(converter), "Unknown converter");

               boost::any callableAnyType = converters.find(converter);
               boost::function1<R, V> callable;
               callable = boost::any_cast<boost::function1<R, V> >(callableAnyType);
               return callable(value);
           }
   };

   class KnowsHowToConvert
   {
   public:
       KnowsHowToConvert(std::string name, Node* node)
           {
               boost::function1<std::string, int> func;
               func = boost::bind(&KnowsHowToConvert::convert, this, _1);
               node->registerConverter(name, func);
           }

       std::string
       convert(int i)
           {
               if (i==0)
               {
                   return "NULL";
               } else {
                   return "NOTNULL";
               }
           }
   };

   class NodeHeaderReaderTest :
        public wns::TestFixture
   {
       CPPUNIT_TEST_SUITE( NodeHeaderReaderTest );
       CPPUNIT_TEST( main );
       CPPUNIT_TEST_SUITE_END();

   public:
       void prepare();
       void cleanup();
       void main();

   };

} // tests
} // node
} // wns

using namespace wns::node::tests;

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( NodeHeaderReaderTest , wns::testsuite::Spike() );

void
NodeHeaderReaderTest::prepare()
{
}

void
NodeHeaderReaderTest::cleanup()
{
}

void
NodeHeaderReaderTest::main()
{
    Node node;

    KnowsHowToConvert converter("converter", &node);

    std::string result = node.convert<std::string, int>("converter", 0);

    CPPUNIT_ASSERT(result == "NULL");
}
