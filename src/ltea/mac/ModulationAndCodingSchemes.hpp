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

#ifndef LTEA_MAC_MODULATIONANDCODINGSCHEME_HPP
#define LTEA_MAC_MODULATIONANDCODINGSCHEME_HPP

#include <map>
#include <IMTAPHY/detail/LookupTable.hpp>
#include <IMTAPHY/link2System/Modulations.hpp>
#include <WNS/Singleton.hpp>
#include <WNS/PowerRatio.hpp>
#include <IMTAPHY/link2System/BlockErrorModel.hpp>
#include <boost/functional/hash.hpp>


namespace ltea { namespace mac {

    
    class ModulationAndCodingSchemes
    {
        public:
            ModulationAndCodingSchemes();
            
            unsigned int getSize(unsigned int mcsIndex, unsigned int numPRBs, unsigned int numLayers);
            
            imtaphy::l2s::ModulationScheme getModulation(unsigned int mcsIndex);

            unsigned int getNumAvailableDownlinkResourceElements(unsigned int pdcchLength,
                                                                 unsigned int numRel8AntennaPorts,
                                                                 unsigned int numPRBs,
                                                                 unsigned int numLayers,
                                                                 unsigned int numSixCenterPRBs,
                                                                 unsigned int tti,
                                                                 bool provideRel10DMRS,
                                                                 unsigned int numRel10CSIrsPorts,
                                                                 unsigned int numRel10CSIrsSets // muted and unmuted, for this TTI
                                                                 );
            
            unsigned int getNumAvailableUplinkResourceElements(unsigned int numPRBs,
                                                               unsigned int numLayers,
                                                               unsigned int tti,
                                                               bool hasSRS);

            
            double getEffectiveCodeRate(unsigned int tbSize, unsigned int availableBits);
    
            double getDownlinkCodeRate( unsigned int mcsIndex,
                                        unsigned int pdcchLength,
                                        unsigned int numRel8AntennaPorts,
                                        unsigned int numPRBs,
                                        unsigned int numLayers,
                                        unsigned int numSixCenterPRBs,
                                        unsigned int tti,
                                        bool provideRel10DMRS,
                                        unsigned int numRel10CSIrsPorts,
                                        unsigned int numRel10CSIrsSets // muted and unmuted, for this TTI
                                        );
 
            double getUplinkCodeRate(unsigned int mcsIndex,
                                     unsigned int numPRBs,
                                     unsigned int numLayers,
                                     unsigned int tti,
                                     bool hasSRS);
                                     
            
//             unsigned int getSuitableMCSindexForCodeRate(double desiredCodeRate,
//                                                        imtaphy::l2s::ModulationScheme modulation,
//                                                        unsigned int numPRBs,
//                                                        unsigned int numLayers,
//                                                        unsigned int pdcchLength,
//                                                        unsigned int numRel8AntennaPorts,
//                                                        unsigned int numSixCenterPRBs,
//                                                        unsigned int tti,
//                                                        bool provideRel10DMRS,
//                                                        unsigned int numRel10CSIrsPorts,
//                                                        unsigned int numRel10CSIrsSets // muted and unmuted, for this TTI
//                                                       );
                                                      
            unsigned int getSuitableDownlinkMCSindexForEffSINR(wns::Ratio sinr, 
                                                               unsigned int numPRBs, 
                                                               unsigned int numLayers, 
                                                               unsigned int pdcchLength, 
                                                               unsigned int numRel8AntennaPorts, 
                                                               unsigned int numSixCenterPRBs, 
                                                               unsigned int tti, 
                                                               bool provideRel10DMRS, 
                                                               unsigned int numRel10CSIrsPorts, 
                                                               unsigned int numRel10CSIrsSets);
                                                            
                                                         

            unsigned int getSuitableUplinkMCSindexForEffSINR(wns::Ratio sinr, 
                                                             unsigned int numPRBs, 
                                                             unsigned int numLayers, 
                                                             unsigned int tti, 
                                                             bool hasSRS,
                                                             const wns::Ratio& maxDeltaTFdB,
                                                             double Ks);
            wns::Ratio computeDeltaTFdB(unsigned int mcsIndex, double codeRate, double Ks);
                                                                
                                                                            
        private:
            void
            computeThresholds(unsigned int hash,
                                unsigned int numPRBs, 
                                unsigned int numLayers, 
                                unsigned int tti, 
                                bool hasSRS);

            typedef std::map<unsigned int, unsigned int> TranslationTable;
            
            TranslationTable oneToTwoLayersTranslationTable;
            TranslationTable oneToThreeLayersTranslationTable;
            TranslationTable oneToFourLayersTranslationTable;
            
            imtaphy::l2s::BlockErrorModel* blerModel;
            std::map<unsigned int, std::map<wns::Ratio, unsigned int> > lut;
    };
    typedef wns::SingletonHolder<ModulationAndCodingSchemes> TheMCSLookup;
}}

#endif // LTEA_MAC_TRANSPORTBLOCKSIZE_HPP
