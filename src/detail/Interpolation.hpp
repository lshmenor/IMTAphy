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

#ifndef IMTAPHY_DETAIL_INTERPOLATION_HPP
#define IMTAPHY_DETAIL_INTERPOLATION_HPP

#include <IMTAPHY/detail/LookupTable.hpp>

namespace imtaphy { namespace detail {

    // this is the general case for D > 1
    template <typename KeyType, typename ValueType, int D>
    class Interpolation
    {
        public:
            Interpolation() {};

            ValueType 
            linear(const typename LookupTable<KeyType, ValueType, D>::HyperCubeType hyperCube, const std::vector<KeyType> target) const
            {
                assure(target.size() == D, "Wrong target dimension");
                
                // create D-1 hypercube by linear interpolation along the last dimension
                
                typename LookupTable<KeyType, ValueType, D-1>::HyperCubeType oneDimensionLess(getResultShape<typename LookupTable<KeyType, ValueType, D-1>::HyperCubeType, D-1>());
                std::vector<KeyType> targetOneDimensionLess(D-1);
                for (int j = 0; j < D-1; j++)
                    targetOneDimensionLess[j] = target[j];
                
                boost::array<typename LookupTable<KeyType, ValueType, D>::HyperCubeType::index, D> shape = getResultShape<typename LookupTable<KeyType, ValueType, D>::HyperCubeType, D>();
                
                std::vector<unsigned int> counters(D, 0);
                std::vector<unsigned int> lowerCubePosition(D, 0);
                std::vector<unsigned int> upperCubePosition(D, 0);
                std::vector<unsigned int> resultPosition(D-1, 0);
            
                
                while (!counters[D-1])
                {
                    std::vector<KeyType> oneDimensionLessKeys(D-1);
                    
                    for (int j = 0; j < D-1; j++)
                    {
                        resultPosition[j] = counters[j];
                        lowerCubePosition[j] = upperCubePosition[j] = counters[j];
                    }
                    
                    lowerCubePosition[D-1] = Lower;
                    upperCubePosition[D-1] = Upper;
                    
                    oneDimensionLess(resultPosition).value = linear(hyperCube(lowerCubePosition),
                                                                    hyperCube(upperCubePosition),
                                                                    target[D-1]);

                    for (int j = 0; j < D-1; j++)
                        oneDimensionLessKeys[j] = hyperCube(lowerCubePosition).keys[j];
                    oneDimensionLess(resultPosition).keys = oneDimensionLessKeys;

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
    
                Interpolation<KeyType, ValueType, D-1>  interpolation; 
                return interpolation.linear(oneDimensionLess, targetOneDimensionLess);
            };
        
            
        private:
            ValueType
            linear(const KeysValuePair<KeyType, ValueType> lower, const KeysValuePair<KeyType, ValueType> upper, const KeyType target) const
            {
                KeyType lowerKey = lower.keys[lower.keys.size() - 1];
                KeyType upperKey = upper.keys[upper.keys.size() - 1];
                
                assure(lowerKey <= target, "Target coordinate below lower corner: cannot extrapolate");
                assure(upperKey >= target, "Target coordinate above upper corner: cannot extrapolate");
                
                // this can happen when the LookupTable has an exact hit in one dimensions
                // the resulting hyperCube will then have identical upper/lower key/value pairs
                // Is upper.value != lower.value possible at all?
                if (upperKey == lowerKey)
                    return (upper.value + lower.value) / 2.0;
                
                // take last dimension        
                KeyType normalized = (target - lowerKey) / (upperKey - lowerKey);
                return (normalized * upper.value + (static_cast<KeyType>(1.0) - normalized) * lower.value);
            };
    };
    
    // this is the specialised case for D==1 -> end of recursion
    template <typename KeyType, typename ValueType>
    class Interpolation<KeyType, ValueType, 1>
    {
        public:
            Interpolation() {};

            ValueType 
            linear(const typename LookupTable<KeyType, ValueType, 1>::HyperCubeType hyperCube, const std::vector<KeyType> target) const
            {   // only one dimension left
                return linear(hyperCube[Lower], hyperCube[Upper], target[0]);
            };
        private:
            ValueType
            linear(const KeysValuePair<KeyType, ValueType> lower, const KeysValuePair<KeyType, ValueType> upper, KeyType target) const
            {
                KeyType lowerKey = lower.keys[0];
                KeyType upperKey = upper.keys[0];
                
                assure(lowerKey <= target, "Target coordinate below lower corner: cannot extrapolate");
                assure(upperKey >= target, "Target coordinate above upper corner: cannot extrapolate");
                
                // this can happen when the LookupTable has an exact hit in one dimensions
                // the resulting hyperCube will then have identical upper/lower key/value pairs
                // Is upper.value != lower.value possible at all?
                if (upperKey == lowerKey)
                    return (upper.value + lower.value) / 2.0;
                
                // take last dimension        
                KeyType normalized = (target - lowerKey) / (upperKey - lowerKey);
                return (normalized * upper.value + (static_cast<KeyType>(1.0) - normalized) * lower.value);
            };
    };

}}

#endif