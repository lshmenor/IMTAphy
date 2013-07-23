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


#ifndef IMTAPHY_DETAIL_LOOKUPTABLE_HPP
#define IMTAPHY_DETAIL_LOOKUPTABLE_HPP

#include <vector>
#include <WNS/Assure.hpp>
// For opt versions disable range checking in the multi_array wrapper
// WNS_NDEBUG means: assures are disabled
#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>

namespace imtaphy { namespace detail {

        template <typename KeyType, typename ValueType>
        struct KeysValuePair {
            std::vector<KeyType> keys;
            ValueType value;
        };
        
        template <typename KeyType>
        struct KeyPair {
            KeyType smaller;
            KeyType bigger;
        };
        
        struct IndexPair {
            int smaller;
            int bigger;
        };
        
        enum Corners 
        {
            Lower = 0,
            Upper = 1
        };
    
        template <typename HyperCubeType, int D>
        boost::array<typename HyperCubeType::index, D> getResultShape()
        {
            boost::array<typename HyperCubeType::index, D> shape;
            for (int i = 0; i < D; i++)
                shape[i] = 2;
            
            return shape;
        };
    
        template <typename KeyType=float, typename ValueType=float, int D=3>
        class LookupTable
        {
            typedef boost::multi_array<ValueType, D> ValueArrayType;
            typedef boost::multi_array_ref<bool, D> InitializedArrayType;
            
            typedef boost::array<unsigned int, D> DdimPositionType;
            
        public:
            typedef boost::multi_array<KeysValuePair<KeyType, ValueType>, D> HyperCubeType;
            typedef KeysValuePair<KeyType, ValueType> NearestNeighbourType;
            
            LookupTable(std::vector<unsigned int> dimensions,
                        std::vector<std::vector<KeyType> > keys_,
                        bool checkInitialized_) :
            keys(keys_),
            values(dimensions),
            inits(NULL),
            checkInitialized(checkInitialized_)
            {
                
                if (checkInitialized)
                {
                    int size = 1;
                    for (unsigned int i = 0; i < dimensions.size(); i++)
                        size *= dimensions[i];
                    
                    initVector = static_cast<bool*>(malloc(size * sizeof(int)));
                    
                    assure(initVector, "Could not allocate temporary vector");
                    
                    for (int i = 0; i < size; i++)
                        initVector[i] = false;
                    inits = new InitializedArrayType(initVector, dimensions);
                }
      
                assure(dimensions.size() == D, "Must give exactly as many values as dimensionality");
                assure(keys.size() == D, "Need to provide as many key vectors as dimensions");
                for (int i = 0; i < D; i++)
                    assure(keys[i].size() == dimensions[i], "Mismatch in dimension " << i << ": wrong number of keys for that dimension");

            };
            
            void fill(std::vector<unsigned int> position, ValueType value)
            {
                assure(position.size() == D, "Dimension mismatch");
            
                values(position) = value;
                if (checkInitialized)
                    (*inits)(position) = true;
            };
            
    
            ~LookupTable()
            {
                free(initVector);
            };
        
            
            HyperCubeType getHyperCube(const std::vector<KeyType> lookForKeys) const
            {
                assure(lookForKeys.size() == D, "Dimension mismatch");

                // the return value of this method is a D-dimensional hyper cube
                // we allocate the result object by setting up its "shape" telling
                // the boost::multi_array that it has D dimensions with an "extent" 
                // of 2 each to hold the lower and upper KeyValuePairs
               
                boost::array<typename HyperCubeType::index, D> shape = getResultShape<HyperCubeType, D>();
                HyperCubeType result(shape);
                
                // find the indices that describe the positions of the 
                // hypercubes's corners
                boost::array<IndexPair, D> indexPairs;
                for (int i = 0; i < D; i++)
                    indexPairs[i] = findIndexPair(lookForKeys[i], i);
                
                // create all combinations and fetch keys and values
                // this could nicely be done by a recursive algorithm but 
                // we don't know the recursion depth in advance
                std::vector<unsigned int> counters(D + 1, 0);
               
                DdimPositionType valuePosition;
                DdimPositionType cubePosition;
                
                // the extra element counters[D] is a sentinal that indicates the end of 
                // iteration
                while (!counters[D])
                {
                    std::vector<KeyType> resultKeys(D);
                    
                    for (int j = 0; j < D; j++)
                    {
                        cubePosition[j] = counters[j];
                        
                        switch (cubePosition[j])
                        {
                            case Lower:
                                valuePosition[j] = indexPairs[j].smaller;
                                resultKeys[j] = keys[j][indexPairs[j].smaller];
                                break;
                            case Upper:
                                valuePosition[j] = indexPairs[j].bigger;
                                resultKeys[j] = keys[j][indexPairs[j].bigger];
                                break;
                            default:
                                assure(0, "there should only be lower or upper and nothing else");
                        }
                    }
                    
                    if (checkInitialized)
                    {
                        assure((*inits)(valuePosition), "Accessing uninitialized value");
                    }
                    
                    result(cubePosition).value = values(valuePosition);
                    result(cubePosition).keys = resultKeys;
                    
                    // starting with first dimension, propagate the carry
                    // in case the counters have reached that dimension's size
                    // if they haven't, increment just that dimension's counter
                    // when all 0..D-1 dimensions carry over (all combinations done),
                    // incrementing counters[D] will terminate the iteration
                    int i;
                    for (i = 0; (i < D) && (counters[i] == shape[i] - 1); i++)
                        counters[i] = 0;
                    counters[i]++;
                }
                return result;
            }
            
