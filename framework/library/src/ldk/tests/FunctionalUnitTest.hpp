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
#ifndef WNS_LDK_FUNCTIONALUNIT_TEST_HPP
#define WNS_LDK_FUNCTIONALUNIT_TEST_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/Exception.hpp>

#include <WNS/Exception.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns {

	namespace testFunctionalUnit
	{
		using namespace wns::ldk;

		enum {
			FUNKY_LENGTH = 23,
			PUNKY_LENGTH = 42
			};

// begin example "FunctionalUnit:FunkyPCIDeclaration.example"

		class FunkyCommand :
			public Command
		{
		public:
			FunkyCommand()
			{
				local.somethingFunky = 23;
				local.destructorCalled = NULL;
			}

			~FunkyCommand()
			{
				if(NULL != local.destructorCalled)
					*local.destructorCalled = true;
			}

			virtual Bit getSize() const
			{
				return FUNKY_LENGTH;
			}

			struct {
				long somethingFunky;
				long *destructorCalled;
			} local;
			struct {} peer;
			struct {} magic;
		};
// end example

// begin example "FunctionalUnit:FunkyFunctionalUnitDeclaration.example"
		class FunkyFunctionalUnit :
			public virtual FunctionalUnit,
			public CommandTypeSpecifier<FunkyCommand>,
			public HasReceptor<>,
			public HasConnector<>,
			public HasDeliverer<>,
			public Cloneable<FunkyFunctionalUnit>
		{
		public:
			FunkyFunctionalUnit(fun::FUN* fuNet) :
					CommandTypeSpecifier<FunkyCommand>(fuNet),
					HasReceptor<>(),
					HasConnector<>(),
					HasDeliverer<>(),
					Cloneable<FunkyFunctionalUnit>(),

					destructorCalled(false)
			{}

			long foo(CommandPool* commandPool)
			{
				FunkyCommand* command = activateCommand(commandPool);

				command->local.destructorCalled = &destructorCalled;
				return command->local.somethingFunky;
			}

			long bar(CommandPool* commandPool)
			{
				FunkyCommand* command = getCommand(commandPool);

				command->local.destructorCalled = &destructorCalled;
				return command->local.somethingFunky;
			}

			long destructorCalled;

			void doOnData(const CompoundPtr&)
			{}

			void doSendData(const CompoundPtr&)
			{}

		private:
			// CompoundHandlerInterface
			bool doIsAccepting(const CompoundPtr&) const
			{
				return true;
			}

			void doWakeup()
			{}
		};
// end example

		class PunkyCommand :
			public Command
		{
		public:
			PunkyCommand()
			{
				local.somethingPunky = 42;
			}
			~PunkyCommand()
			{}

			virtual Bit getSize() const
			{
				return PUNKY_LENGTH;
			} // getLength

			struct {
				long somethingPunky;
			} local;
			struct {} peer;
			struct {} magic;
		};

		class PunkyFunctionalUnit :
			public virtual FunctionalUnit,
			public CommandTypeSpecifier<PunkyCommand>,
			public HasReceptor<>,
			public HasConnector<>,
			public HasDeliverer<>,
			public Cloneable<PunkyFunctionalUnit>
		{
		public:
			PunkyFunctionalUnit(fun::FUN* fuNet) :
					CommandTypeSpecifier<PunkyCommand>(fuNet),
					HasReceptor<>(),
					HasConnector<>(),
					HasDeliverer<>(),
					Cloneable<PunkyFunctionalUnit>()
			{}

			long foo(CommandPool* commandPool)
			{
				PunkyCommand* command = activateCommand(commandPool);

				return command->local.somethingPunky;
			}


			void doOnData(const CompoundPtr&)
			{}

			void doSendData(const CompoundPtr&)
			{}

		private:
			// CompoundHandlerInterface
			bool doIsAccepting(const CompoundPtr&) const
			{
				return true;
			}

			void doWakeup()
			{}
		};
	}


	class FunctionalUnitTest : public CppUnit::TestFixture  {
		CPPUNIT_TEST_SUITE( FunctionalUnitTest );
		CPPUNIT_TEST( testProxy );
		CPPUNIT_TEST( testDestructor );
//		CPPUNIT_TEST_EXCEPTION( testProxyFixation, wns::Exception );
//		CPPUNIT_TEST_EXCEPTION( testWrongCollection, wns::Exception );
#ifdef WNS_ASSURE_THROWS_EXCEPTION
		CPPUNIT_TEST_EXCEPTION( testWrongRoleName, wns::Assure::Exception );
#endif
		CPPUNIT_TEST_EXCEPTION( testDuplicateFUName, wns::Exception );
		CPPUNIT_TEST( testDuplicateRoleName );
#ifdef WNS_ASSURE_THROWS_EXCEPTION
		CPPUNIT_TEST_EXCEPTION( testWrongCommandType, wns::Assure::Exception );
#endif
		CPPUNIT_TEST( testCopy );
		CPPUNIT_TEST( testPartialCopy );
		CPPUNIT_TEST( testRegisterCommandTypeSpecifier );
		CPPUNIT_TEST( testRemoveFunctionalUnit );
		CPPUNIT_TEST_EXCEPTION( testRemoveUnknownFunctionalUnit, wns::Exception );
		CPPUNIT_TEST_EXCEPTION( testRemoveFunctionalUnitNotAddedToFUN, wns::Exception );
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();

		void testProxy();
		void testDestructor();
// 		void testProxyFixation();
// 		void testWrongCollection();
 		void testWrongRoleName();
		void testDuplicateFUName();
		void testDuplicateRoleName();
		void testWrongCommandType();
		void testCopy();
		void testPartialCopy();
		void testRegisterCommandTypeSpecifier();
		void testRemoveFunctionalUnit();
		void testRemoveUnknownFunctionalUnit();
		void testRemoveFunctionalUnitNotAddedToFUN();

	private:
		wns::ldk::ILayer* layer;
		wns::ldk::fun::FUN* fuNet;

		testFunctionalUnit::FunkyFunctionalUnit* fsl;
		testFunctionalUnit::PunkyFunctionalUnit* psl;
		testFunctionalUnit::FunkyFunctionalUnit* bangFunctionalUnit;
		wns::ldk::ILayer* bangLayer;
		wns::ldk::fun::FUN* bangFUN;
		wns::ldk::CommandPool* bangCollection;
	};

} // wns

#endif // NOT defined WNS_LDK_FUNCTIONALUNIT_TEST_HPP


