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

#include <IMTAPHY/receivers/filters/NoFilter.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>

using namespace imtaphy::receivers::filters;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::filters::NoFilter,
    imtaphy::receivers::filters::ReceiveFilterInterface,
    "imtaphy.receiver.filter.NoFilter",
    wns::PyConfigViewCreator);
    
    
    
NoFilter::NoFilter(const wns::pyconfig::View& pyConfigView): ReceiveFilterInterface(pyConfigView)
{
}



void 
NoFilter::initFilterComputation(imtaphy::Channel* channel, imtaphy::receivers::LinearReceiver* receiver)
{
    numRxAntennas = receiver->getNumRxAntennas();
    receiver->setMaxRank(1);
}


void 
NoFilter::computeFilter(imtaphy::detail::ComplexFloatMatrix& W, imtaphy::detail::ComplexFloatMatrix& precodedH, InterferersCollectionPtr interferers, unsigned int prb, StationPhy* desiredNode, unsigned int m)
{
    // we simply fill the filter matrix with ones:

    unsigned int rows = W.getRows();
    unsigned int cols = W.getColumns();
    
    for (unsigned int i = 0; i < rows; i++)
        for (unsigned int j = 0; j < cols; j++)
            W[i][j] = std::complex<float>(1.0, 0.);
}

//#endif // MKL