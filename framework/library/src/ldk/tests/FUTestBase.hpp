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

#ifndef WNS_LDK_TESTS_FUTESTBASE_HPP
#define WNS_LDK_TESTS_FUTESTBASE_HPP

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/CppUnit.hpp>

namespace wns { namespace ldk { namespace tests {

	/**
	 * @brief Base class for FU tests.
	 *
	 * This class sets up a layer, fun, and two stubs.
	 * Deriving classes set up the FU chain they wish to
	 * test, which is then placed between the two stubs.
	 */
	class FUTestBase :
		public wns::TestFixture
	{
	public:

		FUTestBase();

		virtual
		~FUTestBase();

	protected:

		/**
		 * @brief Returns a Compound carrying a FakePDU
		 */
		virtual CompoundPtr
		newFakeCompound();

		/**
		 * @brief Sends a compound through the upper stub.
		 *
		 * @return Last compound sent by the lower stub.
		 */
		virtual CompoundPtr
		sendCompound(const CompoundPtr& compound);

		/**
		 * @brief Let the lower stub receive a compound.
		 *
		 * @return Last compound received by the upper stub.
		 */
		virtual CompoundPtr
		receiveCompound(const CompoundPtr& compound);

		/**
		 * @brief Number of calls to sendCompound.
		 */
		unsigned int
		compoundsAccepted() const;

		/**
		 * @brief Number of compounds sent by the lower stub.
		 */
		unsigned int
		compoundsSent() const;

		/**
		 * @brief Number of calls to receiveCompound.
		 */
		unsigned int
		compoundsReceived() const;

		/**
		 * @brief Number of compounds received by the upper stub.
		 */
		unsigned int
		compoundsDelivered() const;

		/**
		 * @brief Return the FUN.
		 */
		fun::FUN*
		getFUN();

		/**
		 * @brief Return the lower stub.
		 */
		tools::Stub*
		getLowerStub() const;

		/**
		 * @brief Return the upper stub.
		 */
		tools::Stub*
		getUpperStub() const;

		simTimeType
		getSojurnTime(const CompoundPtr& compound) const;

		virtual void
		prepare();

		virtual void
		cleanup();

		/**
		 * @brief Set up the FU chain to test.
		 *
		 * This method sets up the FUs which should be tested,
		 * adds them to the FUN and connects them. They must
		 * not be connected to the stubs.
		 */
		virtual void
		setUpTestFUs() = 0;

		/**
		 * @brief Tear down the FU chain to test.
		 *
		 * @note Note that FUs that have been added to a FUN don't
		 * need to be deleted.
		 */
		virtual void
		tearDownTestFUs() = 0;

	private:
		/**
		 * @brief Returns the FU that should be connected to the upper stub.
		 */
		virtual FunctionalUnit*
		getUpperTestFU() const = 0;

		/**
		 * @brief Returns the FU that should be connected to the lower stub.
		 */
		virtual FunctionalUnit*
		getLowerTestFU() const = 0;

		/**
		 * @brief Return a new instance of the upper stub.
		 */
		virtual tools::Stub*
		newUpperStub();

		/**
		 * @brief Return a new instance of the lower stub.
		 */
		virtual tools::Stub*
		newLowerStub();

		virtual wns::ldk::ILayer*
		newLayer();

		ILayer* layer;
		fun::Main* fun;

		tools::Stub* upperStub;
		tools::Stub* lowerStub;

		unsigned int accepted;
		unsigned int received;
	};

} // tests
} // ldk
} // wns

#endif // WNS_LDK_TESTS_FUTESTBASE_HPP
