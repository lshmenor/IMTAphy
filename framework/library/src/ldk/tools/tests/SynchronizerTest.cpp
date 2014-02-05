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

#include <WNS/ldk/tests/DelayedInterfaceTest.hpp>
#include <WNS/ldk/tools/Synchronizer.hpp>
#include <WNS/ldk/tools/Producer.hpp>
#include <WNS/ldk/buffer/Bounded.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace tools { namespace tests {

	class SynchronizerTest :
		public wns::ldk::tests::DelayedInterfaceTest
	{
		CPPUNIT_TEST_SUB_SUITE( SynchronizerTest, wns::ldk::tests::DelayedInterfaceTest );
		CPPUNIT_TEST( somePDUs );
		CPPUNIT_TEST_SUITE_END();
	public:
		void
		somePDUs();

	private:
		virtual void
		setUpTestFUs();

		virtual Synchronizer*
		newTestee();

		virtual void
		tearDownTestee(DelayedInterface*)
		{
		}

		virtual FunctionalUnit*
		getLowerTestFU() const;

		Producer* upper;
		buffer::Bounded *buffer;
		Stub* middle;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( SynchronizerTest );

	void
	SynchronizerTest::setUpTestFUs()
	{
		wns::ldk::tests::DelayedInterfaceTest::setUpTestFUs();

		wns::pyconfig::Parser pyco;
		pyco.loadString("from openwns.Buffer import Bounded\n"
						"buffer = Bounded(size = 10)\n"
						);
		wns::pyconfig::View view(pyco, "buffer");

		middle = new tools::Stub(getFUN(), pyco);
		buffer = new buffer::Bounded(getFUN(), view);

		getFUN()->addFunctionalUnit("middle", middle);
		getFUN()->addFunctionalUnit("buffer", buffer);
		getTestee<FunctionalUnit>()
			->connect(middle)
			->connect(buffer);
		middle->close();
	} // setUpTestFUs()
	
	Synchronizer*
	SynchronizerTest::newTestee()
	{
		pyconfig::Parser emptyConfig;
		return new Synchronizer(getFUN(), emptyConfig);
	} // newTestee

	FunctionalUnit*
	SynchronizerTest::getLowerTestFU() const
	{
		return buffer;
	} // getLowerTestFU

	void
	SynchronizerTest::somePDUs()
	{
		Producer* upper = new Producer(getFUN());
		getFUN()->addFunctionalUnit("producer", upper);
		upper->connect(getUpperStub());

		middle->open(false);

		getLowerStub()->close();
		buffer->wakeup();

		CPPUNIT_ASSERT(buffer->getSize() == 10);
		CPPUNIT_ASSERT(upper->sent == 11);
	} // somePDUs

}}}}