            NearestNeighbourType getNearestNeighbour(const std::vector<KeyType> lookForKeys)
            {
//                HyperCubeType result(getResultShape<unsigned int, D>());
                HyperCubeType result(getHyperCube(lookForKeys));

                // check all result hyper cube's dimensions to see whether that 
                // dimension's key to look for is closer to the lower or upper 
                // corner
                
                std::vector<unsigned int> nearestNeighbourPosition(D);
                
                std::vector<unsigned int> position(D, 0);                
                for (unsigned int d = 0; d < D; d++)
                {
                    position[d] = 1;
                    KeyType upperDistance = result(position).keys[d] - lookForKeys[d];
                    position[d] = 0;
                    KeyType lowerDistance = lookForKeys[d] - result(position).keys[d];
                    
                    if (upperDistance < lowerDistance)
                        nearestNeighbourPosition[d] = Upper;
                    else
                        nearestNeighbourPosition[d] = Lower;
                }
                
                return result(nearestNeighbourPosition);
            }
            
            
        private:
            std::vector<std::vector<KeyType> > keys;
            ValueArrayType values;
            bool *initVector;
            InitializedArrayType *inits;
            bool checkInitialized;
            
            IndexPair findIndexPair(const KeyType x, const int dimension) const
            {
                assure(dimension < D, "Dimension out of bounds");
                assure(dimension >= 0, "Dimension must be positive");
                
                int size = keys[dimension].size();
                
                assure(size > 0, "Key vector cannot be empty");
                
                // TODO: we might want to throw an exception or handle this differently
                // If desired key x is smaller than smallest entry, return 
                // smallest entry as if it was an exact match. Do the same thing if
                // the keys vector only consists of one entry
                if ((x <= keys[dimension][0]) || (size == 1))
                {
                    IndexPair result;
                    result.smaller = 0;
                    result.bigger = 0;
                    
                    return result;
                }

                // TODO: we might want to throw an exception or handle this differently
                // If desired key x is bigger than biggest entry, return 
                // biggest entry as if it was an exact match
                if (x >= keys[dimension][size - 1])
                {
                    IndexPair result;
                    result.smaller = size - 1;
                    result.bigger = size - 1;
                    
                    return result;
                }

                // simple binary search for the indices of the keys that are immediately smaller 
                // and bigger than searched for x.
                
                assure(size > 1, "Size must be bigger than 1, we ruled out size==1 before");
                assure(keys[dimension][0] < keys[dimension][size - 1], "keys must be in ascending order - not even the first arg is smaller than the last");

                int smallerIndex = 0;
                int biggerIndex = size - 1;

                do
                {
                    int mid = (biggerIndex + smallerIndex) / 2; // using ints, this is "div" in C

                    // if - by coincidence - we have an exact match
                    if (keys[dimension][mid] == x)
                    {
                        IndexPair result;
                        result.smaller = mid;
                        result.bigger = mid;
                        
                        return result;
                    }
                    
                    // we have narrowed it down to the immediately smaller and bigger keys
                    if (biggerIndex == (smallerIndex + 1))
                    {
                        IndexPair result;
                        result.smaller = smallerIndex;
                        result.bigger = biggerIndex;
                        
                        return result;
                    }

                    if (x > keys[dimension][mid])
                        smallerIndex = mid;
                    else
                        biggerIndex = mid;

                } while (smallerIndex < biggerIndex);

                assure(0, "We should never reach this point - check the logic");
                IndexPair result = {0, 0};
                return result;
            }
        };

    } // detail
} // imtaphy

#endif 
