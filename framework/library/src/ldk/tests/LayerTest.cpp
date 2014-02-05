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

#include "LayerTest.hpp"

#include <WNS/pyconfig/Parser.hpp>

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/buffer/Dropping.hpp>

#include <iostream>

using namespace wns;

CPPUNIT_TEST_SUITE_REGISTRATION( LayerTest );

void
LayerTest::setUp()
{
	layer = new wns::ldk::tests::LayerStub();
} // setUp


void
LayerTest::tearDown()
{
	delete layer;
} // tearDown

// simple Management Service
class MService :
	public virtual wns::ldk::ManagementServiceInterface,
	public wns::ldk::ManagementService
{
public:
	MService( wns::ldk::ManagementServiceRegistry* msr )
		: wns::ldk::ManagementService( msr ) {}
	void onMSRCreated() {}
};

// simple Control Service
class CService :
	public virtual wns::ldk::ControlServiceInterface,
	public wns::ldk::ControlService
{
public:
	CService( wns::ldk::ControlServiceRegistry* csr )
		: wns::ldk::ControlService( csr ) {}

	void onCSRCreated() {}
};

void
LayerTest::testManagementService()
{
	MService* a = new MService( layer->getMSR() );
	MService* b = new MService( layer->getMSR() );
	layer->addManagementService("hallo",a);
	layer->addManagementService("welt",b);

	CPPUNIT_ASSERT(layer->getManagementService<MService>("hallo") == a);
	CPPUNIT_ASSERT(layer->getManagementService<MService>("welt") == b);
}

void
LayerTest::testControlService()
{
	CService* a = new CService( layer->getCSR() );
	CService* b = new CService( layer->getCSR() );
	layer->addControlService("hallo",a);
	layer->addControlService("welt",b);

	CPPUNIT_ASSERT(layer->getControlService<CService>("hallo") == a);
	CPPUNIT_ASSERT(layer->getControlService<CService>("welt") == b);
}


// begin example "Layer:InterfaceDefinition.example"
class SimpleTestLayer :
	public wns::ldk::Layer
{
public:
	// Method to process outgoing compounds.
	virtual void doSendData(osi::PDUPtr pdu);
	// Method to process incoming compounds.
	virtual void doOnData(osi::PDUPtr pdu);

	virtual std::string getName() const { return "SimpleTestLayer"; }
};
// end example

// dummy implementation to provide typeinfo
void SimpleTestLayer::doSendData( osi::PDUPtr ) {}
void SimpleTestLayer::doOnData( osi::PDUPtr ) {}

// begin example "Layer:FunctionalUnitDeclaration.example"
class ExampleFunctionalUnit :
	virtual public wns::ldk::FunctionalUnit,
	public wns::ldk::CommandTypeSpecifier<wns::ldk::EmptyCommand>,
	public wns::ldk::HasReceptor<>,
	public wns::ldk::HasConnector<>,
	public wns::ldk::HasDeliverer<>,
	public wns::Cloneable<ExampleFunctionalUnit>
{
public:
	ExampleFunctionalUnit( wns::ldk::fun::FUN* fuNet );


	// Handles compounds from higher functional units
	virtual void doSendData(const wns::ldk::CompoundPtr& sdu);

	// Handles compounds from lower functional units
	virtual void doOnData(const wns::ldk::CompoundPtr& compound);

private:
	// CompoundHandlerInterface
	// Returns true if the FunctionalUnit is able to accept Compounds from higher FunctionalUnit.
	virtual bool doIsAccepting( const wns::ldk::CompoundPtr& compound ) const;

	// Wakes the FunctionalUnit and is called from lower FunctionalUnit whenever they accept compounds again.
	virtual void doWakeup();
};
// end example

// begin example "Layer:InterfaceDefinition2.example"
class TestLayer :
	public wns::ldk::Layer
{
public:
	TestLayer(const wns::pyconfig::View& _config);

	// Method to process outgoing compounds.
	virtual void doSendData( osi::PDUPtr pdu );
	// Method to process incoming compounds.
	virtual void doOnData( osi::PDUPtr pdu );

	virtual std::string getName() const { return "TestLayer"; }

private:
	wns::ldk::fun::FUN* fuNet;
	wns::ldk::tools::Stub* topStub;
	wns::ldk::buffer::Dropping* buffer;
	ExampleFunctionalUnit* exampleFunctionalUnit;
	wns::ldk::tools::Stub* bottomStub;

	wns::pyconfig::View config;
};
// end example

ExampleFunctionalUnit::ExampleFunctionalUnit(wns::ldk::fun::FUN* fuNet) :
		wns::ldk::CommandTypeSpecifier<wns::ldk::EmptyCommand>(fuNet)
{}

bool ExampleFunctionalUnit::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
	return getConnector()->getAcceptor(compound)->isAccepting(compound);
}

void ExampleFunctionalUnit::doOnData(const wns::ldk::CompoundPtr&)
{}

void ExampleFunctionalUnit::doSendData(const wns::ldk::CompoundPtr&)
{}

void ExampleFunctionalUnit::doWakeup()
{}

// begin example "Layer:ConstructorDefinition.example"
TestLayer::TestLayer(const wns::pyconfig::View& _config)	:
	wns::ldk::Layer(),
	config( _config )
{
	pyconfig::Parser emptyConfig;

	// create a fuNet
	fuNet = new wns::ldk::fun::Main(this);

	// create functionalUnits
	topStub = new wns::ldk::tools::Stub(fuNet, emptyConfig);

	// create and configure drop tail buffer
	wns::pyconfig::View droptailConfig(config, "Dropping");
	buffer = new wns::ldk::buffer::Dropping(fuNet, droptailConfig);

	// create the example functionalUnit
	exampleFunctionalUnit = new ExampleFunctionalUnit(fuNet);
	bottomStub = new wns::ldk::tools::Stub(fuNet, emptyConfig );

	// connect functionalUnits
	topStub
		->connect(buffer)
		->connect(exampleFunctionalUnit)
		->connect(bottomStub);
}
// end example

// begin example "Layer:sendDataAndonDataDefinition.example"
void TestLayer::doSendData(osi::PDUPtr sdu)
{
	// create a PDU that can be handled by the functionalUnits
	wns::ldk::CompoundPtr compound(fuNet->createCompound(sdu));
	// let the topStub handle the PDU
	topStub->sendData(compound);
}

void TestLayer::doOnData(osi::PDUPtr pdu)
{
	// this is a Compound
	wns::ldk::CompoundPtr command = dynamicCast<wns::ldk::Compound>(pdu);
	// let the lowest functionalUnit handle this pdu
	bottomStub->onData(command);
}
// end example



