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

#include <IMTAPHY/ltea/mac/ModulationAndCodingSchemes.hpp>

#include <WNS/Assure.hpp>
#include <algorithm>

using namespace ltea::mac;

namespace ltea { namespace mac {
    // 3GPP TS 36.213 Table 7.1.7.2.1-1: Transport block size table (dimension 27×110)
    static unsigned int tbSizes[27][110] = {
    { 16, 32, 56, 88, 120, 152, 176, 208, 224, 256, 288, 328, 344, 376, 392, 424, 456, 488, 504, 536, 568, 600, 616, 648, 680, 712, 744, 776, 776, 808, 840, 872, 904, 936, 968, 1000, 1032, 1032, 1064, 1096, 1128, 1160, 1192, 1224, 1256, 1256, 1288, 1320, 1352, 1384, 1416, 1416, 1480, 1480, 1544, 1544, 1608, 1608, 1608, 1672, 1672, 1736, 1736, 1800, 1800, 1800, 1864, 1864, 1928, 1928, 1992, 1992, 2024, 2088, 2088, 2088, 2152, 2152, 2216, 2216, 2280, 2280, 2280, 2344, 2344, 2408, 2408, 2472, 2472, 2536, 2536, 2536, 2600, 2600, 2664, 2664, 2728, 2728, 2728, 2792, 2792, 2856, 2856, 2856, 2984, 2984, 2984, 2984, 2984, 3112 },
    { 24, 56, 88, 144, 176, 208, 224, 256, 328, 344, 376, 424, 456, 488, 520, 568, 600, 632, 680, 712, 744, 776, 808, 872, 904, 936, 968, 1000, 1032, 1064, 1128, 1160, 1192, 1224, 1256, 1288, 1352, 1384, 1416, 1416, 1480, 1544, 1544, 1608, 1608, 1672, 1736, 1736, 1800, 1800, 1864, 1864, 1928, 1992, 1992, 2024, 2088, 2088, 2152, 2152, 2216, 2280, 2280, 2344, 2344, 2408, 2472, 2472, 2536, 2536, 2600, 2600, 2664, 2728, 2728, 2792, 2792, 2856, 2856, 2856, 2984, 2984, 2984, 3112, 3112, 3112, 3240, 3240, 3240, 3240, 3368, 3368, 3368, 3496, 3496, 3496, 3496, 3624, 3624, 3624, 3752, 3752, 3752, 3752, 3880, 3880, 3880, 4008, 4008, 4008 },
    { 32, 72, 144, 176, 208, 256, 296, 328, 376, 424, 472, 520, 568, 616, 648, 696, 744, 776, 840, 872, 936, 968, 1000, 1064, 1096, 1160, 1192, 1256, 1288, 1320, 1384, 1416, 1480, 1544, 1544, 1608, 1672, 1672, 1736, 1800, 1800, 1864, 1928, 1992, 2024, 2088, 2088, 2152, 2216, 2216, 2280, 2344, 2344, 2408, 2472, 2536, 2536, 2600, 2664, 2664, 2728, 2792, 2856, 2856, 2856, 2984, 2984, 3112, 3112, 3112, 3240, 3240, 3240, 3368, 3368, 3368, 3496, 3496, 3496, 3624, 3624, 3624, 3752, 3752, 3880, 3880, 3880, 4008, 4008, 4008, 4136, 4136, 4136, 4264, 4264, 4264, 4392, 4392, 4392, 4584, 4584, 4584, 4584, 4584, 4776, 4776, 4776, 4776, 4968, 4968 },
    { 40, 104, 176, 208, 256, 328, 392, 440, 504, 568, 616, 680, 744, 808, 872, 904, 968, 1032, 1096, 1160, 1224, 1256, 1320, 1384, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928, 1992, 2024, 2088, 2152, 2216, 2280, 2344, 2408, 2472, 2536, 2536, 2600, 2664, 2728, 2792, 2856, 2856, 2984, 2984, 3112, 3112, 3240, 3240, 3368, 3368, 3496, 3496, 3624, 3624, 3624, 3752, 3752, 3880, 3880, 4008, 4008, 4136, 4136, 4264, 4264, 4392, 4392, 4392, 4584, 4584, 4584, 4776, 4776, 4776, 4776, 4968, 4968, 4968, 5160, 5160, 5160, 5352, 5352, 5352, 5352, 5544, 5544, 5544, 5736, 5736, 5736, 5736, 5992, 5992, 5992, 5992, 6200, 6200, 6200, 6200, 6456, 6456 },
    { 56, 120, 208, 256, 328, 408, 488, 552, 632, 696, 776, 840, 904, 1000, 1064, 1128, 1192, 1288, 1352, 1416, 1480, 1544, 1608, 1736, 1800, 1864, 1928, 1992, 2088, 2152, 2216, 2280, 2344, 2408, 2472, 2600, 2664, 2728, 2792, 2856, 2984, 2984, 3112, 3112, 3240, 3240, 3368, 3496, 3496, 3624, 3624, 3752, 3752, 3880, 4008, 4008, 4136, 4136, 4264, 4264, 4392, 4392, 4584, 4584, 4584, 4776, 4776, 4968, 4968, 4968, 5160, 5160, 5160, 5352, 5352, 5544, 5544, 5544, 5736, 5736, 5736, 5992, 5992, 5992, 5992, 6200, 6200, 6200, 6456, 6456, 6456, 6456, 6712, 6712, 6712, 6968, 6968, 6968, 6968, 7224, 7224, 7224, 7480, 7480, 7480, 7480, 7736, 7736, 7736, 7992 },
    { 72, 144, 224, 328, 424, 504, 600, 680, 776, 872, 968, 1032, 1128, 1224, 1320, 1384, 1480, 1544, 1672, 1736, 1864, 1928, 2024, 2088, 2216, 2280, 2344, 2472, 2536, 2664, 2728, 2792, 2856, 2984, 3112, 3112, 3240, 3368, 3496, 3496, 3624, 3752, 3752, 3880, 4008, 4008, 4136, 4264, 4392, 4392, 4584, 4584, 4776, 4776, 4776, 4968, 4968, 5160, 5160, 5352, 5352, 5544, 5544, 5736, 5736, 5736, 5992, 5992, 5992, 6200, 6200, 6200, 6456, 6456, 6712, 6712, 6712, 6968, 6968, 6968, 7224, 7224, 7224, 7480, 7480, 7480, 7736, 7736, 7736, 7992, 7992, 7992, 8248, 8248, 8248, 8504, 8504, 8760, 8760, 8760, 8760, 9144, 9144, 9144, 9144, 9528, 9528, 9528, 9528, 9528 },
    // changed entry [0][6] for I_TBS=6, N_PRB=1 from 328 to 88 because 328 makes no sense at all
    { 88, 176, 256, 392, 504, 600, 712, 808, 936, 1032, 1128, 1224, 1352, 1480, 1544, 1672, 1736, 1864, 1992, 2088, 2216, 2280, 2408, 2472, 2600, 2728, 2792, 2984, 2984, 3112, 3240, 3368, 3496, 3496, 3624, 3752, 3880, 4008, 4136, 4136, 4264, 4392, 4584, 4584, 4776, 4776, 4968, 4968, 5160, 5160, 5352, 5352, 5544, 5736, 5736, 5992, 5992, 5992, 6200, 6200, 6456, 6456, 6456, 6712, 6712, 6968, 6968, 6968, 7224, 7224, 7480, 7480, 7736, 7736, 7736, 7992, 7992, 8248, 8248, 8248, 8504, 8504, 8760, 8760, 8760, 9144, 9144, 9144, 9144, 9528, 9528, 9528, 9528, 9912, 9912, 9912, 10296, 10296, 10296, 10296, 10680, 10680, 10680, 10680, 11064, 11064, 11064, 11448, 11448, 11448 },
    { 104, 224, 328, 472, 584, 712, 840, 968, 1096, 1224, 1320, 1480, 1608, 1672, 1800, 1928, 2088, 2216, 2344, 2472, 2536, 2664, 2792, 2984, 3112, 3240, 3368, 3368, 3496, 3624, 3752, 3880, 4008, 4136, 4264, 4392, 4584, 4584, 4776, 4968, 4968, 5160, 5352, 5352, 5544, 5736, 5736, 5992, 5992, 6200, 6200, 6456, 6456, 6712, 6712, 6712, 6968, 6968, 7224, 7224, 7480, 7480, 7736, 7736, 7992, 7992, 8248, 8248, 8504, 8504, 8760, 8760, 8760, 9144, 9144, 9144, 9528, 9528, 9528, 9912, 9912, 9912, 10296, 10296, 10296, 10680, 10680, 10680, 11064, 11064, 11064, 11448, 11448, 11448, 11448, 11832, 11832, 11832, 12216, 12216, 12216, 12576, 12576, 12576, 12960, 12960, 12960, 12960, 13536, 13536 },
    { 120, 256, 392, 536, 680, 808, 968, 1096, 1256, 1384, 1544, 1672, 1800, 1928, 2088, 2216, 2344, 2536, 2664, 2792, 2984, 3112, 3240, 3368, 3496, 3624, 3752, 3880, 4008, 4264, 4392, 4584, 4584, 4776, 4968, 4968, 5160, 5352, 5544, 5544, 5736, 5992, 5992, 6200, 6200, 6456, 6456, 6712, 6968, 6968, 7224, 7224, 7480, 7480, 7736, 7736, 7992, 7992, 8248, 8504, 8504, 8760, 8760, 9144, 9144, 9144, 9528, 9528, 9528, 9912, 9912, 9912, 10296, 10296, 10680, 10680, 10680, 11064, 11064, 11064, 11448, 11448, 11448, 11832, 11832, 12216, 12216, 12216, 12576, 12576, 12576, 12960, 12960, 12960, 13536, 13536, 13536, 13536, 14112, 14112, 14112, 14112, 14688, 14688, 14688, 14688, 15264, 15264, 15264, 15264 },
    { 136, 296, 456, 616, 776, 936, 1096, 1256, 1416, 1544, 1736, 1864, 2024, 2216, 2344, 2536, 2664, 2856, 2984, 3112, 3368, 3496, 3624, 3752, 4008, 4136, 4264, 4392, 4584, 4776, 4968, 5160, 5160, 5352, 5544, 5736, 5736, 5992, 6200, 6200, 6456, 6712, 6712, 6968, 6968, 7224, 7480, 7480, 7736, 7992, 7992, 8248, 8248, 8504, 8760, 8760, 9144, 9144, 9144, 9528, 9528, 9912, 9912, 10296, 10296, 10296, 10680, 10680, 11064, 11064, 11064, 11448, 11448, 11832, 11832, 11832, 12216, 12216, 12576, 12576, 12960, 12960, 12960, 13536, 13536, 13536, 13536, 14112, 14112, 14112, 14112, 14688, 14688, 14688, 15264, 15264, 15264, 15264, 15840, 15840, 15840, 16416, 16416, 16416, 16416, 16992, 16992, 16992, 16992, 17568 },
    { 144, 328, 504, 680, 872, 1032, 1224, 1384, 1544, 1736, 1928, 2088, 2280, 2472, 2664, 2792, 2984, 3112, 3368, 3496, 3752, 3880, 4008, 4264, 4392, 4584, 4776, 4968, 5160, 5352, 5544, 5736, 5736, 5992, 6200, 6200, 6456, 6712, 6712, 6968, 7224, 7480, 7480, 7736, 7992, 7992, 8248, 8504, 8504, 8760, 9144, 9144, 9144, 9528, 9528, 9912, 9912, 10296, 10296, 10680, 10680, 11064, 11064, 11448, 11448, 11448, 11832, 11832, 12216, 12216, 12576, 12576, 12960, 12960, 12960, 13536, 13536, 13536, 14112, 14112, 14112, 14688, 14688, 14688, 14688, 15264, 15264, 15264, 15840, 15840, 15840, 16416, 16416, 16416, 16992, 16992, 16992, 16992, 17568, 17568, 17568, 18336, 18336, 18336, 18336, 18336, 19080, 19080, 19080, 19080 },
    { 176, 376, 584, 776, 1000, 1192, 1384, 1608, 1800, 2024, 2216, 2408, 2600, 2792, 2984, 3240, 3496, 3624, 3880, 4008, 4264, 4392, 4584, 4776, 4968, 5352, 5544, 5736, 5992, 5992, 6200, 6456, 6712, 6968, 6968, 7224, 7480, 7736, 7736, 7992, 8248, 8504, 8760, 8760, 9144, 9144, 9528, 9528, 9912, 9912, 10296, 10680, 10680, 11064, 11064, 11448, 11448, 11832, 11832, 12216, 12216, 12576, 12576, 12960, 12960, 13536, 13536, 13536, 14112, 14112, 14112, 14688, 14688, 14688, 15264, 15264, 15840, 15840, 15840, 16416, 16416, 16416, 16992, 16992, 16992, 17568, 17568, 17568, 18336, 18336, 18336, 18336, 19080, 19080, 19080, 19080, 19848, 19848, 19848, 19848, 20616, 20616, 20616, 21384, 21384, 21384, 21384, 22152, 22152, 22152 },
    { 208, 440, 680, 904, 1128, 1352, 1608, 1800, 2024, 2280, 2472, 2728, 2984, 3240, 3368, 3624, 3880, 4136, 4392, 4584, 4776, 4968, 5352, 5544, 5736, 5992, 6200, 6456, 6712, 6712, 6968, 7224, 7480, 7736, 7992, 8248, 8504, 8760, 8760, 9144, 9528, 9528, 9912, 9912, 10296, 10680, 10680, 11064, 11064, 11448, 11832, 11832, 12216, 12216, 12576, 12576, 12960, 12960, 13536, 13536, 14112, 14112, 14112, 14688, 14688, 15264, 15264, 15264, 15840, 15840, 16416, 16416, 16416, 16992, 16992, 17568, 17568, 17568, 18336, 18336, 18336, 19080, 19080, 19080, 19080, 19848, 19848, 19848, 20616, 20616, 20616, 21384, 21384, 21384, 21384, 22152, 22152, 22152, 22920, 22920, 22920, 23688, 23688, 23688, 23688, 24496, 24496, 24496, 24496, 25456 },
    { 224, 488, 744, 1000, 1256, 1544, 1800, 2024, 2280, 2536, 2856, 3112, 3368, 3624, 3880, 4136, 4392, 4584, 4968, 5160, 5352, 5736, 5992, 6200, 6456, 6712, 6968, 7224, 7480, 7736, 7992, 8248, 8504, 8760, 9144, 9144, 9528, 9912, 9912, 10296, 10680, 10680, 11064, 11448, 11448, 11832, 12216, 12216, 12576, 12960, 12960, 13536, 13536, 14112, 14112, 14688, 14688, 14688, 15264, 15264, 15840, 15840, 16416, 16416, 16992, 16992, 16992, 17568, 17568, 18336, 18336, 18336, 19080, 19080, 19080, 19848, 19848, 19848, 20616, 20616, 20616, 21384, 21384, 21384, 22152, 22152, 22152, 22920, 22920, 22920, 23688, 23688, 23688, 24496, 24496, 24496, 25456, 25456, 25456, 25456, 26416, 26416, 26416, 26416, 27376, 27376, 27376, 27376, 28336, 28336 },
    { 256, 552, 840, 1128, 1416, 1736, 1992, 2280, 2600, 2856, 3112, 3496, 3752, 4008, 4264, 4584, 4968, 5160, 5544, 5736, 5992, 6200, 6456, 6968, 7224, 7480, 7736, 7992, 8248, 8504, 8760, 9144, 9528, 9912, 9912, 10296, 10680, 11064, 11064, 11448, 11832, 12216, 12216, 12576, 12960, 12960, 13536, 13536, 14112, 14112, 14688, 14688, 15264, 15264, 15840, 15840, 16416, 16416, 16992, 16992, 17568, 17568, 18336, 18336, 18336, 19080, 19080, 19848, 19848, 19848, 20616, 20616, 20616, 21384, 21384, 22152, 22152, 22152, 22920, 22920, 22920, 23688, 23688, 24496, 24496, 24496, 25456, 25456, 25456, 25456, 26416, 26416, 26416, 27376, 27376, 27376, 28336, 28336, 28336, 28336, 29296, 29296, 29296, 29296, 30576, 30576, 30576, 30576, 31704, 31704 },
    { 280, 600, 904, 1224, 1544, 1800, 2152, 2472, 2728, 3112, 3368, 3624, 4008, 4264, 4584, 4968, 5160, 5544, 5736, 6200, 6456, 6712, 6968, 7224, 7736, 7992, 8248, 8504, 8760, 9144, 9528, 9912, 10296, 10296, 10680, 11064, 11448, 11832, 11832, 12216, 12576, 12960, 12960, 13536, 13536, 14112, 14688, 14688, 15264, 15264, 15840, 15840, 16416, 16416, 16992, 16992, 17568, 17568, 18336, 18336, 18336, 19080, 19080, 19848, 19848, 20616, 20616, 20616, 21384, 21384, 22152, 22152, 22152, 22920, 22920, 23688, 23688, 23688, 24496, 24496, 24496, 25456, 25456, 25456, 26416, 26416, 26416, 27376, 27376, 27376, 28336, 28336, 28336, 29296, 29296, 29296, 29296, 30576, 30576, 30576, 30576, 31704, 31704, 31704, 31704, 32856, 32856, 32856, 34008, 34008 },
    { 328, 632, 968, 1288, 1608, 1928, 2280, 2600, 2984, 3240, 3624, 3880, 4264, 4584, 4968, 5160, 5544, 5992, 6200, 6456, 6712, 7224, 7480, 7736, 7992, 8504, 8760, 9144, 9528, 9912, 9912, 10296, 10680, 11064, 11448, 11832, 12216, 12216, 12576, 12960, 13536, 13536, 14112, 14112, 14688, 14688, 15264, 15840, 15840, 16416, 16416, 16992, 16992, 17568, 17568, 18336, 18336, 19080, 19080, 19848, 19848, 19848, 20616, 20616, 21384, 21384, 22152, 22152, 22152, 22920, 22920, 23688, 23688, 24496, 24496, 24496, 25456, 25456, 25456, 26416, 26416, 26416, 27376, 27376, 27376, 28336, 28336, 28336, 29296, 29296, 29296, 30576, 30576, 30576, 30576, 31704, 31704, 31704, 31704, 32856, 32856, 32856, 34008, 34008, 34008, 34008, 35160, 35160, 35160, 35160 },
    { 336, 696, 1064, 1416, 1800, 2152, 2536, 2856, 3240, 3624, 4008, 4392, 4776, 5160, 5352, 5736, 6200, 6456, 6712, 7224, 7480, 7992, 8248, 8760, 9144, 9528, 9912, 10296, 10296, 10680, 11064, 11448, 11832, 12216, 12576, 12960, 13536, 13536, 14112, 14688, 14688, 15264, 15264, 15840, 16416, 16416, 16992, 17568, 17568, 18336, 18336, 19080, 19080, 19848, 19848, 20616, 20616, 20616, 21384, 21384, 22152, 22152, 22920, 22920, 23688, 23688, 24496, 24496, 24496, 25456, 25456, 26416, 26416, 26416, 27376, 27376, 27376, 28336, 28336, 29296, 29296, 29296, 30576, 30576, 30576, 30576, 31704, 31704, 31704, 32856, 32856, 32856, 34008, 34008, 34008, 35160, 35160, 35160, 35160, 36696, 36696, 36696, 36696, 37888, 37888, 37888, 39232, 39232, 39232, 39232 },
    { 376, 776, 1160, 1544, 1992, 2344, 2792, 3112, 3624, 4008, 4392, 4776, 5160, 5544, 5992, 6200, 6712, 7224, 7480, 7992, 8248, 8760, 9144, 9528, 9912, 10296, 10680, 11064, 11448, 11832, 12216, 12576, 12960, 13536, 14112, 14112, 14688, 15264, 15264, 15840, 16416, 16416, 16992, 17568, 17568, 18336, 18336, 19080, 19080, 19848, 19848, 20616, 21384, 21384, 22152, 22152, 22920, 22920, 23688, 23688, 24496, 24496, 24496, 25456, 25456, 26416, 26416, 27376, 27376, 27376, 28336, 28336, 29296, 29296, 29296, 30576, 30576, 30576, 31704, 31704, 31704, 32856, 32856, 32856, 34008, 34008, 34008, 35160, 35160, 35160, 36696, 36696, 36696, 37888, 37888, 37888, 37888, 39232, 39232, 39232, 40576, 40576, 40576, 40576, 42368, 42368, 42368, 42368, 43816, 43816 },
    { 408, 840, 1288, 1736, 2152, 2600, 2984, 3496, 3880, 4264, 4776, 5160, 5544, 5992, 6456, 6968, 7224, 7736, 8248, 8504, 9144, 9528, 9912, 10296, 10680, 11064, 11448, 12216, 12576, 12960, 13536, 13536, 14112, 14688, 15264, 15264, 15840, 16416, 16992, 16992, 17568, 18336, 18336, 19080, 19080, 19848, 20616, 20616, 21384, 21384, 22152, 22152, 22920, 22920, 23688, 24496, 24496, 25456, 25456, 25456, 26416, 26416, 27376, 27376, 28336, 28336, 29296, 29296, 29296, 30576, 30576, 30576, 31704, 31704, 32856, 32856, 32856, 34008, 34008, 34008, 35160, 35160, 35160, 36696, 36696, 36696, 37888, 37888, 37888, 39232, 39232, 39232, 40576, 40576, 40576, 40576, 42368, 42368, 42368, 43816, 43816, 43816, 43816, 45352, 45352, 45352, 46888, 46888, 46888, 46888 },
    { 440, 904, 1384, 1864, 2344, 2792, 3240, 3752, 4136, 4584, 5160, 5544, 5992, 6456, 6968, 7480, 7992, 8248, 8760, 9144, 9912, 10296, 10680, 11064, 11448, 12216, 12576, 12960, 13536, 14112, 14688, 14688, 15264, 15840, 16416, 16992, 16992, 17568, 18336, 18336, 19080, 19848, 19848, 20616, 20616, 21384, 22152, 22152, 22920, 22920, 23688, 24496, 24496, 25456, 25456, 26416, 26416, 27376, 27376, 28336, 28336, 29296, 29296, 29296, 30576, 30576, 31704, 31704, 31704, 32856, 32856, 34008, 34008, 34008, 35160, 35160, 35160, 36696, 36696, 36696, 37888, 37888, 39232, 39232, 39232, 40576, 40576, 40576, 42368, 42368, 42368, 42368, 43816, 43816, 43816, 45352, 45352, 45352, 46888, 46888, 46888, 46888, 48936, 48936, 48936, 48936, 48936, 51024, 51024, 51024 },
    { 488, 1000, 1480, 1992, 2472, 2984, 3496, 4008, 4584, 4968, 5544, 5992, 6456, 6968, 7480, 7992, 8504, 9144, 9528, 9912, 10680, 11064, 11448, 12216, 12576, 12960, 13536, 14112, 14688, 15264, 15840, 15840, 16416, 16992, 17568, 18336, 18336, 19080, 19848, 19848, 20616, 21384, 21384, 22152, 22920, 22920, 23688, 24496, 24496, 25456, 25456, 26416, 26416, 27376, 27376, 28336, 28336, 29296, 29296, 30576, 30576, 31704, 31704, 31704, 32856, 32856, 34008, 34008, 35160, 35160, 35160, 36696, 36696, 36696, 37888, 37888, 39232, 39232, 39232, 40576, 40576, 40576, 42368, 42368, 42368, 43816, 43816, 43816, 45352, 45352, 45352, 46888, 46888, 46888, 46888, 48936, 48936, 48936, 48936, 51024, 51024, 51024, 51024, 52752, 52752, 52752, 52752, 55056, 55056, 55056 },
    { 520, 1064, 1608, 2152, 2664, 3240, 3752, 4264, 4776, 5352, 5992, 6456, 6968, 7480, 7992, 8504, 9144, 9528, 10296, 10680, 11448, 11832, 12576, 12960, 13536, 14112, 14688, 15264, 15840, 16416, 16992, 16992, 17568, 18336, 19080, 19080, 19848, 20616, 21384, 21384, 22152, 22920, 22920, 23688, 24496, 24496, 25456, 25456, 26416, 27376, 27376, 28336, 28336, 29296, 29296, 30576, 30576, 31704, 31704, 32856, 32856, 34008, 34008, 34008, 35160, 35160, 36696, 36696, 36696, 37888, 37888, 39232, 39232, 40576, 40576, 40576, 42368, 42368, 42368, 43816, 43816, 43816, 45352, 45352, 45352, 46888, 46888, 46888, 48936, 48936, 48936, 48936, 51024, 51024, 51024, 51024, 52752, 52752, 52752, 55056, 55056, 55056, 55056, 57336, 57336, 57336, 57336, 59256, 59256, 59256 },
    { 552, 1128, 1736, 2280, 2856, 3496, 4008, 4584, 5160, 5736, 6200, 6968, 7480, 7992, 8504, 9144, 9912, 10296, 11064, 11448, 12216, 12576, 12960, 13536, 14112, 14688, 15264, 15840, 16416, 16992, 17568, 18336, 19080, 19848, 19848, 20616, 21384, 22152, 22152, 22920, 23688, 24496, 24496, 25456, 25456, 26416, 27376, 27376, 28336, 28336, 29296, 29296, 30576, 30576, 31704, 31704, 32856, 32856, 34008, 34008, 35160, 35160, 36696, 36696, 37888, 37888, 37888, 39232, 39232, 40576, 40576, 40576, 42368, 42368, 43816, 43816, 43816, 45352, 45352, 45352, 46888, 46888, 46888, 48936, 48936, 48936, 51024, 51024, 51024, 51024, 52752, 52752, 52752, 55056, 55056, 55056, 55056, 57336, 57336, 57336, 57336, 59256, 59256, 59256, 59256, 61664, 61664, 61664, 61664, 63776 },
    { 584, 1192, 1800, 2408, 2984, 3624, 4264, 4968, 5544, 5992, 6712, 7224, 7992, 8504, 9144, 9912, 10296, 11064, 11448, 12216, 12960, 13536, 14112, 14688, 15264, 15840, 16416, 16992, 17568, 18336, 19080, 19848, 19848, 20616, 21384, 22152, 22920, 22920, 23688, 24496, 25456, 25456, 26416, 26416, 27376, 28336, 28336, 29296, 29296, 30576, 31704, 31704, 32856, 32856, 34008, 34008, 35160, 35160, 36696, 36696, 36696, 37888, 37888, 39232, 39232, 40576, 40576, 42368, 42368, 42368, 43816, 43816, 45352, 45352, 45352, 46888, 46888, 46888, 48936, 48936, 48936, 51024, 51024, 51024, 52752, 52752, 52752, 52752, 55056, 55056, 55056, 57336, 57336, 57336, 57336, 59256, 59256, 59256, 61664, 61664, 61664, 61664, 63776, 63776, 63776, 63776, 66592, 66592, 66592, 66592 },
    { 616, 1256, 1864, 2536, 3112, 3752, 4392, 5160, 5736, 6200, 6968, 7480, 8248, 8760, 9528, 10296, 10680, 11448, 12216, 12576, 13536, 14112, 14688, 15264, 15840, 16416, 16992, 17568, 18336, 19080, 19848, 20616, 20616, 21384, 22152, 22920, 23688, 24496, 24496, 25456, 26416, 26416, 27376, 28336, 28336, 29296, 29296, 30576, 31704, 31704, 32856, 32856, 34008, 34008, 35160, 35160, 36696, 36696, 37888, 37888, 39232, 39232, 40576, 40576, 40576, 42368, 42368, 43816, 43816, 43816, 45352, 45352, 46888, 46888, 46888, 48936, 48936, 48936, 51024, 51024, 51024, 52752, 52752, 52752, 55056, 55056, 55056, 55056, 57336, 57336, 57336, 59256, 59256, 59256, 61664, 61664, 61664, 61664, 63776, 63776, 63776, 63776, 66592, 66592, 66592, 66592, 68808, 68808, 68808, 71112 },
    { 712, 1480, 2216, 2984, 3752, 4392, 5160, 5992, 6712, 7480, 8248, 8760, 9528, 10296, 11064, 11832, 12576, 13536, 14112, 14688, 15264, 16416, 16992, 17568, 18336, 19080, 19848, 20616, 21384, 22152, 22920, 23688, 24496, 25456, 25456, 26416, 27376, 28336, 29296, 29296, 30576, 30576, 31704, 32856, 32856, 34008, 35160, 35160, 36696, 36696, 37888, 37888, 39232, 40576, 40576, 40576, 42368, 42368, 43816, 43816, 45352, 45352, 46888, 46888, 48936, 48936, 48936, 51024, 51024, 52752, 52752, 52752, 55056, 55056, 55056, 55056, 57336, 57336, 57336, 59256, 59256, 59256, 61664, 61664, 61664, 63776, 63776, 63776, 66592, 66592, 66592, 68808, 68808, 68808, 71112, 71112, 71112, 73712, 73712, 75376, 75376, 75376, 75376, 75376, 75376, 75376, 75376, 75376, 75376, 75376 }
    };

}}

