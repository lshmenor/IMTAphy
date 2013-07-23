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

#include <IMTAPHY/link2System/Modulations.hpp>

using namespace imtaphy::l2s;

// the base class should not be instantiated
ModulationScheme::ModulationScheme():
    bitsPerSymbol(0),
    name("none")
{
}

BPSK::BPSK()
{
    bitsPerSymbol = 1;
    name = std::string("BPSK");
}

QPSK::QPSK()
{
    bitsPerSymbol = 2;
    name = std::string("QPSK");
}

QAM8::QAM8()
{
    bitsPerSymbol = 3;
    name = std::string("QAM8");
}

QAM16::QAM16()
{
    bitsPerSymbol = 4;
    name = std::string("QAM16");
}

QAM32::QAM32()
{
    bitsPerSymbol = 5;
    name = std::string("QAM32");
}

QAM64::QAM64()
{
    bitsPerSymbol = 6;
    name = std::string("QAM64");
}

QAM128::QAM128()
{
    bitsPerSymbol = 7;
    name = std::string("QAM128");
}

QAM256::QAM256()
{
    bitsPerSymbol = 8;
    name = std::string("QAM256");
}

