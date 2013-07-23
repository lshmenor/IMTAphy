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

#ifndef IMTAPHY_L2S_LTECQIS_HPP
#define IMTAPHY_L2S_LTECQIS_HPP

#include <IMTAPHY/link2System/Modulations.hpp>
#include <IMTAPHY/detail/LookupTable.hpp>

#include <WNS/PowerRatio.hpp>
#include <algorithm>

namespace imtaphy { namespace l2s {

    typedef unsigned int CQI;

    struct CQIStruct {
        imtaphy::l2s::ModulationScheme modulation;
        double codeRate;
        unsigned int id;
        double efficiency;
    };



    static CQIStruct CQITable[] = {
        {imtaphy::l2s::BPSK(), 0.0,             0, 0.}, // CQI 0 //
        {imtaphy::l2s::QPSK(), 78.0 / 1024.,    1, 0.1523}, // CQI 1
        {imtaphy::l2s::QPSK(), 120.0 / 1024.,   2, 0.2344}, // CQI 2
        {imtaphy::l2s::QPSK(), 193.0 / 1024.,   3, 0.377}, // CQI 3
        {imtaphy::l2s::QPSK(), 308.0 / 1024.,   4, 0.6016}, // CQI 4
        {imtaphy::l2s::QPSK(), 449.0 / 1024.,   5, 0.877}, // CQI 5
        {imtaphy::l2s::QPSK(), 602.0 / 1024.,   6, 1.1758}, // CQI 6
        {imtaphy::l2s::QAM16(), 378.0 / 1024.,  7, 1.4766}, // CQI 7
        {imtaphy::l2s::QAM16(), 490.0 / 1024.,  8, 1.9141}, // CQI 8
        {imtaphy::l2s::QAM16(), 616.0 / 1024.,  9, 2.4063}, // CQI 9
        {imtaphy::l2s::QAM64(), 466.0 / 1024., 10, 2.7305}, // CQI 10
        {imtaphy::l2s::QAM64(), 567.0 / 1024., 11, 3.3223}, // CQI 11
        {imtaphy::l2s::QAM64(), 666.0 / 1024., 12, 3.9023}, // CQI 12
        {imtaphy::l2s::QAM64(), 772.0 / 1024., 13, 4.5234}, // CQI 13
        {imtaphy::l2s::QAM64(), 873.0 / 1024., 14, 5.1152}, // CQI 14
        {imtaphy::l2s::QAM64(), 948.0 / 1024., 15, 5.5547} // CQI 15
    };

}}

#endif 