ModulationAndCodingSchemes::ModulationAndCodingSchemes() :
    blerModel(imtaphy::l2s::TheLTEBlockErrorModel::getInstance())
{
    // 3GPP 36.213 V10.1.0
    // Table 7.1.7.2.2-1: One-layer to two-layer TBS translation table
    oneToTwoLayersTranslationTable[1544] = 3112;
    oneToTwoLayersTranslationTable[1608] = 3240;
    oneToTwoLayersTranslationTable[1672] = 3368;
    oneToTwoLayersTranslationTable[1736] = 3496;
    oneToTwoLayersTranslationTable[1800] = 3624;
    oneToTwoLayersTranslationTable[1864] = 3752;
    oneToTwoLayersTranslationTable[1928] = 3880;
    oneToTwoLayersTranslationTable[1992] = 4008;
    oneToTwoLayersTranslationTable[2024] = 4008;
    oneToTwoLayersTranslationTable[2088] = 4136;
    oneToTwoLayersTranslationTable[2152] = 4264;
    oneToTwoLayersTranslationTable[2216] = 4392;
    oneToTwoLayersTranslationTable[2280] = 4584;
    oneToTwoLayersTranslationTable[2344] = 4776;
    oneToTwoLayersTranslationTable[2408] = 4776;
    oneToTwoLayersTranslationTable[2472] = 4968;
    oneToTwoLayersTranslationTable[2536] = 5160;
    oneToTwoLayersTranslationTable[2600] = 5160;
    oneToTwoLayersTranslationTable[2664] = 5352;
    oneToTwoLayersTranslationTable[2728] = 5544;
    oneToTwoLayersTranslationTable[2792] = 5544;
    oneToTwoLayersTranslationTable[2856] = 5736;
    oneToTwoLayersTranslationTable[2984] = 5992;
    oneToTwoLayersTranslationTable[3112] = 6200;
    oneToTwoLayersTranslationTable[3240] = 6456;
    oneToTwoLayersTranslationTable[3368] = 6712;
    oneToTwoLayersTranslationTable[3496] = 6968;
    oneToTwoLayersTranslationTable[3624] = 7224;
    oneToTwoLayersTranslationTable[3752] = 7480;
    oneToTwoLayersTranslationTable[3880] = 7736;
    oneToTwoLayersTranslationTable[4008] = 7992;
    oneToTwoLayersTranslationTable[4136] = 8248;
    oneToTwoLayersTranslationTable[4264] = 8504;
    oneToTwoLayersTranslationTable[4392] = 8760;
    oneToTwoLayersTranslationTable[4584] = 9144;
    oneToTwoLayersTranslationTable[4776] = 9528;
    oneToTwoLayersTranslationTable[4968] = 9912;
    oneToTwoLayersTranslationTable[5160] = 10296;
    oneToTwoLayersTranslationTable[5352] = 10680;
    oneToTwoLayersTranslationTable[5544] = 11064;
    oneToTwoLayersTranslationTable[5736] = 11448;
    oneToTwoLayersTranslationTable[5992] = 11832;
    oneToTwoLayersTranslationTable[6200] = 12576;
    oneToTwoLayersTranslationTable[6456] = 12960;
    oneToTwoLayersTranslationTable[6712] = 13536;
    oneToTwoLayersTranslationTable[6968] = 14112;
    oneToTwoLayersTranslationTable[7224] = 14688;
    oneToTwoLayersTranslationTable[7480] = 14688;
    oneToTwoLayersTranslationTable[7736] = 15264;
    oneToTwoLayersTranslationTable[7992] = 15840;
    oneToTwoLayersTranslationTable[8248] = 16416;
    oneToTwoLayersTranslationTable[8504] = 16992;
    oneToTwoLayersTranslationTable[8760] = 17568;
    oneToTwoLayersTranslationTable[9144] = 18336;
    oneToTwoLayersTranslationTable[9528] = 19080;
    oneToTwoLayersTranslationTable[9912] = 19848;
    oneToTwoLayersTranslationTable[10296] = 20616;
    oneToTwoLayersTranslationTable[10680] = 21384;
    oneToTwoLayersTranslationTable[11064] = 22152;
    oneToTwoLayersTranslationTable[11448] = 22920;
    oneToTwoLayersTranslationTable[11832] = 23688;
    oneToTwoLayersTranslationTable[12216] = 24496;
    oneToTwoLayersTranslationTable[12576] = 25456;
    oneToTwoLayersTranslationTable[12960] = 25456;
    oneToTwoLayersTranslationTable[13536] = 27376;
    oneToTwoLayersTranslationTable[14112] = 28336;
    oneToTwoLayersTranslationTable[14688] = 29296;
    oneToTwoLayersTranslationTable[15264] = 30576;
    oneToTwoLayersTranslationTable[15840] = 31704;
    oneToTwoLayersTranslationTable[16416] = 32856;
    oneToTwoLayersTranslationTable[16992] = 34008;
    oneToTwoLayersTranslationTable[17568] = 35160;
    oneToTwoLayersTranslationTable[18336] = 36696;
    oneToTwoLayersTranslationTable[19080] = 37888;
    oneToTwoLayersTranslationTable[19848] = 39232;
    oneToTwoLayersTranslationTable[20616] = 40576;
    oneToTwoLayersTranslationTable[21384] = 42368;
    oneToTwoLayersTranslationTable[22152] = 43816;
    oneToTwoLayersTranslationTable[22920] = 45352;
    oneToTwoLayersTranslationTable[23688] = 46888;
    oneToTwoLayersTranslationTable[24496] = 48936;
    oneToTwoLayersTranslationTable[25456] = 51024;
    oneToTwoLayersTranslationTable[26416] = 52752;
    oneToTwoLayersTranslationTable[27376] = 55056;
    oneToTwoLayersTranslationTable[28336] = 57336;
    oneToTwoLayersTranslationTable[29296] = 59256;
    oneToTwoLayersTranslationTable[30576] = 61664;
    oneToTwoLayersTranslationTable[31704] = 63776;
    oneToTwoLayersTranslationTable[32856] = 66592;
    oneToTwoLayersTranslationTable[34008] = 68808;
    oneToTwoLayersTranslationTable[35160] = 71112;
    oneToTwoLayersTranslationTable[36696] = 73712;
    oneToTwoLayersTranslationTable[37888] = 76208;
    oneToTwoLayersTranslationTable[39232] = 78704;
    oneToTwoLayersTranslationTable[40576] = 81176;
    oneToTwoLayersTranslationTable[42368] = 84760;
    oneToTwoLayersTranslationTable[43816] = 87936;
    oneToTwoLayersTranslationTable[45352] = 90816;
    oneToTwoLayersTranslationTable[46888] = 93800;
    oneToTwoLayersTranslationTable[48936] = 97896;
    oneToTwoLayersTranslationTable[51024] = 101840;
    oneToTwoLayersTranslationTable[52752] = 105528;
    oneToTwoLayersTranslationTable[55056] = 110136;
    oneToTwoLayersTranslationTable[57336] = 115040;
    oneToTwoLayersTranslationTable[59256] = 119816;
    oneToTwoLayersTranslationTable[61664] = 124464;
    oneToTwoLayersTranslationTable[63776] = 128496;
    oneToTwoLayersTranslationTable[66592] = 133208;
    oneToTwoLayersTranslationTable[68808] = 137792;
    oneToTwoLayersTranslationTable[71112] = 142248;
    oneToTwoLayersTranslationTable[73712] = 146856;
    oneToTwoLayersTranslationTable[75376] = 149776;

    // Table 7.7.2.4-1: One-layer to three-layer TBS translation table
    oneToThreeLayersTranslationTable[1032] = 3112;
    oneToThreeLayersTranslationTable[1064] = 3240;
    oneToThreeLayersTranslationTable[1096] = 3240;
    oneToThreeLayersTranslationTable[1128] = 3368;
    oneToThreeLayersTranslationTable[1160] = 3496;
    oneToThreeLayersTranslationTable[1192] = 3624;
    oneToThreeLayersTranslationTable[1224] = 3624;
    oneToThreeLayersTranslationTable[1256] = 3752;
    oneToThreeLayersTranslationTable[1288] = 3880;
    oneToThreeLayersTranslationTable[1320] = 4008;
    oneToThreeLayersTranslationTable[1352] = 4008;
    oneToThreeLayersTranslationTable[1384] = 4136;
    oneToThreeLayersTranslationTable[1416] = 4264;
    oneToThreeLayersTranslationTable[1480] = 4392;
    oneToThreeLayersTranslationTable[1544] = 4584;
    oneToThreeLayersTranslationTable[1608] = 4776;
    oneToThreeLayersTranslationTable[1672] = 4968;
    oneToThreeLayersTranslationTable[1736] = 5160;
    oneToThreeLayersTranslationTable[1800] = 5352;
    oneToThreeLayersTranslationTable[1864] = 5544;
    oneToThreeLayersTranslationTable[1928] = 5736;
    oneToThreeLayersTranslationTable[1992] = 5992;
    oneToThreeLayersTranslationTable[2024] = 5992;
    oneToThreeLayersTranslationTable[2088] = 6200;
    oneToThreeLayersTranslationTable[2152] = 6456;
    oneToThreeLayersTranslationTable[2216] = 6712;
    oneToThreeLayersTranslationTable[2280] = 6712;
    oneToThreeLayersTranslationTable[2344] = 6968;
    oneToThreeLayersTranslationTable[2408] = 7224;
    oneToThreeLayersTranslationTable[2472] = 7480;
    oneToThreeLayersTranslationTable[2536] = 7480;
    oneToThreeLayersTranslationTable[2600] = 7736;
    oneToThreeLayersTranslationTable[2664] = 7992;
    oneToThreeLayersTranslationTable[2728] = 8248;
    oneToThreeLayersTranslationTable[2792] = 8248;
    oneToThreeLayersTranslationTable[2856] = 8504;
    oneToThreeLayersTranslationTable[2984] = 8760;
    oneToThreeLayersTranslationTable[3112] = 9144;
    oneToThreeLayersTranslationTable[3240] = 9528;
    oneToThreeLayersTranslationTable[3368] = 9912;
    oneToThreeLayersTranslationTable[3496] = 10296;
    oneToThreeLayersTranslationTable[3624] = 10680;
    oneToThreeLayersTranslationTable[3752] = 11064;
    oneToThreeLayersTranslationTable[3880] = 11448;
    oneToThreeLayersTranslationTable[4008] = 11832;
    oneToThreeLayersTranslationTable[4136] = 12576;
    oneToThreeLayersTranslationTable[4264] = 12960;
    oneToThreeLayersTranslationTable[4392] = 12960;
    oneToThreeLayersTranslationTable[4584] = 13536;
    oneToThreeLayersTranslationTable[4776] = 14112;
    oneToThreeLayersTranslationTable[4968] = 14688;
    oneToThreeLayersTranslationTable[5160] = 15264;
    oneToThreeLayersTranslationTable[5352] = 15840;
    oneToThreeLayersTranslationTable[5544] = 16416;
    oneToThreeLayersTranslationTable[5736] = 16992;
    oneToThreeLayersTranslationTable[5992] = 18336;
    oneToThreeLayersTranslationTable[6200] = 18336;
    oneToThreeLayersTranslationTable[6456] = 19080;
    oneToThreeLayersTranslationTable[6712] = 19848;
    oneToThreeLayersTranslationTable[6968] = 20616;
    oneToThreeLayersTranslationTable[7224] = 21384;
    oneToThreeLayersTranslationTable[7480] = 22152;
    oneToThreeLayersTranslationTable[7736] = 22920;
    oneToThreeLayersTranslationTable[7992] = 23688;
    oneToThreeLayersTranslationTable[8248] = 24496;
    oneToThreeLayersTranslationTable[8504] = 25456;
    oneToThreeLayersTranslationTable[8760] = 26416;
    oneToThreeLayersTranslationTable[9144] = 27376;
    oneToThreeLayersTranslationTable[9528] = 28336;
    oneToThreeLayersTranslationTable[9912] = 29296;
    oneToThreeLayersTranslationTable[10296] = 30576;
    oneToThreeLayersTranslationTable[10680] = 31704;
    oneToThreeLayersTranslationTable[11064] = 32856;
    oneToThreeLayersTranslationTable[11448] = 34008;
    oneToThreeLayersTranslationTable[11832] = 35160;
    oneToThreeLayersTranslationTable[12216] = 36696;
    oneToThreeLayersTranslationTable[12576] = 37888;
    oneToThreeLayersTranslationTable[12960] = 39232;
    oneToThreeLayersTranslationTable[13536] = 40576;
    oneToThreeLayersTranslationTable[14112] = 42368;
    oneToThreeLayersTranslationTable[14688] = 43816;
    oneToThreeLayersTranslationTable[15264] = 45352;
    oneToThreeLayersTranslationTable[15840] = 46888;
    oneToThreeLayersTranslationTable[16416] = 48936;
    oneToThreeLayersTranslationTable[16992] = 51024;
    oneToThreeLayersTranslationTable[17568] = 52752;
    oneToThreeLayersTranslationTable[18336] = 55056;
    oneToThreeLayersTranslationTable[19080] = 57336;
    oneToThreeLayersTranslationTable[19848] = 59256;
    oneToThreeLayersTranslationTable[20616] = 61664;
    oneToThreeLayersTranslationTable[21384] = 63776;
    oneToThreeLayersTranslationTable[22152] = 66592;
    oneToThreeLayersTranslationTable[22920] = 68808;
    oneToThreeLayersTranslationTable[23688] = 71112;
    oneToThreeLayersTranslationTable[24496] = 73712;
    oneToThreeLayersTranslationTable[25456] = 76208;
    oneToThreeLayersTranslationTable[26416] = 78704;
    oneToThreeLayersTranslationTable[27376] = 81176;
    oneToThreeLayersTranslationTable[28336] = 84760;
    oneToThreeLayersTranslationTable[29296] = 87936;
    oneToThreeLayersTranslationTable[30576] = 90816;
    oneToThreeLayersTranslationTable[31704] = 93800;
    oneToThreeLayersTranslationTable[32856] = 97896;
    oneToThreeLayersTranslationTable[34008] = 101840;
    oneToThreeLayersTranslationTable[35160] = 105528;
    oneToThreeLayersTranslationTable[36696] = 110136;
    oneToThreeLayersTranslationTable[37888] = 115040;
    oneToThreeLayersTranslationTable[39232] = 119816;
    oneToThreeLayersTranslationTable[40576] = 119816;
    oneToThreeLayersTranslationTable[42368] = 128496;
    oneToThreeLayersTranslationTable[43816] = 133208;
    oneToThreeLayersTranslationTable[45352] = 137792;
    oneToThreeLayersTranslationTable[46888] = 142248;
    oneToThreeLayersTranslationTable[48936] = 146856;
    oneToThreeLayersTranslationTable[51024] = 152976;
    oneToThreeLayersTranslationTable[52752] = 157432;
    oneToThreeLayersTranslationTable[55056] = 165216;
    oneToThreeLayersTranslationTable[57336] = 171888;
    oneToThreeLayersTranslationTable[59256] = 177816;
    oneToThreeLayersTranslationTable[61664] = 185728;
    oneToThreeLayersTranslationTable[63776] = 191720;
    oneToThreeLayersTranslationTable[66592] = 199824;
    oneToThreeLayersTranslationTable[68808] = 205880;
    oneToThreeLayersTranslationTable[71112] = 214176;
    oneToThreeLayersTranslationTable[73712] = 221680;
    oneToThreeLayersTranslationTable[75376] = 226416;
       
    // Table 7.1.7.2.5-1: One-layer to four-layer TBS translation table
    oneToFourLayersTranslationTable[776] = 3112;
    oneToFourLayersTranslationTable[808] = 3240;
    oneToFourLayersTranslationTable[840] = 3368;
    oneToFourLayersTranslationTable[872] = 3496;
    oneToFourLayersTranslationTable[904] = 3624;
    oneToFourLayersTranslationTable[936] = 3752;
    oneToFourLayersTranslationTable[968] = 3880;
    oneToFourLayersTranslationTable[1000] = 4008;
    oneToFourLayersTranslationTable[1032] = 4136;
    oneToFourLayersTranslationTable[1064] = 4264;
    oneToFourLayersTranslationTable[1096] = 4392;
    oneToFourLayersTranslationTable[1128] = 4584;
    oneToFourLayersTranslationTable[1160] = 4584;
    oneToFourLayersTranslationTable[1192] = 4776;
    oneToFourLayersTranslationTable[1224] = 4968;
    oneToFourLayersTranslationTable[1256] = 4968;
    oneToFourLayersTranslationTable[1288] = 5160;
    oneToFourLayersTranslationTable[1320] = 5352;
    oneToFourLayersTranslationTable[1352] = 5352;
    oneToFourLayersTranslationTable[1384] = 5544;
    oneToFourLayersTranslationTable[1416] = 5736;
    oneToFourLayersTranslationTable[1480] = 5992;
    oneToFourLayersTranslationTable[1544] = 6200;
    oneToFourLayersTranslationTable[1608] = 6456;
    oneToFourLayersTranslationTable[1672] = 6712;
    oneToFourLayersTranslationTable[1736] = 6968;
    oneToFourLayersTranslationTable[1800] = 7224;
    oneToFourLayersTranslationTable[1864] = 7480;
    oneToFourLayersTranslationTable[1928] = 7736;
    oneToFourLayersTranslationTable[1992] = 7992;
    oneToFourLayersTranslationTable[2024] = 7992;
    oneToFourLayersTranslationTable[2088] = 8248;
    oneToFourLayersTranslationTable[2152] = 8504;
    oneToFourLayersTranslationTable[2216] = 8760;
    oneToFourLayersTranslationTable[2280] = 9144;
    oneToFourLayersTranslationTable[2344] = 9528;
    oneToFourLayersTranslationTable[2408] = 9528;
    oneToFourLayersTranslationTable[2472] = 9912;
    oneToFourLayersTranslationTable[2536] = 10296;
    oneToFourLayersTranslationTable[2600] = 10296;
    oneToFourLayersTranslationTable[2664] = 10680;
    oneToFourLayersTranslationTable[2728] = 11064;
    oneToFourLayersTranslationTable[2792] = 11064;
    oneToFourLayersTranslationTable[2856] = 11448;
    oneToFourLayersTranslationTable[2984] = 11832;
    oneToFourLayersTranslationTable[3112] = 12576;
    oneToFourLayersTranslationTable[3240] = 12960;
    oneToFourLayersTranslationTable[3368] = 13536;
    oneToFourLayersTranslationTable[3496] = 14112;
    oneToFourLayersTranslationTable[3624] = 14688;
    oneToFourLayersTranslationTable[3752] = 15264;
    oneToFourLayersTranslationTable[3880] = 15264;
    oneToFourLayersTranslationTable[4008] = 15840;
    oneToFourLayersTranslationTable[4136] = 16416;
    oneToFourLayersTranslationTable[4264] = 16992;
    oneToFourLayersTranslationTable[4392] = 17568;
    oneToFourLayersTranslationTable[4584] = 18336;
    oneToFourLayersTranslationTable[4776] = 19080;
    oneToFourLayersTranslationTable[4968] = 19848;
    oneToFourLayersTranslationTable[5160] = 20616;
    oneToFourLayersTranslationTable[5352] = 21384;
    oneToFourLayersTranslationTable[5544] = 22152;
    oneToFourLayersTranslationTable[5736] = 22920;
    oneToFourLayersTranslationTable[5992] = 23688;
    oneToFourLayersTranslationTable[6200] = 24496;
    oneToFourLayersTranslationTable[6456] = 25456;
    oneToFourLayersTranslationTable[6712] = 26416;
    oneToFourLayersTranslationTable[6968] = 28336;
    oneToFourLayersTranslationTable[7224] = 29296;
    oneToFourLayersTranslationTable[7480] = 29296;
    oneToFourLayersTranslationTable[7736] = 30576;
    oneToFourLayersTranslationTable[7992] = 31704;
    oneToFourLayersTranslationTable[8248] = 32856;
    oneToFourLayersTranslationTable[8504] = 34008;
    oneToFourLayersTranslationTable[8760] = 35160;
    oneToFourLayersTranslationTable[9144] = 36696;
    oneToFourLayersTranslationTable[9528] = 37888;
    oneToFourLayersTranslationTable[9912] = 39232;
    oneToFourLayersTranslationTable[10296] = 40576;
    oneToFourLayersTranslationTable[10680] = 42368;
    oneToFourLayersTranslationTable[11064] = 43816;
    oneToFourLayersTranslationTable[11448] = 45352;
    oneToFourLayersTranslationTable[11832] = 46888;
    oneToFourLayersTranslationTable[12216] = 48936;
    oneToFourLayersTranslationTable[12576] = 51024;
    oneToFourLayersTranslationTable[12960] = 51024;
    oneToFourLayersTranslationTable[13536] = 55056;
    oneToFourLayersTranslationTable[14112] = 57336;
    oneToFourLayersTranslationTable[14688] = 59256;
    oneToFourLayersTranslationTable[15264] = 61664;
    oneToFourLayersTranslationTable[15840] = 63776;
    oneToFourLayersTranslationTable[16416] = 66592;
    oneToFourLayersTranslationTable[16992] = 68808;
    oneToFourLayersTranslationTable[17568] = 71112;
    oneToFourLayersTranslationTable[18336] = 73712;
    oneToFourLayersTranslationTable[19080] = 76208;
    oneToFourLayersTranslationTable[19848] = 78704;
    oneToFourLayersTranslationTable[20616] = 81176;
    oneToFourLayersTranslationTable[21384] = 84760;
    oneToFourLayersTranslationTable[22152] = 87936;
    oneToFourLayersTranslationTable[22920] = 90816;
    oneToFourLayersTranslationTable[23688] = 93800;
    oneToFourLayersTranslationTable[24496] = 97896;
    oneToFourLayersTranslationTable[25456] = 101840;
    oneToFourLayersTranslationTable[26416] = 105528;
    oneToFourLayersTranslationTable[27376] = 110136;
    oneToFourLayersTranslationTable[28336] = 115040;
    oneToFourLayersTranslationTable[29296] = 115040;
    oneToFourLayersTranslationTable[30576] = 124464;
    oneToFourLayersTranslationTable[31704] = 128496;
    oneToFourLayersTranslationTable[32856] = 133208;
    oneToFourLayersTranslationTable[34008] = 137792;
    oneToFourLayersTranslationTable[35160] = 142248;
    oneToFourLayersTranslationTable[36696] = 146856;
    oneToFourLayersTranslationTable[37888] = 151376;
    oneToFourLayersTranslationTable[39232] = 157432;
    oneToFourLayersTranslationTable[40576] = 161760;
    oneToFourLayersTranslationTable[42368] = 169544;
    oneToFourLayersTranslationTable[43816] = 175600;
    oneToFourLayersTranslationTable[45352] = 181656;
    oneToFourLayersTranslationTable[46888] = 187712;
    oneToFourLayersTranslationTable[48936] = 195816;
    oneToFourLayersTranslationTable[51024] = 203704;
    oneToFourLayersTranslationTable[52752] = 211936;
    oneToFourLayersTranslationTable[55056] = 220296;
    oneToFourLayersTranslationTable[57336] = 230104;
    oneToFourLayersTranslationTable[59256] = 236160;
    oneToFourLayersTranslationTable[61664] = 245648;
    oneToFourLayersTranslationTable[63776] = 254328;
    oneToFourLayersTranslationTable[66592] = 266440;
    oneToFourLayersTranslationTable[68808] = 275376;
    oneToFourLayersTranslationTable[71112] = 284608;
    oneToFourLayersTranslationTable[73712] = 293736;
    oneToFourLayersTranslationTable[75376] = 299856;

}


