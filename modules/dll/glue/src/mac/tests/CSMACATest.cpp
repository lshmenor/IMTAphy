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

#include <GLUE/mac/CSMACA.hpp>

#include <WNS/ldk/tests/FUTestBase.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Average.hpp>

namespace glue { namespace mac { namespace tests {

	/**
	 * @todo This test is currently disabled: It is not clear how a
	 * FunctionalUnit should be tested that
	 *
	 * a) produces a number of events. The processOneEvent() method does not
	 * apply since with further patches the FU may produce a different number
	 * of events. If the functionality does not change it would be nice if
	 * the test doesn't need to be changed.
	 *
	 * b) uses a periodically timeout mechanism. The number of events is
	 * then ifinite.
	 */
	class CSMACATest:
		public wns::ldk::tests::FUTestBase
	{
		CPPUNIT_TEST_SUITE(CSMACATest);
		//CPPUNIT_TEST ( testData );
		CPPUNIT_TEST_SUITE_END();

		virtual void
		setUpTestFUs()
		{
			wns::pyconfig::Parser parser;
			parser.loadString(
					  "from glue.Glue import CSMACAMAC\n"
					  "from wns.ARQ import StopAndWait\n"
					  "import wns.FUN\n"
					  "csmaca = CSMACAMAC(commandName = 'foo', stopAndWaitARQName = 'arq', phyNotification = 'phyNotfication')\n"
					  "arq = wns.FUN.Node('arq', wns.ARQ.StopAndWait(resendTimeout = 0.005))\n"
					  "\n");
			this->testee = new CSMACA(this->getFUN(), parser.get("csmaca"));
			this->arq = new wns::ldk::arq::StopAndWait(this->getFUN(), parser.get("arq"));
			this->getFUN()->addFunctionalUnit(parser.get("arq").get<std::string>("name"), this->arq);
		}

		virtual void
		tearDownTestFUs()
		{
			this->testee = NULL;
			this->arq = NULL;
		}

		virtual wns::ldk::FunctionalUnit*
		getUpperTestFU() const
		{
			return this->arq;
		}

		virtual wns::ldk::FunctionalUnit*
		getLowerTestFU() const
		{
			return this->testee;
		}

		void
		testData()
		{
			wns::ldk::tools::Stub* lower = this->getLowerStub();
			wns::ldk::tools::Stub* upper = this->getUpperStub();
			lower->setStepping(false);
			upper->setStepping(false);
			wns::Average<simTimeType> average;
			for(int ii = 0; ii < 2000; ++ii)
			{
				wns::ldk::CompoundPtr compound = this->newFakeCompound();
				this->sendCompound(compound);
				while(wns::simulator::getEventScheduler()->processOneEvent() == true)
				{
				}
				average.put(this->getSojurnTime(compound));
			}
			CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(2000), lower->sent.size() );
			simTimeType expectedSojurnTime =
				//   DIFS       +  mean(0...7)*slotLength
				10E-6 + 2*10E-6 + 3.5*10E-6;
			WNS_ASSERT_MAX_REL_ERROR(expectedSojurnTime, average.get(), 1E-2);
		}

	private:
		CSMACA* testee;
		wns::ldk::arq::StopAndWait* arq;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( CSMACATest );

} // tests
} // mac
} // glue
