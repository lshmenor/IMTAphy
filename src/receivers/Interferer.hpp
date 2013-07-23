/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
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

#ifndef IMTAPHY_RECEIVERS_INTERFERER_HPP
#define IMTAPHY_RECEIVERS_INTERFERER_HPP

#include <map>
#include <set>
#include <WNS/PowerRatio.hpp>
#include <IMTAPHY/Transmission.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <boost/functional/hash.hpp>

namespace imtaphy {

    namespace receivers {
        
        class Interferer
        {
        public:
            Interferer(){};
            Interferer(const Interferer& copy) :
                interferingTransmission(copy.interferingTransmission),
                interferingLink(copy.interferingLink)
            {
            };
           
            imtaphy::TransmissionPtr interferingTransmission;
            imtaphy::Link* interferingLink;
        };
                
        class InterferersCompare
        {
        public:
            bool operator()(const Interferer& a, const Interferer& b) const
            {
                return a.interferingTransmission->getId() < b.interferingTransmission->getId();
            }
        };

        typedef std::set<Interferer, InterferersCompare> InterferersSet;
        
        
        class InterferersCollection
        {
        public:
            InterferersCollection() :
                hash(static_cast<std::size_t>(0)),
                sealed(false)
            {}
            
            bool operator==(const InterferersCollection& rhs)
            {
                return (getHash() == rhs.getHash());
            }

            bool operator!=(const InterferersCollection& rhs)
            {
                return (getHash() != rhs.getHash());
            }

            
            std::size_t getHash() const 
            {
                assure(sealed, "Tried to access hash value before interferers collection was sealed");
                return hash;
            }
            
            void seal()
            {
                assure(sealed == false, "Interferers collection already sealed");
                
                // iterate over transmission pointers in well-defined order and combine to hash value
                for (InterferersSet::const_iterator iter = interferers.begin();
                     iter != interferers.end(); iter++)
                {
                    boost::hash_combine(hash, iter->interferingTransmission.get());
                }
                
                sealed = true;
            }
            
            void insert(const Interferer& interferer)
            {
                interferers.insert(interferer);
            }
            
            InterferersSet& getInterferersSet() 
            {
                assure(sealed, "Tried to access before sealed");
                return interferers;
            }

        private:
            std::size_t hash;
            bool sealed;
            InterferersSet interferers;
        };

        typedef boost::shared_ptr<InterferersCollection> InterferersCollectionPtr;
       
        
        // helper to compare InterferersCollectionPtrs by comparing the underlying hash values
        class InterferersCollectionCompare
        {
        public:
            bool operator()(const InterferersCollectionPtr a, const InterferersCollectionPtr b) const
            {
                return a->getHash() < b->getHash();
            }
        };
        

    }}

#endif //