unsigned int 
ModulationAndCodingSchemes::getSize(unsigned int mcsIndex, unsigned int numPRBs, unsigned int numLayers)
{
    assure((0 <= mcsIndex) && (mcsIndex <= 28), "Invalid MCS index - should be from 0 to 28");
    assure((0 < numPRBs) && (numPRBs <= 110), "Invalid number of PRBs - should be between 1 and 110");
    assure((0 < numLayers) && (numLayers <= 4), "Invalid number of layers - should be between 1 and 4");
    
    // indices 9 and 10 as well as 16 and 17 have identical block lengths
    // -> therefore they are only stored once in the tbSizes table
    if (mcsIndex > 16)
        mcsIndex -= 2;
    else if (mcsIndex > 9)
        mcsIndex -= 1;
    
    switch(numLayers)
    {
        case 1: return tbSizes[mcsIndex][numPRBs - 1]; break;
        case 2: if (numPRBs <= 55) // see 36.213 7.1.7.2.2
                    return 2 * tbSizes[mcsIndex][numPRBs - 1];
                else
                {
                    unsigned int temp = tbSizes[mcsIndex][numPRBs - 1];
                    assure(oneToTwoLayersTranslationTable.find(temp) != oneToTwoLayersTranslationTable.end(), "No lookup value found");
                    return oneToTwoLayersTranslationTable[temp]; 
                }
                break;
        case 3: if (numPRBs <= 36) // see 36.213 7.1.7.2.4
                    return 3 * tbSizes[mcsIndex][numPRBs - 1];
                else
                {
                    unsigned int temp = tbSizes[mcsIndex][numPRBs - 1];
                    assure(oneToThreeLayersTranslationTable.find(temp) != oneToThreeLayersTranslationTable.end(), "No lookup value found");
                    return oneToThreeLayersTranslationTable[temp]; 
                }
                break;
        case 4: if (numPRBs <= 27) // see 36.213 7.1.7.2.5
                    return 4 * tbSizes[mcsIndex][numPRBs - 1];
                else
                {
                    unsigned int temp = tbSizes[mcsIndex][numPRBs - 1];
                    assure(oneToFourLayersTranslationTable.find(temp) != oneToFourLayersTranslationTable.end(), "No lookup value found");
                    return oneToFourLayersTranslationTable[temp]; 
                }
                break;
        default:
                assure(0, "Should never be reached");
    };
    assure(0, "This point should never be reached in the code");
    return 0;
}


