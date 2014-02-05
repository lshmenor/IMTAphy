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

#include <IMTAPHY/receivers/filters/MMSE.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>

using namespace imtaphy;
using namespace imtaphy::receivers::filters;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::filters::MMSE,
    imtaphy::receivers::filters::ReceiveFilterInterface,
    "imtaphy.receiver.filter.MMSE",
    wns::PyConfigViewCreator);
    

MMSE::MMSE(const wns::pyconfig::View& pyConfigView): 
    ReceiveFilterInterface(pyConfigView),
    ServingPrecodedChannelCovariance(imtaphy::detail::ComplexFloatMatrixPtr()), // "smart" null pointers
    inverse(imtaphy::detail::ComplexFloatMatrixPtr()),
    maxRank(pyConfigView.get<int>("maxRank"))
{
}

void 
MMSE::initFilterComputation(Channel* channel, receivers::LinearReceiver* receiver_)
{
    receiver = receiver_;
    numRxAntennas = receiver->getNumRxAntennas();
    
    // for MMSE only, to hold the inverse of the covariance matrix:
    inverse = imtaphy::detail::ComplexFloatMatrixPtr(new imtaphy::detail::ComplexFloatMatrix(numRxAntennas, numRxAntennas));
    
    ServingPrecodedChannelCovariance = imtaphy::detail::ComplexFloatMatrixPtr(new imtaphy::detail::ComplexFloatMatrix(numRxAntennas, numRxAntennas)); 

    if (receiver->getStation()->getStationType() == MOBILESTATION)
    { // only mobiles should compute feedback
    
        // That's the max number of layers we can handle
        unsigned int numTxAntennasServingBS = channel->getLinkManager()->getServingLinkForMobileStation(receiver->getStation())->getBS()->getAntenna()->getNumberOfElements();
        int rank = std::min(numRxAntennas, numTxAntennasServingBS);
        
        if (maxRank > 0)
        {
            rank = std::min(rank, maxRank);
        }
        
        receiver->setMaxRank(rank);
    }
}





void 
MMSE::computeFilter(imtaphy::detail::ComplexFloatMatrix& W, imtaphy::detail::ComplexFloatMatrix& precodedH, InterferersCollectionPtr interferers, unsigned int prb, StationPhy* desiredNode, unsigned int m)
{
    // numRxAntennas is the number of Rx antennas u
    // m is the number of streams

    assure(numRxAntennas == precodedH.getRows(), "Precoded channel matrix needs as many rows as we have Rx antennas");
    assure(numRxAntennas == W.getRows(), "Filter matrix needs as many rows as we have Rx antennas");
    assure(m == precodedH.getColumns(), "Precoded channel matrix needs as many columns as we expect streams/layers");
    assure(m == W.getColumns(), "Filter matrix matrix needs as many columns as we expect streams/layers");
    
    // prepare inteference and noise covariance matrix
    imtaphy::detail::ComplexFloatMatrixPtr IandNoiseCovariance = receiver->getEstimatedInterferenceAndNoiseCovariance(prb, interferers);

    // all involved matrices have been pre-allocated and are reused for each prb again
    // compute u-by-u covariance matrix Hs * Hs^H with Hs being the precoded channel as a u-by-m matrix
    imtaphy::detail::matrixMultiplyCequalsAAhermitian<float>(*ServingPrecodedChannelCovariance, precodedH);

    // add the noise and interference covariance matrix to it
    imtaphy::detail::matrixAddAtoB(*IandNoiseCovariance, *ServingPrecodedChannelCovariance);

    // take the inverse
    imtaphy::detail::matrixInverse(*ServingPrecodedChannelCovariance, *inverse);

    // W is u-by-m, Ainv is u-by-u and Hs is u-by-m and 
    imtaphy::detail::matrixMultiplyCequalsAB(W, *inverse, precodedH);
}