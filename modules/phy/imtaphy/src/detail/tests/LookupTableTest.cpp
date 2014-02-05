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

namespace imtaphy { namespace detail { namespace tests {
        class LookupTableTest :
            public CppUnit::TestFixture
        {
            CPPUNIT_TEST_SUITE( LookupTableTest );
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

        CPPUNIT_TEST_SUITE_REGISTRATION( LookupTableTest );
    
        void
        LookupTableTest::setUp()
        {
        }

        void
        LookupTableTest::tearDown()
        {
        }
    
        void
        LookupTableTest::test1D()
        {
            typedef LookupTable<float, float, 1> FloatFloat1LutType;
            
            unsigned int shape_[1] = {4};
            float keys1_[4] = {-1.0, 0.0, 1.0, 2.0};
            std::vector<unsigned int> shape(shape_, shape_+1);
            
            std::vector<float> keysDim1(keys1_, keys1_ + 4);

            std::vector<std::vector<float> > keys(1);
            keys[0] = keysDim1;
            
            FloatFloat1LutType lut(shape, keys, true);
            
            std::vector<unsigned int> position(1);
            
            position[0] = 0; lut.fill(position, -1); // (-1 -> -1)
            position[0] = 1; lut.fill(position, 0); // (0 -> 0)
            position[0] = 2; lut.fill(position, 1); // (1 -> 1)
            position[0] = 3; lut.fill(position, 2); // (2 -> 2)

            FloatFloat1LutType::HyperCubeType result(getResultShape<FloatFloat1LutType::HyperCubeType, 1>());
            
            std::vector<float> lookForKeys(1);
            
            // if key to look for is smaller than smallest key, the 
            // pair should contain the smallest value for upper and lower
            lookForKeys[0] = -1.5;
            result = lut.getHyperCube(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Upper].keys[0], 1e-05);

            KeysValuePair<float, float> nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, nearest.value, 1e-05);
            

            // if key to look for is bigger than biggest key, the 
            // pair should contain the biggest key's value for upper and lower
            lookForKeys[0] = 2.5;
            result = lut.getHyperCube(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Upper].keys[0], 1e-05);

            nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, nearest.value, 1e-05);

                    
            // if key to look for is an exact match, the 
            // pair should contain the exact match value for upper and lower
            lookForKeys[0] = -1.0;
            result = lut.getHyperCube(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Upper].keys[0], 1e-05);

            nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, nearest.value, 1e-05);

            
            lookForKeys[0] = 0.0;
            result = lut.getHyperCube(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Upper].keys[0], 1e-05);

            nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, nearest.value, 1e-05);

            
            lookForKeys[0] = 1.0;
            result = lut.getHyperCube(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Upper].keys[0], 1e-05);

            nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, nearest.value, 1e-05);

            
            lookForKeys[0] = 2.0;
            result = lut.getHyperCube(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Upper].keys[0], 1e-05);

            nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, nearest.value, 1e-05);

            
            // when the key to look for lies between to keys, the smaller and
            // bigger key's values should be given
            
            lookForKeys[0] = -0.05;
            result = lut.getHyperCube(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Upper].keys[0], 1e-05);

            nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, nearest.value, 1e-05);

            
            lookForKeys[0] = 0.5;
            result = lut.getHyperCube(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Upper].keys[0], 1e-05);

            // it's only the upper if its truely smaller
            nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, nearest.value, 1e-05);

            
            lookForKeys[0] = 1.00001;
            result = lut.getHyperCube(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Upper].keys[0], 1e-05);
 
            nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, nearest.value, 1e-05);

            
        }
            
        void
        LookupTableTest::test2D()
        {
            typedef LookupTable<float, float, 2> FloatFloat2LutType;
            
            // without specifying template arguments, the standard arguments 
            // (float/float/3) are used
            
            unsigned int shape_[2] = {2, 4};
            float keys0_[4] = {-1, 1};
            float keys1_[4] = {0, 0.2, 0.4, 6};
            std::vector<unsigned int> shape(shape_, shape_+2);
            
            std::vector<float> keysDim0(keys0_, keys0_ + 2);
            std::vector<float> keysDim1(keys1_, keys1_ + 4);

            std::vector<std::vector<float> > keys(2);
            keys[0] = keysDim0;
            keys[1] = keysDim1;
            
            FloatFloat2LutType lut(shape, keys,true);
            
            // dim1
            // 6.0 | 4   8
            // 0.4 | 3   7
            // 0.2 | 2   6
            // 0.0 | 1   5
            //     +-------
            //      -1   1 dim0
            std::vector<unsigned int> position(2);
            position[0] = 0; position[1] = 0; lut.fill(position, 1.0); 
            position[0] = 0; position[1] = 1; lut.fill(position, 2.0); 
            position[0] = 0; position[1] = 2; lut.fill(position, 3.0); 
            position[0] = 0; position[1] = 3; lut.fill(position, 4.0); 
            position[0] = 1; position[1] = 0; lut.fill(position, 5.0); 
            position[0] = 1; position[1] = 1; lut.fill(position, 6.0); 
            position[0] = 1; position[1] = 2; lut.fill(position, 7.0); 
            position[0] = 1; position[1] = 3; lut.fill(position, 8.0); 
            
            FloatFloat2LutType::HyperCubeType result(getResultShape<FloatFloat2LutType::HyperCubeType, 2>());
            
            std::vector<float> lookForKeys(2);
            lookForKeys[0] = 0.5;
            lookForKeys[1] = 0.15; 
            result = lut.getHyperCube(lookForKeys);
            // should be the 1 2 6 5 square at -1 to 1 and 0.0 to 0.2
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Lower][Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, result[Lower][Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, result[Upper][Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, result[Upper][Upper].value, 1e-05);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Lower][Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Lower][Upper].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Upper][Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Upper][Upper].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Lower][Lower].keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2, result[Lower][Upper].keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Upper][Lower].keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2, result[Upper][Upper].keys[1], 1e-05);
            
            KeysValuePair<float, float> nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2, nearest.keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, nearest.value, 1e-05);

            
            // next one
            lookForKeys[0] = 0.0;
            lookForKeys[1] = 0.0; 
            result = lut.getHyperCube(lookForKeys);
            // should be the 1 5 "line" at -1 to 1 and 0.0 to 0.0
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Lower][Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Lower][Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, result[Upper][Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, result[Upper][Upper].value, 1e-05);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Lower][Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, result[Lower][Upper].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Upper][Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Upper][Upper].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Lower][Lower].keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Lower][Upper].keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Upper][Lower].keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, result[Upper][Upper].keys[1], 1e-05);

            nearest = lut.getNearestNeighbour(lookForKeys);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, nearest.keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, nearest.keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, nearest.value, 1e-05);

            
            // and another one
            lookForKeys[0] = 10.0;
            lookForKeys[1] = 10.0; 
            result = lut.getHyperCube(lookForKeys);
            // should be the "point" 8 at 1 to 1 and 6.0 to 6.0
            CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, result[Lower][Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, result[Lower][Upper].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, result[Upper][Lower].value, 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, result[Upper][Upper].value, 1e-05);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Lower][Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Lower][Upper].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Upper][Lower].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, result[Upper][Upper].keys[0], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, result[Lower][Lower].keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, result[Lower][Upper].keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, result[Upper][Lower].keys[1], 1e-05);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, result[Upper][Upper].keys[1], 1e-05);

        }
        
         void
        LookupTableTest::test3D()
        {
            typedef LookupTable<int, int, 3> IntInt3LutType;
            
            
            unsigned int shape_[3] = {2, 2, 2};
            int keys0_[2] = {0, 2};
            int keys1_[2] = {0, 2};
            int keys2_[2] = {0, 2};

            std::vector<unsigned int> shape(shape_, shape_+3);
            
            std::vector<int> keysDim0(keys0_, keys0_ + 2);
            std::vector<int> keysDim1(keys1_, keys1_ + 2);
            std::vector<int> keysDim2(keys2_, keys2_ + 2);

            std::vector<std::vector<int> > keys(3);
            keys[0] = keysDim0;
            keys[1] = keysDim1;
            keys[2] = keysDim2;
            
            IntInt3LutType lut(shape, keys,true);
            
            // First test: lower corner
            
            // this is a cube with corners at 0 and 2 filled with values 1..8
            std::vector<unsigned int> position(3);
            position[0] = 0; position[1] = 0; position[2] = 0; lut.fill(position, 1); 
            position[0] = 0; position[1] = 0; position[2] = 1; lut.fill(position, 2); 
            position[0] = 0; position[1] = 1; position[2] = 0; lut.fill(position, 3); 
            position[0] = 0; position[1] = 1; position[2] = 1; lut.fill(position, 4); 
            position[0] = 1; position[1] = 0; position[2] = 0; lut.fill(position, 5); 
            position[0] = 1; position[1] = 0; position[2] = 1; lut.fill(position, 6); 
            position[0] = 1; position[1] = 1; position[2] = 0; lut.fill(position, 7); 
            position[0] = 1; position[1] = 1; position[2] = 1; lut.fill(position, 8); 

            IntInt3LutType::HyperCubeType result(getResultShape<IntInt3LutType::HyperCubeType, 3>());
            
            std::vector<int> lookForKeys(3);
            lookForKeys[0] = 0;
            lookForKeys[1] = 0;
            lookForKeys[2] = 0;

            result = lut.getHyperCube(lookForKeys);
            // that's the lowest corner -> value 1
            CPPUNIT_ASSERT_EQUAL(1, result[Lower][Lower][Lower].value);
            CPPUNIT_ASSERT_EQUAL(1, result[Lower][Lower][Upper].value);
            CPPUNIT_ASSERT_EQUAL(1, result[Lower][Upper][Lower].value);
            CPPUNIT_ASSERT_EQUAL(1, result[Lower][Upper][Upper].value);
            CPPUNIT_ASSERT_EQUAL(1, result[Upper][Lower][Lower].value);
            CPPUNIT_ASSERT_EQUAL(1, result[Upper][Lower][Upper].value);
            CPPUNIT_ASSERT_EQUAL(1, result[Upper][Upper][Lower].value);
            CPPUNIT_ASSERT_EQUAL(1, result[Upper][Upper][Upper].value);
            
            // it should all be the (0,0,0) coordinates
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Lower].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Lower].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Lower].keys[2]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Upper].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Upper].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Upper].keys[2]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Upper][Lower].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Upper][Lower].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Upper][Lower].keys[2]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Upper][Upper].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Upper][Upper].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Upper][Upper].keys[2]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Lower][Lower].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Lower][Lower].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Lower][Lower].keys[2]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Lower][Upper].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Lower][Upper].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Lower][Upper].keys[2]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Upper][Lower].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Upper][Lower].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Upper][Lower].keys[2]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Upper][Upper].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Upper][Upper].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Upper][Upper].keys[2]);

            // Second test: right in the middle
            lookForKeys[0] = 1;
            lookForKeys[1] = 1;
            lookForKeys[2] = 1;

            result = lut.getHyperCube(lookForKeys);
            // right in the middle, all corners should be there
            CPPUNIT_ASSERT_EQUAL(1, result[Lower][Lower][Lower].value);
            CPPUNIT_ASSERT_EQUAL(2, result[Lower][Lower][Upper].value);
            CPPUNIT_ASSERT_EQUAL(3, result[Lower][Upper][Lower].value);
            CPPUNIT_ASSERT_EQUAL(4, result[Lower][Upper][Upper].value);
            CPPUNIT_ASSERT_EQUAL(5, result[Upper][Lower][Lower].value);
            CPPUNIT_ASSERT_EQUAL(6, result[Upper][Lower][Upper].value);
            CPPUNIT_ASSERT_EQUAL(7, result[Upper][Upper][Lower].value);
            CPPUNIT_ASSERT_EQUAL(8, result[Upper][Upper][Upper].value);
            
            // it should be all 8 coordinates
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Lower].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Lower].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Lower].keys[2]);
            
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Upper].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Lower][Upper].keys[1]);
            CPPUNIT_ASSERT_EQUAL(2, result[Lower][Lower][Upper].keys[2]);
            
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Upper][Lower].keys[0]);
            CPPUNIT_ASSERT_EQUAL(2, result[Lower][Upper][Lower].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Upper][Lower].keys[2]);
            
            CPPUNIT_ASSERT_EQUAL(0, result[Lower][Upper][Upper].keys[0]);
            CPPUNIT_ASSERT_EQUAL(2, result[Lower][Upper][Upper].keys[1]);
            CPPUNIT_ASSERT_EQUAL(2, result[Lower][Upper][Upper].keys[2]);
            
            CPPUNIT_ASSERT_EQUAL(2, result[Upper][Lower][Lower].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Lower][Lower].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Lower][Lower].keys[2]);
            
            CPPUNIT_ASSERT_EQUAL(2, result[Upper][Lower][Upper].keys[0]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Lower][Upper].keys[1]);
            CPPUNIT_ASSERT_EQUAL(2, result[Upper][Lower][Upper].keys[2]);
            
            CPPUNIT_ASSERT_EQUAL(2, result[Upper][Upper][Lower].keys[0]);
            CPPUNIT_ASSERT_EQUAL(2, result[Upper][Upper][Lower].keys[1]);
            CPPUNIT_ASSERT_EQUAL(0, result[Upper][Upper][Lower].keys[2]);
            
            CPPUNIT_ASSERT_EQUAL(2, result[Upper][Upper][Upper].keys[0]);
            CPPUNIT_ASSERT_EQUAL(2, result[Upper][Upper][Upper].keys[1]);
            CPPUNIT_ASSERT_EQUAL(2, result[Upper][Upper][Upper].keys[2]);
        }
    
    }}}