unsigned int
ModulationAndCodingSchemes::getNumAvailableDownlinkResourceElements(unsigned int pdcchLength, 
                                                    unsigned int numRel8AntennaPorts,
                                                    unsigned int numPRBs,
                                                    unsigned int numLayers,
                                                    unsigned int numSixCenterPRBs,
                                                    unsigned int tti,
                                                    bool provideRel10DMRS,
                                                    unsigned int numRel10CSIrsPorts,
                                                    unsigned int numRel10CSIrsSets // muted and unmuted, for this TTI
                                                   )
{
    assure((pdcchLength >= 1) && (pdcchLength <= 3), "Control region must be 1-3 symbols over 12 subcarriers per PRB");
    assure((numRel8AntennaPorts == 1) || (numRel8AntennaPorts == 2) || (numRel8AntennaPorts == 4), "Only 1, 2 or 4 antenna ports supported");
    assure((numLayers >= 1) && (numLayers <= 4), "Only 1..4 layers allowed");
    assure(numLayers <= numRel8AntennaPorts, "We cannot have more layers than antennas");
    assure(numSixCenterPRBs <= 6, "numSixCenterPRBs is supposed to indicate how many of the PRBs are within the 6 center PRBs, so must be 0..6");
    assure((numRel10CSIrsPorts == 0) || (numRel10CSIrsPorts == 1) || (numRel10CSIrsPorts == 2) || (numRel10CSIrsPorts == 4) ||(numRel10CSIrsPorts == 8),
           "Only 1, 2, 4, or 8 CSI-RS ports supported");
    
    unsigned int syncSymbols = 0;
    unsigned int bchSymbols = 0;
    
    if (tti % 5 == 0)
        syncSymbols = 2;
    
    if (tti % 10 == 0)
        bchSymbols = 4;

    // 36.814 "Table A.3-1 Simulation assumption for Cell/cell-edge spectrum efficiency"
    // seems to suggest that for transmissions relying on DMRS, a cell-specific overhead 
    // according to transmissioin on antenna port 5 (single antenna port) should be assumed
        
    // Rel10 demodulation reference symbols
    unsigned int numDMRS = 0;
    if (provideRel10DMRS)
    {
        if (numLayers <= 2)
            numDMRS = 12;
        else
            numDMRS = 24;

        // to be transmitted on every PRB scheduled to the user
        numDMRS *= numPRBs;
    }

	// CSI-RS, see Dahlman p. 159
	// 
    // - there is a periodicity of 5..80ms for CSI-RS
    // - they are always transmitted in all PRBs
    // - it is also possible to mute CSI-RS (i.e., not use them for PDSCH mapping) to allow for neighbor-cell CSI estimation
    
    unsigned int numCSIrs = 0;
    if (numRel10CSIrsPorts <= 2)
        numCSIrs = 2;
    else if (numRel10CSIrsPorts == 4)
        numCSIrs = 4;
    else
        numCSIrs = 8;

    numCSIrs *= numRel10CSIrsSets;
    numCSIrs *= numPRBs;
    
    
    numSixCenterPRBs = std::min(numSixCenterPRBs, numPRBs);
    unsigned int syncAndBroadcastSymbols = std::min(numSixCenterPRBs, 6u) * 12 * (bchSymbols + syncSymbols);
            
    unsigned int totalNumResourceElements = 12 * 14 * numPRBs;
    unsigned int numControlResourceElements = 12 * pdcchLength * numPRBs; // L = 1..3 symbols PDCCH size


    // the number of reference symbols in Rel-8 depends on the number of antenna ports
    // here we only count RS outside the first 3 symobls of the subframe because those
    // are assumed to be used for control signaling anyway
    unsigned int numReferenceSymbols;
    switch(numRel8AntennaPorts)
    {
        case 1:
            numReferenceSymbols = 6;
            break;
        case 2:
            numReferenceSymbols = 12;
            break;
        case 4:
        default:
            numReferenceSymbols = 16;
    };

    numReferenceSymbols *= numPRBs;

    
    unsigned int numREsAvailableForData = totalNumResourceElements - numControlResourceElements - syncAndBroadcastSymbols - numReferenceSymbols - numDMRS - numCSIrs;
/*
    std::cout   << "For " << numPRBs << " PRBs (" << numSixCenterPRBs << " of which on 6 center PRBs) on " << numLayers << " layers at TTI " << tti
                << " with " << numAntennaPorts << " antenna ports and a control region of " << pdcchLength << " symbols results in: \n"
                << totalNumResourceElements << " total REs per layer out of which \n"
                << numControlResourceElements << " REs are used for PDCCH control region\n"
                << syncAndBroadcastSymbols << " REs are used for synchronization and BCH\n"
                << numReferenceSymbols << " REs are used for cell-specific (R8) reference symbols\n"
                << " leaving " << numREsAvailableForData << " REs per layer available for PDSCH transmission, overhead of "
                << 100.0 - 100.0 * static_cast<double>(numControlResourceElements) / static_cast<double>(totalNumResourceElements) << "%\n";
*/    

    numREsAvailableForData *= numLayers;

    return numREsAvailableForData;
}


