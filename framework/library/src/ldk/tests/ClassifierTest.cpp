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

#include "ClassifierTest.hpp"
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/pyconfig/Parser.hpp>

using namespace wns;
using namespace wns::ldk;
using namespace wns::ldk::classifiertest;

CPPUNIT_TEST_SUITE_REGISTRATION( ClassifierTest );

void
ClassifierTest::setUp()
{
    CommandProxy::clearRegistries();

    layer = new tests::LayerStub();
    fuNet = new fun::Main(layer);

    wns::pyconfig::Parser emptyConfig;
    classifier =
	new Classifier<IncementingClassificationPolicy>(fuNet, emptyConfig);
    classifier->setModulo(2);

    fuNet->addFunctionalUnit("ernie", classifier);

    lower = new tools::Stub(fuNet, emptyConfig);

    classifier->connect(lower);
}

void
ClassifierTest::tearDown()
{
    delete lower;
    delete fuNet;
    delete layer;
}

void
ClassifierTest::classify()
{
    CompoundPtr compound1(fuNet->createCompound());
    classifier->sendData(compound1);
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    ClassifierCommand* command1 =
	classifier->getCommand( compound1->getCommandPool() );
    CPPUNIT_ASSERT( command1->peer.id == 1 );

    CompoundPtr compound2(fuNet->createCompound());
    classifier->sendData(compound2);
    CPPUNIT_ASSERT(lower->sent.size() == 2);
    ClassifierCommand* command2 =
	classifier->getCommand( compound2->getCommandPool() );
    CPPUNIT_ASSERT( command2->peer.id == 0 );

    CompoundPtr compound3(fuNet->createCompound());
    classifier->sendData(compound3);
    CPPUNIT_ASSERT(lower->sent.size() == 3);
    ClassifierCommand* command3 =
	classifier->getCommand( compound3->getCommandPool() );
    CPPUNIT_ASSERT( command3->peer.id == 1 );
}

