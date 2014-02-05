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

#ifndef WNS_LDK_DROPPER_TEST_HPP
#define WNS_LDK_DROPPER_TEST_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Dropper.hpp>

#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/Exception.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk {
	namespace droppertest
	{
		class DropItLikeItsHot :
			public CommandTypeSpecifier<>,
			public HasReceptor<>,
			public HasConnector<>,
			public HasDeliverer<>,
			public Dropper<DropItLikeItsHot>,
			public Cloneable<DropItLikeItsHot>
		{
		public:
			DropItLikeItsHot(fun::FUN* fuNet) :
					CommandTypeSpecifier<>(fuNet),
					HasReceptor<>(),
					HasConnector<>(),
					HasDeliverer<>(),
					Dropper<DropItLikeItsHot>(),
					Cloneable<DropItLikeItsHot>(),

					ind(0),
					req(0),
					drop(0),

					dropping(false)
			{
			}

			void
			setDropping(bool _dropping)
			{
				dropping = _dropping;
			} // setDropping

			virtual bool
			wouldDrop(const CompoundPtr& /* compound */) const
			{
				return dropping;
			} // wouldDrop

			virtual void
			processIncoming(const CompoundPtr& /* compound */)
			{
				++ind;
			} // processIncoming

			virtual void
			processOutgoing(const CompoundPtr& /* compound */)
			{
				++req;
			} // processOutgoing

			virtual void
			processDropping(const CompoundPtr& /* compound */)
			{
				++drop;
			} // processDropping

			int ind;
			int req;
			int drop;

		private:
			bool dropping;
		};
	}

	class DropperTest : public CppUnit::TestFixture  {
		CPPUNIT_TEST_SUITE( DropperTest );
		CPPUNIT_TEST( testDownlink );
		CPPUNIT_TEST( testUplink );
		CPPUNIT_TEST( testDrop );
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();

		void testDownlink();
		void testUplink();
		void testDrop();

	private:
		ILayer* layer;
		fun::FUN* fuNet;

		tools::Stub* upper;
		droppertest::DropItLikeItsHot* dropper;
		tools::Stub* lower;
	};
}}


#endif // NOT defined WNS_LDK_DROPPER_TEST_HPP