unsigned int
ModulationAndCodingSchemes::getNumAvailableUplinkResourceElements(unsigned int numPRBs,
                                                                  unsigned int numLayers,
                                                                  unsigned int tti,
                                                                  bool hasSRS
                                                                 )
{
    assure((numLayers >= 1) && (numLayers <= 4), "Only 1..4 layers allowed");
    
    unsigned int demodulationRS = 12*2; //two symbols will always be used for reference signal

    unsigned int numSRSymbols;
    if (hasSRS)
    {
        numSRSymbols = 12;
    }
    else
    {
        numSRSymbols = 0;
    }

    

    unsigned int totalNumResourceElements = 12 * 14 * numPRBs;
    unsigned int numReferenceSymbols = numPRBs * (demodulationRS + numSRSymbols);

    // how to treat Uplink L1/L2 control signaling multiplexed on PUSCH?
    // The PUCCH overhead at the edges of the system bandwidth is handled in the Scheduler already.    
    // The exact number of symbols / REs is very hard to determine as it depends
    // on the size of the CQI/PMI/RI feedback (wideband vs. frequency-selective etc.)
    // ACk/NACK of 1 or 2 TBs etc. See TS 36.212, for example.
    // 
    // Fig. 17.15 (Sesia et al.) suggests 46 (CQI/PMI) + 16 (ACK/NACK) + 24 (RI) REs overhead
    // -> we should only have CQI/PMI during TTIs when we have actual feedback; RI is reported even less frequently; 
    // ACK/NACK would always be there if we have downlink traffic (who knows) and the size probably depends on the 
    // number of DL transport blocks
    unsigned int numUplinkControlREs = 0; // we do not assume anything here
    
    unsigned int numREsAvailableForData = totalNumResourceElements - numReferenceSymbols;

    numREsAvailableForData *= numLayers;
    
    numREsAvailableForData -= numUplinkControlREs;

    return numREsAvailableForData;
}

