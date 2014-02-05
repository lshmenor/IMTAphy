/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2010-11
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

#ifndef IMTAPHYL2S_EFFECTIVESINRMODELINTERFACE_HPP
#define IMTAPHYL2S_EFFECTIVESINRMODELINTERFACE_HPP

#include <string>

namespace imtaphy { namespace l2s {

        typedef unsigned int Bits;
    
        class ModulationScheme 
        {
            public:
                ModulationScheme();
                
                Bits getBitsPerSymbol() const {return bitsPerSymbol;};
                std::string getName() const {return name;};
            protected:
                Bits bitsPerSymbol;
                std::string name;
        };
        
        class Generic :
            public ModulationScheme
        {
        public: 
            Generic() {
                bitsPerSymbol = 0;
                name = "Generic"; 
            }
        };
            
        class BPSK :
            public imtaphy::l2s::ModulationScheme
        {
            public:
                BPSK();
        };

       
        class QPSK :
            public imtaphy::l2s::ModulationScheme
        {
            public:
                QPSK();
        };
        

        class QAM8 :
            public imtaphy::l2s::ModulationScheme
        {
            public:
                QAM8();
        };

        class QAM16 :
            public imtaphy::l2s::ModulationScheme
        {
            public:
                QAM16();
        };

        class QAM32 :
            public imtaphy::l2s::ModulationScheme
        {
            public:
                QAM32();
        };

        class QAM64 :
            public imtaphy::l2s::ModulationScheme
        {
            public:
                QAM64();
        };

        class QAM128 :
            public imtaphy::l2s::ModulationScheme
        {
            public:
                QAM128();
        };

        class QAM256 :
            public imtaphy::l2s::ModulationScheme
        {
            public:
                QAM256();
        };

        
}}
#endif
