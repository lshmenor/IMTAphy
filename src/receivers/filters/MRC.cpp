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

#include <IMTAPHY/receivers/filters/MRC.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>

using namespace imtaphy::receivers::filters;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::filters::MRC,
    imtaphy::receivers::filters::ReceiveFilterInterface,
    "imtaphy.receiver.filter.MRC",
    wns::PyConfigViewCreator);
    
    

MRC::MRC(const wns::pyconfig::View& pyConfigView): 
    ReceiveFilterInterface(pyConfigView)
{
}


void 
MRC::initFilterComputation(imtaphy::Channel* channel, imtaphy::receivers::LinearReceiver* receiver)
{
    numRxAntennas = receiver->getNumRxAntennas();
    receiver->setMaxRank(1);
}


void 
MRC::computeFilter(imtaphy::detail::ComplexFloatMatrix& W, imtaphy::detail::ComplexFloatMatrix& precodedH, InterferersCollectionPtr interferers, unsigned int prb, StationPhy* desiredNode, unsigned int m)
{
    // This MRC implementation is only supposed to work for the case of one stream
    // Therefore, we first check if we are receiving one stream

    assure(m == 1, "MRC is not supposed to work on multi-layer transmissions");
    assure(W.getColumns() == 1, "MRC is not supposed to work on multi-layer transmissions");
    assure(precodedH.getColumns() == 1, "MRC is not supposed to work on multi-layer transmissions");
    assure(W.getRows() == precodedH.getRows(), "Equivalent channel matrix and filter matrix must have identical number of rows");
    assure(numRxAntennas == W.getRows(), "Equivalent channel matrix and filter matrix must be identical to #Rx antennas");
    
    // for MRC, we just use the equivalent channel as the MRC filter
    memcpy(W.getLocation(), precodedH.getLocation(), sizeof(std::complex<float>) * numRxAntennas * 1);
}