double
ModulationAndCodingSchemes::getEffectiveCodeRate(unsigned int tbSize, unsigned int availableBits)
{
    // For each transport block a 24 bit CRC is attached prior to channel coding
    // If the transport block including the 24 bit CRC (cf. Dahlman 4G book) is bigger than the
    // maximum block size of 6144 bit, it is segmented into so-called code blocks that each get
    // an additional 24 bit CRC checksum. 
    // In addition to the payload bits from the transport block, the CRC bits also need to encoded 
    // and transmitted so that the effective code rate per code block increases. Here, we do not consider
    // that only certain code block sizes are allowed

    // See 3GPP TS 36.212 5.1.2, we use the same variable names as in the standard
       
    unsigned int B = tbSize + 24; // input bit sequence length, i.e., transport block inlcuding TB's CRC
    unsigned int Z = 6144; // maximum code block size
    
    unsigned int C; // num code blocks
    
    if (B <= Z)
    {
        C = 0; // transport block including CRC fits into code block, no segmentation/additional CRC
    }
    else
    {
        unsigned int L = 24; // attach 24 bits CRC per code block
        C = (B + (Z-L) - 1) / (Z-L);  // this is ceil(B/6120)
    }
    
    // we assume all code blocks have identical sizes, for correct procedure see 36.212 5.1.2
    
    unsigned int numCRCbits = 24 * (1 + C); // one CRC for the transport block and for each of the C code blocks
    
    return static_cast<double>(tbSize + numCRCbits) / static_cast<double>(availableBits);
}

