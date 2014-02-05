/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2010
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WNS/CppUnit.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <IMTAPHY/detail/LookupTable.hpp>

#include <IMTAPHY/detail/Interpolation.hpp>

namespace imtaphy { namespace detail { namespace tests {
        class InterpolationTest :
            public CppUnit::TestFixture
        {
            CPPUNIT_TEST_SUITE( InterpolationTest );
            CPPUNIT_TEST ( test1D );
            CPPUNIT_TEST ( test2D );
            CPPUNIT_TEST ( test3D );
            CPPUNIT_TEST_SUITE_END();
        
        public:
            void setUp();
            void tearDown();

            void test1D();
            void test2D();
            void test3D();

        private:
        };

        CPPUNIT_TEST_SUITE_REGISTRATION( InterpolationTest );
    
        void
        InterpolationTest::setUp()
        {
        }

        void
        InterpolationTest::tearDown()
        {
        }
    
        void
        InterpolationTest::test1D()
        {
            Interpolation<float, float, 1> interpolation;
            
            LookupTable<float, float, 1>::HyperCubeType hyperCube(getResultShape<LookupTable<float, float, 1>::HyperCubeType, 1>());
            
            KeysValuePair<float, float> lowerCorner;
            KeysValuePair<float, float> upperCorner;

            lowerCorner.keys = std::vector<float>(1, 0);
            upperCorner.keys = std::vector<float>(1, 1.0);
            
            lowerCorner.value = 0.0;
            upperCorner.value = 1.0;
            
            hyperCube[Lower] = lowerCorner;
            hyperCube[Upper] = upperCorner;
            
            std::vector<float> target(1);
            
            
            target[0] = 0.5;
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, interpolation.linear(hyperCube, target), 1e-05);

            target[0] = 0.0;
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, interpolation.linear(hyperCube, target), 1e-05);

            target[0] = 1.0;
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, interpolation.linear(hyperCube, target), 1e-05);
			
			lowerCorner.keys = std::vector<float>(1, 1.0);
            upperCorner.keys = std::vector<float>(1, 1.0);
            
            lowerCorner.value = 1.0;
            upperCorner.value = 1.0;
			hyperCube[Lower] = lowerCorner;
            hyperCube[Upper] = upperCorner;
            
           
			target[0] = 1.0;
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, interpolation.linear(hyperCube, target), 1e-05);
			
			
        }
            
        void
        InterpolationTest::test2D()
        {
            /*
            This recreates the Matlab 2D interpolation example, see http://www.mathworks.com/help/techdoc/ref/interp2.html
              
            Given this set of employee data,

            years = 1950:10:1990;
            service = 10:10:30;
            wage = [150.697 199.592 187.625
            179.323 195.072 250.287
            203.212 179.092 322.767
            226.505 153.706 426.730
            249.633 120.281 598.243];
            it is possible to interpolate to find the wage earned in 1975 by an employee with 15 years' service:

            w = interp2(service,years,wage,15,1975)
            w =
            190.6287*/
            
            Interpolation<float, float, 2> interpolation;
            
            LookupTable<float, float, 2>::HyperCubeType hyperCube(getResultShape<LookupTable<float, float, 2>::HyperCubeType, 2>());
           
            KeysValuePair<float, float> lowerLowerCorner;
            KeysValuePair<float, float> lowerUpperCorner;
            KeysValuePair<float, float> upperLowerCorner;
            KeysValuePair<float, float> upperUpperCorner;
            
            float lowerLowerKeys[2] = {10.0, 1970};
            float lowerUpperKeys[2] = {10.0, 1980};
            float upperLowerKeys[2] = {20.0, 1970};
            float upperUpperKeys[2] = {20.0, 1980};

            lowerLowerCorner.keys = std::vector<float>(lowerLowerKeys, lowerLowerKeys + 2);
            lowerUpperCorner.keys = std::vector<float>(lowerUpperKeys, lowerUpperKeys + 2);
            upperLowerCorner.keys = std::vector<float>(upperLowerKeys, upperLowerKeys + 2);
            upperUpperCorner.keys = std::vector<float>(upperUpperKeys, upperUpperKeys + 2);

            
            lowerLowerCorner.value = 203.212;
            lowerUpperCorner.value = 226.505;
            upperLowerCorner.value = 179.092;
            upperUpperCorner.value = 153.706;
            
            hyperCube[Lower][Lower] = lowerLowerCorner;
            hyperCube[Lower][Upper] = lowerUpperCorner;
            hyperCube[Upper][Lower] = upperLowerCorner;
            hyperCube[Upper][Upper] = upperUpperCorner;
            
            std::vector<float> target(2);
            
            
            target[0] = 15;
            target[1] = 1975;
            CPPUNIT_ASSERT_DOUBLES_EQUAL(190.6287, interpolation.linear(hyperCube, target), 1e-03);

//             target[0] = 0.0;
//             CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, interpolation.linear(hyperCube, target), 1e-05);
// 
//             target[0] = 1.0;
//             CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, interpolation.linear(hyperCube, target), 1e-05);

        }
        
         void
        InterpolationTest::test3D()
        {
        }
    
    }}}
