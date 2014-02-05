/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/convergence/Upper.hpp>
#include <GLUE/convergence/Lower.hpp>
#include <GLUE/convergence/Lower2Copper.hpp>

#include <WNS/service/phy/copper/DataTransmission.hpp>
#include <WNS/service/phy/copper/Notification.hpp>
#include <WNS/service/phy/copper/CarrierSensing.hpp>

#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/service/dll/Address.hpp>

#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/ldk/multiplexer/Dispatcher.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/utils.hpp>

#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/pyconfig/View.hpp>

#include <WNS/Subject.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/Assure.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace glue { namespace convergence { namespace tests {

	struct OnData
	{
		OnData(const wns::osi::PDUPtr& _pdu, double _ber, bool _collision) :
			pdu(_pdu),
			ber(_ber),
			collision(_collision)
		{
		}

		void
		operator()(wns::service::phy::copper::Handler* handler)
		{
			handler->onData(this->pdu, this->ber, this->collision);
		}

	private:
		wns::osi::PDUPtr pdu;
		double ber;
		bool collision;
	};

    // Class that pretends to be a copper phy:
    class PhyShammer :
	public wns::service::phy::copper::DataTransmission,
	public wns::service::phy::copper::Notification
    {
    public:
	PhyShammer();

	virtual
	~PhyShammer();

	// DataTransmission
    public:
		virtual void sendData(const wns::service::dll::BroadcastAddress& address,
							  const wns::osi::PDUPtr& data);
		virtual void sendData(const wns::service::dll::UnicastAddress& address,
							  const wns::osi::PDUPtr& data);
		virtual void cancelData(const wns::osi::PDUPtr& data);
		virtual bool isFree() const;

		wns::osi::PDUPtr lastBroadcastSent;
		wns::osi::PDUPtr lastUnicastSent;
		unsigned int broadcastSendCounter;
		unsigned int unicastSendCounter;

		// Notification
    public:
		virtual void setDLLUnicastAddress(const wns::service::dll::UnicastAddress& address);

    private:
		wns::service::dll::UnicastAddress address;

	// Testing
    public:
		void pushUp(const wns::osi::PDUPtr& data);
    }; // PhyShammer

    // Very simple DLL, just enough for our tests
    class SimpleDLL :
		virtual public wns::service::dll::UnicastDataTransmission,
		virtual public wns::service::dll::BroadcastDataTransmission,
		public wns::ldk::tests::LayerStub
    {
    public:
		SimpleDLL(PhyShammer* phy);

		virtual	~SimpleDLL();

		// DataTransmission
		virtual void sendData(const wns::service::dll::UnicastAddress& address,
							  const wns::osi::PDUPtr& data,
							  wns::service::dll::protocolNumber protocol,
							  int _dllFlowID = 0);
		virtual void sendData(const wns::service::dll::BroadcastAddress& address,
							  const wns::osi::PDUPtr& data,
							  wns::service::dll::protocolNumber protocol,
							  int _dllFlowID = 0);

		virtual wns::service::dll::UnicastAddress
		getMACAddress() const { return this->unicastUpperConvergence->getMACAddress();}

		// pseudo Notification interface
		virtual void registerUnicastHandler(wns::service::dll::Handler* handler);
		virtual void registerBroadcastHandler(wns::service::dll::Handler* handler);

		// Layer
		virtual std::string	getName() const;

    private:
		wns::ldk::fun::Main* fun;

		UnicastUpper* unicastUpperConvergence;
		BroadcastUpper* broadcastUpperConvergence;
		wns::ldk::multiplexer::Dispatcher* dispatcher;
		Lower2Copper* lowerConvergence;
    }; // SimpleDLL

    // Class that pretends to be a network layer
    class NLShammer	:
		public wns::service::dll::Handler
    {
    public:
		NLShammer(wns::service::dll::UnicastDataTransmission* unicastDLL,
				  wns::service::dll::BroadcastDataTransmission* broadcastDLL);

		virtual	~NLShammer();

		// Handler
    public:
		virtual void onData(const wns::osi::PDUPtr& pdu, wns::service::dll::FlowID);

		unsigned int receiveCounter;
		wns::osi::PDUPtr lastPDUReceived;

	// Testing
    public:
		void sendUnicast(const wns::osi::PDUPtr& pdu);

		void sendBroadcast(const wns::osi::PDUPtr& pdu);

		wns::service::dll::UnicastDataTransmission* unicastDLL;
		wns::service::dll::BroadcastDataTransmission* broadcastDLL;
    }; // NLShammer

    class UnicastBroadcastTest :
		public wns::TestFixture
    {
		CPPUNIT_TEST_SUITE( UnicastBroadcastTest );
		CPPUNIT_TEST( outgoingUnicast );
		CPPUNIT_TEST( outgoingBroadcast );
		CPPUNIT_TEST( incomingUnicast );
		CPPUNIT_TEST( incomingBroadcast );
		CPPUNIT_TEST_SUITE_END();

    public:
		virtual void prepare();
		virtual void cleanup();
		void outgoingUnicast();
		void outgoingBroadcast();
		void incomingUnicast();
		void incomingBroadcast();

    private:
		PhyShammer* phy;
		SimpleDLL* dll;
		NLShammer* unicastNL;
		NLShammer* broadcastNL;
    }; // UnicastBroadcastTest

	CPPUNIT_TEST_SUITE_REGISTRATION( UnicastBroadcastTest );

    PhyShammer::PhyShammer()
		: lastBroadcastSent(),
		  lastUnicastSent(),
		  broadcastSendCounter(0),
		  unicastSendCounter(0),
		  address()
    {
    } // PhyShammer::PhyShammer

    PhyShammer::~PhyShammer()
    {
    } // PhyShammer::~PhyShammer

    void PhyShammer::sendData(const wns::service::dll::BroadcastAddress& /* address */,
						 const wns::osi::PDUPtr& data)
    {
		++broadcastSendCounter;
		lastBroadcastSent = data;
    } // PhyShammer::sendData [broadcast]

    void PhyShammer::sendData(const wns::service::dll::UnicastAddress& /* address */,
			 const wns::osi::PDUPtr& data)
    {
		++unicastSendCounter;
		lastUnicastSent = data;
    } // PhyShammer::sendData [unicast]

    void PhyShammer::cancelData(const wns::osi::PDUPtr& /* data */)
    {
		assure(false, "PhyShammer does not support cancelling of data");
    } // PhyShammer::cancelData

    bool PhyShammer::isFree() const
    {
		return true;
    } // PhyShammer::isFree

    void PhyShammer::setDLLUnicastAddress(const wns::service::dll::UnicastAddress& address)
    {
		this->address = address;
    } // PhyShammer::registerHandler

    void PhyShammer::pushUp(const wns::osi::PDUPtr& data)
    {
	    typedef wns::service::phy::copper::Handler Handler;
	    this->wns::Subject<Handler>::forEachObserver(OnData(data, 0, false));
    } // PhyShammer::pushUp

    SimpleDLL::SimpleDLL(PhyShammer* phy) :
		fun(NULL),
		unicastUpperConvergence(NULL),
		broadcastUpperConvergence(NULL),
		lowerConvergence(NULL)
    {
		fun = new wns::ldk::fun::Main(this);

		wns::pyconfig::View unicastUpperConfig = wns::pyconfig::helper::createViewFromDropInConfig("glue.Glue",
																								   "UnicastUpperConvergence");
		unicastUpperConvergence = new UnicastUpper(fun, unicastUpperConfig);
		wns::pyconfig::View broadcastUpperConfig = wns::pyconfig::helper::createViewFromDropInConfig("glue.Glue",
																									 "BroadcastUpperConvergence");
		broadcastUpperConvergence = new BroadcastUpper(fun, broadcastUpperConfig);
		wns::pyconfig::View dispatcherConfig = wns::pyconfig::helper::createViewFromDropInConfig("openwns.Multiplexer",
																								 "Dispatcher",
																								 "1");
		dispatcher = new wns::ldk::multiplexer::Dispatcher(fun, dispatcherConfig);
		wns::pyconfig::View lowerConfig = wns::pyconfig::helper::createViewFromDropInConfig("glue.Glue",
																							"Lower2Copper",
																							"\"unicastRouting\", \"broadcastRouting\"");
		lowerConvergence = new Lower2Copper(fun, lowerConfig);
		fun->addFunctionalUnit("unicastRouting", unicastUpperConvergence);
		fun->addFunctionalUnit("broadcastRouting", broadcastUpperConvergence);
		fun->addFunctionalUnit("dispatcher", dispatcher);
		fun->addFunctionalUnit("lowerConvergence", lowerConvergence);

		unicastUpperConvergence->connect(dispatcher);
		broadcastUpperConvergence->connect(dispatcher);
		dispatcher->connect(lowerConvergence);

		fun->onFUNCreated();

		broadcastUpperConvergence->setMACAddress(wns::service::dll::UnicastAddress(1));
		unicastUpperConvergence->setMACAddress(wns::service::dll::UnicastAddress(1));
		lowerConvergence->setMACAddress(wns::service::dll::UnicastAddress(1));
		lowerConvergence->setDataTransmissionService(phy);
		lowerConvergence->setNotificationService(phy);
    } // SimpleDLL::SimpleDLL

    SimpleDLL::~SimpleDLL()
    {
		delete fun;
    } // SimpleDLL::~SimpleDLL

    void SimpleDLL::sendData(const wns::service::dll::UnicastAddress& /* address */,
							 const wns::osi::PDUPtr& data,
							 wns::service::dll::protocolNumber /*protocol*/,
							 int /*dllFlowID*/)
    {
		wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fun->getProxy()->createCommandPool(), data));
		UnicastUpperCommand* command = dynamic_cast<UnicastUpperCommand*>
			(unicastUpperConvergence->activateCommand(compound->getCommandPool()));
		command->peer.targetMACAddress = wns::service::dll::UnicastAddress(1);
		command->peer.sourceMACAddress = wns::service::dll::UnicastAddress(1);
		if (unicastUpperConvergence->isAccepting(compound))
			unicastUpperConvergence->sendData(compound);
    } // SimpleDLL::sendData [unicast]

    void SimpleDLL::sendData(const wns::service::dll::BroadcastAddress& /* address */,
							 const wns::osi::PDUPtr& data,
							 wns::service::dll::protocolNumber /*protocol*/,
							 int /*dllFlowID*/)
    {
		wns::ldk::CompoundPtr compound(new wns::ldk::Compound(fun->getProxy()->createCommandPool(), data));
		BroadcastUpperCommand* command = dynamic_cast<BroadcastUpperCommand*>(broadcastUpperConvergence->activateCommand(compound->getCommandPool()));
		command->peer.targetMACAddress = wns::service::dll::BroadcastAddress();
		command->peer.sourceMACAddress = wns::service::dll::UnicastAddress(1);
		if (broadcastUpperConvergence->isAccepting(compound))
			broadcastUpperConvergence->sendData(compound);
    } // SimpleDLL::sendData [broadcast]

    void SimpleDLL::registerUnicastHandler(wns::service::dll::Handler* handler)
    {
		unicastUpperConvergence->registerHandler(wns::service::dll::TESTING, handler);
    } // SimpleDLL::registerUnicastHandler

    void SimpleDLL::registerBroadcastHandler(wns::service::dll::Handler* handler)
    {
		broadcastUpperConvergence->registerHandler(wns::service::dll::TESTING, handler);
    } // SimpleDLL::registerBroadcastHandler

    std::string SimpleDLL::getName() const
    {
		return "SimpleDLL";
    } // SimpleDLL::getName

    NLShammer::NLShammer(wns::service::dll::UnicastDataTransmission* unicastDLL,
						 wns::service::dll::BroadcastDataTransmission* broadcastDLL)
		: receiveCounter(0),
		unicastDLL(unicastDLL),
		  broadcastDLL(broadcastDLL)
    {
    } // NLShammer::NLShammer

    NLShammer::~NLShammer()
    {
    } // NLShammer::~NLShammer

    void NLShammer::onData(const wns::osi::PDUPtr& pdu, wns::service::dll::FlowID)
    {
		++receiveCounter;
		lastPDUReceived = pdu;
    } // NLShammer::onData

    void NLShammer::sendUnicast(const wns::osi::PDUPtr& pdu)
    {
		unicastDLL->sendData(wns::service::dll::UnicastAddress(1), pdu, wns::service::dll::TESTING);
    } // NLShammer::sendUnicast

    void NLShammer::sendBroadcast(const wns::osi::PDUPtr& pdu)
    {
		broadcastDLL->sendData(wns::service::dll::BroadcastAddress(), pdu, wns::service::dll::TESTING);
    } // NLShammer::sendBroadcast

    void UnicastBroadcastTest::prepare()
    {
		phy = new PhyShammer();
		dll = new SimpleDLL(phy);
		unicastNL = new NLShammer(dll, dll);
		broadcastNL = new NLShammer(dll, dll);
		dll->registerUnicastHandler(unicastNL);
		dll->registerBroadcastHandler(broadcastNL);
    } // UnicastBroadcastTest::prepare

    void UnicastBroadcastTest::cleanup()
    {
		delete broadcastNL;
		delete unicastNL;
		delete dll;
		delete phy;
    } // UnicastBroadcastTest::cleanup

    void UnicastBroadcastTest::outgoingUnicast()
    {
		unsigned int initialCount = phy->unicastSendCounter;
		unicastNL->sendUnicast(wns::ldk::helper::FakePDUPtr());
		CPPUNIT_ASSERT_EQUAL(initialCount + 1, phy->unicastSendCounter);
    } // UnicastBroadcastTest::outgoingUnicast

    void UnicastBroadcastTest::outgoingBroadcast()
    {
		unsigned int initialCount = phy->broadcastSendCounter;
		broadcastNL->sendBroadcast(wns::ldk::helper::FakePDUPtr());
		CPPUNIT_ASSERT_EQUAL(initialCount + 1, phy->broadcastSendCounter);
    } // UnicastBroadcastTest::outgoinBroadcast

    void UnicastBroadcastTest::incomingUnicast()
    {
		wns::ldk::helper::FakePDU* pdu = new wns::ldk::helper::FakePDU();
		pdu->setPDUType(wns::service::dll::TESTING);

		unicastNL->sendUnicast(wns::ldk::helper::FakePDUPtr(pdu));
		unsigned int initialCount = unicastNL->receiveCounter;
		phy->pushUp(phy->lastUnicastSent);
		CPPUNIT_ASSERT_EQUAL(initialCount + 1, unicastNL->receiveCounter);
    } // UnicastBroadcastTest::incomingUnicast

    void UnicastBroadcastTest::incomingBroadcast()
    {
		wns::ldk::helper::FakePDU* pdu = new wns::ldk::helper::FakePDU();
		pdu->setPDUType(wns::service::dll::TESTING);

		broadcastNL->sendBroadcast(wns::ldk::helper::FakePDUPtr(pdu));
		unsigned int initialCount = broadcastNL->receiveCounter;
		phy->pushUp(phy->lastBroadcastSent);
		CPPUNIT_ASSERT_EQUAL(initialCount + 1, broadcastNL->receiveCounter);
    } // UnicastBroadcastTest::incomingBroadcast

}}}