double
ModulationAndCodingSchemes::getUplinkCodeRate(unsigned int mcsIndex,
                                              unsigned int numPRBs,
                                              unsigned int numLayers,
                                              unsigned int tti,
                                              bool hasSRS
                                              )
{
    assure(mcsIndex < 29, "MCS index must be 0..28");

    unsigned int numAvailableREs = getNumAvailableUplinkResourceElements(numPRBs,
                                                                         numLayers,
                                                                         tti,
                                                                         hasSRS
                                                                         );

    imtaphy::l2s::ModulationScheme modulation = getModulation(mcsIndex);
    unsigned int numDataBits = getSize(mcsIndex, numPRBs, numLayers);

    double codeRate = getEffectiveCodeRate(numDataBits, modulation.getBitsPerSymbol() * numAvailableREs);

    // There could be situations where we would get a code rate greater than 1
    // let's avoid that
    if (codeRate >= 0.999)
        codeRate = 0.999;
    
    assure(( codeRate >= 0.0) && (codeRate <= 1.0), "Invalid code rate, must be in [0,1]");

    return codeRate;
}


wns::Ratio
ModulationAndCodingSchemes::computeDeltaTFdB(unsigned int mcsIndex, double codeRate, double Ks)
{
    // See Sesia book for explanantion and variable names or TS 36.213 section 5.1.1 

    if (Ks == 0.0)
    {
        return wns::Ratio::from_factor(1.0);
    }
    // the spectral efficiency
    double mpr = static_cast<double>(getModulation(mcsIndex).getBitsPerSymbol()) * codeRate;

    return wns::Ratio::from_factor(pow(2.0, mpr * Ks) - 1.0);
}

