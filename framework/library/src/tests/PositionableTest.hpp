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

#ifndef WNS_TESTS_POSITIONABLETEST_HPP
#define WNS_TESTS_POSITIONABLETEST_HPP

#include <WNS/Positionable.hpp>
#include <WNS/PositionObserver.hpp>
#include <WNS/Observer.hpp>
#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace tests {
	class PositionableTest
		: public CppUnit::TestFixture
	{
		class PositionableObject :
			public Positionable
		{
		public:
			PositionableObject()
			{
			}

			void move(const Position& p)
			{
				setPosition(p);
			}
		};

		class PositionObserverTester :
			public wns::Observer<PositionObserver>
		{
		public:
			PositionObserverTester() :
				hasBeenInformedBefore(false),
				hasBeenInformedAfter(false)
			{
			}

			void positionWillChange()
			{
				hasBeenInformedBefore = true;
			}

			void positionChanged()
			{
				hasBeenInformedAfter = true;
			}

			bool hasBeenInformedBefore;
			bool hasBeenInformedAfter;

		private:
			PositionObserverTester(const PositionObserverTester& other);
		};

		CPPUNIT_TEST_SUITE( PositionableTest );
		CPPUNIT_TEST( getPosition );
		CPPUNIT_TEST( setPosition );
		CPPUNIT_TEST( getDistance );
        CPPUNIT_TEST( getAngle );
		CPPUNIT_TEST_SUITE_END();
	public:
                PositionableTest();
                ~PositionableTest();
		void setUp();
		void tearDown();
		void getPosition();
		void setPosition();
		void getDistance();
        void getAngle();
	};

} // tests
} // wns
#endif