unsigned int 
ModulationAndCodingSchemes::getSuitableUplinkMCSindexForEffSINR(wns::Ratio sinr, 
                                                                unsigned int numPRBs, 
                                                                unsigned int numLayers, 
                                                                unsigned int tti, 
                                                                bool hasSRS,
                                                                const wns::Ratio& maxDeltaTFdB,
                                                                double Ks
                                                               )
{
    unsigned int nRE = getNumAvailableUplinkResourceElements(numPRBs,
                                                             numLayers,
                                                             tti,
                                                             hasSRS);

    std::size_t hash = static_cast<std::size_t>(nRE);
    boost::hash_combine(hash, numPRBs);

    if(lut.find(hash) == lut.end())
        computeThresholds(hash, numPRBs, numLayers, tti, hasSRS);

    unsigned int minMCS = 0;
    unsigned int maxMCS = 28;

    unsigned int mcs = minMCS;
        
    std::map<wns::Ratio, unsigned int>::iterator it;

    // No deltaTF, use fast STL algrithm to find MCS
    if(Ks == 0.0)
    {
        it = lut[hash].lower_bound(sinr);
        it--;
        mcs = it->second;
    }
    else
    {
        // TODO: Include maxDeltaTF
        wns::Ratio deltaTF = std::max(maxDeltaTFdB, 
            computeDeltaTFdB(minMCS, getUplinkCodeRate(minMCS, numPRBs, numLayers, tti, hasSRS), Ks));

        it = lut[hash].begin();
        it++;
        while(it != lut[hash].end() && sinr + deltaTF >= it->first)
        {
            it++;
            wns::Ratio deltaTF = std::max(maxDeltaTFdB, 
                computeDeltaTFdB(minMCS, getUplinkCodeRate(it->second, numPRBs, numLayers, tti, hasSRS), Ks));
        }
        it--;
        mcs = it->second;
    }
    assure((mcs >= 0) && (mcs <= 28), "Invalid MCS");
    return mcs;
}






double
ModulationAndCodingSchemes::getDownlinkCodeRate(unsigned int mcsIndex,
                                unsigned int pdcchLength,
                                unsigned int numRel8AntennaPorts,
                                unsigned int numPRBs,
                                unsigned int numLayers,
                                unsigned int numSixCenterPRBs,
                                unsigned int tti,
                                bool provideRel10DMRS,
                                unsigned int numRel10CSIrsPorts,
                                unsigned int numRel10CSIrsSets // muted and unmuted, for this TTI
                               )
{
    assure(mcsIndex < 29, "MCS index must be 0..28");

    unsigned int numAvailableREs = getNumAvailableDownlinkResourceElements(pdcchLength,
                                                                   numRel8AntennaPorts,
                                                                   numPRBs,
                                                                   numLayers,
                                                                   numSixCenterPRBs,
                                                                   tti,
                                                                   provideRel10DMRS,
                                                                   numRel10CSIrsPorts,
                                                                   numRel10CSIrsSets
                                                                  );

    imtaphy::l2s::ModulationScheme modulation = getModulation(mcsIndex);
    unsigned int numDataBits = getSize(mcsIndex, numPRBs, numLayers);

    double codeRate = getEffectiveCodeRate(numDataBits, modulation.getBitsPerSymbol() * numAvailableREs);
    
    // There could be situations where we would get a code rate greater than 1
    // let's avoid that
    if (codeRate >= 0.999)
        codeRate = 0.999;
    
    assure(( codeRate >= 0.0) && (codeRate <= 1.0), "Invalid code rate, must be in [0,1]");

    return codeRate;
}



imtaphy::l2s::ModulationScheme
ModulationAndCodingSchemes::getModulation(unsigned int mcsIndex)
{
    assure((0 <= mcsIndex) && (mcsIndex <= 28), "Invalid MCS index - should be from  0 to 28");
    
    if (mcsIndex < 10)
        return imtaphy::l2s::QPSK(); // 0..9 is QPSK
    else if (mcsIndex < 17)
        return imtaphy::l2s::QAM16(); // 10..16 is QAM16
    else
        return imtaphy::l2s::QAM64(); // 17..28 is QAM64
    
}



unsigned int 
ModulationAndCodingSchemes::getSuitableDownlinkMCSindexForEffSINR(wns::Ratio sinr, 
                                                         unsigned int numPRBs, 
                                                         unsigned int numLayers, 
                                                         unsigned int pdcchLength, 
                                                         unsigned int numRel8AntennaPorts, 
                                                         unsigned int numSixCenterPRBs, 
                                                         unsigned int tti, 
                                                         bool provideRel10DMRS, 
                                                         unsigned int numRel10CSIrsPorts, 
                                                         unsigned int numRel10CSIrsSets)
{
    int minMCS = 0;
    int maxMCS = 28;
    
    std::multimap<double, unsigned int> mcsRanking; 
    unsigned int conventionalMCS = minMCS;
    
    int mcs;
    for (mcs = maxMCS; mcs >= minMCS; mcs--)
    {
        unsigned int blockSize = getSize(mcs, numPRBs, numLayers);
        double codeRate = getDownlinkCodeRate(mcs, pdcchLength, numRel8AntennaPorts, numPRBs, numLayers, numSixCenterPRBs, tti, provideRel10DMRS, numRel10CSIrsPorts, numRel10CSIrsSets);
        double bler = blerModel->getBlockErrorRate(sinr, getModulation(mcs), codeRate, blockSize);
        
        // expected throughput
        mcsRanking.insert(std::make_pair<double, unsigned int>((1.0 - bler) * static_cast<double>(blockSize), mcs));
        
        if ((bler <= 0.1) && (conventionalMCS == minMCS))
            conventionalMCS = mcs;
    }

    //TODO: decide whether we want to make the flavour (and maybe the BLER target) configurable
    mcs = mcsRanking.rbegin()->second;
//     std::cout << "At SINR=" << sinr << " 10% BLER scheme gives " << conventionalMCS << " while max throughput scheme gives " << mcs << "\n";

    
    assure((mcs >= 0) && (mcs <= 28), "Invalid MCS");
    return mcs;
}

void
ModulationAndCodingSchemes::computeThresholds(unsigned int hash,
                   unsigned int numPRBs, 
                    unsigned int numLayers, 
                    unsigned int tti, 
                    bool hasSRS)
{
    int minMCS = 0;
    int maxMCS = 28;

#ifndef WNS_NDEBUG
    std::cout << "Computing Link Adaptation thresholds - this may take some time...\n";
#endif
    
    // MCS 0 is always possible
    lut[hash][wns::Ratio::from_factor(1.0/std::numeric_limits<float>::max())] = 0;

    int mcs;
    for (mcs = minMCS + 1; mcs <= maxMCS; mcs++)
    {
        unsigned int blockSize1 = getSize(mcs - 1, numPRBs, numLayers);
        double codeRate1 = getUplinkCodeRate(mcs - 1, numPRBs, numLayers, tti, hasSRS);

        unsigned int blockSize2 = getSize(mcs, numPRBs, numLayers);
        double codeRate2 = getUplinkCodeRate(mcs, numPRBs, numLayers, tti, hasSRS);

        unsigned int blockSize3;
        double codeRate3;
        if(mcs < maxMCS)
        {
            blockSize3 = getSize(mcs + 1, numPRBs, numLayers);
            codeRate3 = getUplinkCodeRate(mcs + 1, numPRBs, numLayers, tti, hasSRS);
        }
        else
        {
            // Put in something to prevent compiler warnings etc.
            blockSize3 = 1;
            codeRate3 = 1.0;
        }

        for(double x = -10.0; x < 22.0; x += 0.01)
        {
        
            wns::Ratio sinr = wns::Ratio::from_dB(x);
            double bler1 = blerModel->getBlockErrorRate(sinr, 
                                                   getModulation(mcs - 1), codeRate1, blockSize1);
            double bler2 = blerModel->getBlockErrorRate(sinr, 
                                                   getModulation(mcs), codeRate2, blockSize2);

            double bler3;
            if(mcs < maxMCS)
            {
                bler3 = blerModel->getBlockErrorRate(sinr, 
                                                       getModulation(mcs + 1), codeRate3, blockSize3);
            }
            else
                bler3 = 1.0;

            // The BLER LUT has problems interpolating very high BLERs, 
            // they should not be used anyways
            if(bler1 > 0.8)
                bler1 = 1.0;
            if(bler2 > 0.8)
                bler2 = 1.0;
            if(bler3 > 0.8)
                bler3 = 1.0;

            double thr1 = (1.0 - bler1) * static_cast<double>(blockSize1);
            double thr2 = (1.0 - bler2) * static_cast<double>(blockSize2);
            double thr3 = (1.0 - bler3) * static_cast<double>(blockSize3);

            if(thr2 > thr1 || thr3 > thr1)
            {
                // Skip one MCS
                if(thr3 > thr2)
                    lut[hash][sinr] = mcs + 1;
                else
                    lut[hash][sinr] = mcs;

                break;
            }
        }
    }
}



