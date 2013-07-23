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

#include <IMTAPHY/receivers/channelEstimation/covariance/IntraAndInterCellDistinguisher.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>
#include <IMTAPHY/receivers/Interferer.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::receivers::channelEstimation::covariance::IntraAndInterCellDistinguisher,
    imtaphy::receivers::channelEstimation::covariance::NoiseAndInterferenceCovarianceInterface,
    "imtaphy.receiver.covarianceEstimation.IntraAndInterCellDistinguisher",
    imtaphy::receivers::ReceiverModuleCreator);

using namespace imtaphy::receivers::channelEstimation::covariance;

IntraAndInterCellDistinguisher::IntraAndInterCellDistinguisher(LinearReceiver* receiver_) :
        NoiseAndInterferenceCovarianceInterface(receiver_),
        receiver(receiver_),
        numRxAntennas(receiver->getNumRxAntennas()),
        direction(receiver->getDirection()),
        intraCellEstimation(NULL),
        interCellEstimation(NULL)
{
}

IntraAndInterCellDistinguisher::IntraAndInterCellDistinguisher(LinearReceiver* receiver_, const wns::pyconfig::View& pyConfigView) :
        NoiseAndInterferenceCovarianceInterface(receiver_, pyConfigView),
        receiver(receiver_),
        numRxAntennas(receiver->getNumRxAntennas()),
        direction(receiver->getDirection()),
        intraCellEstimation(NULL),
        interCellEstimation(NULL)
{

    // we only want to use the specified intraCell Estimation model if it is provided. Otherwise we compute the I+N covariance ourself from the serving channel "estimates"
    if (!pyConfigView.isNone("intraCellEstimation"))
    {
        wns::pyconfig::View intraCellEstimationConfig(pyConfigView.get("intraCellEstimation"));
        intraCellEstimation= wns::StaticFactory<imtaphy::receivers::ReceiverModuleCreator<imtaphy::receivers::channelEstimation::covariance::NoiseAndInterferenceCovarianceInterface> >::creator(intraCellEstimationConfig.get<std::string>("nameInFactory"))->create(receiver_, intraCellEstimationConfig);
        
    }

    wns::pyconfig::View interCellEstimationConfig(pyConfigView.get("interCellEstimation"));
    interCellEstimation = wns::StaticFactory<imtaphy::receivers::ReceiverModuleCreator<imtaphy::receivers::channelEstimation::covariance::NoiseAndInterferenceCovarianceInterface> >::creator(interCellEstimationConfig.get<std::string>("nameInFactory"))->create(receiver_, interCellEstimationConfig);

    
    imtaphy::StationPhy* receiversStation = receiver->getStation();
    if (receiversStation->getStationType() == imtaphy::BASESTATION)
    {
        servingBasestation = receiversStation;
    }
    else
    {
        assure(receiversStation->getStationType() == imtaphy::MOBILESTATION, "Must be either BS or MS");
        imtaphy::Link* servingLink = imtaphy::TheIMTAChannel::Instance().getLinkManager()->getServingLinkForMobileStation(receiversStation);
        
        servingBasestation = servingLink->getBS();
    }
    
    noNoiseMatrix = imtaphy::detail::ComplexFloatMatrixPtr(new imtaphy::detail::ComplexFloatMatrix(numRxAntennas, numRxAntennas));
    for (unsigned int i = 0; i < noNoiseMatrix->getColumns()*noNoiseMatrix->getRows(); i++)
    {
        noNoiseMatrix->data()[i] = std::complex<float>(0,0);
    }
}

imtaphy::detail::ComplexFloatMatrixPtr
IntraAndInterCellDistinguisher::computeNoiseAndInterferenceCovariance(imtaphy::receivers::InterferersCollectionPtr interferers, const imtaphy::detail::ComplexFloatMatrixPtr noiseOnlyMatrix, unsigned int prb)
{
    imtaphy::detail::ComplexFloatMatrixPtr intraCellCovariance;
    imtaphy::detail::ComplexFloatMatrixPtr interCellCovariance;
    
    // split interferer collections in intra and inter cell interferer collections

    imtaphy::receivers::InterferersSet allInterferers = interferers->getInterferersSet();

    InterferersCollectionPtr intraCellInterferers(new InterferersCollection());
    InterferersCollectionPtr interCellInterferers(new InterferersCollection());
        
    for (imtaphy::receivers::InterferersSet::iterator iter = allInterferers.begin(); iter != allInterferers.end(); iter++)
    {
        if (direction == imtaphy::Downlink)
        {
            if (iter->interferingTransmission->getSource() == servingBasestation)
            {
                intraCellInterferers->insert(*iter);
            }
            else
            {
                interCellInterferers->insert(*iter);
            }
        }
        else // Uplink
        {
            if (iter->interferingTransmission->getDestination() == servingBasestation)
            {
                intraCellInterferers->insert(*iter);
            }
            else
            {
                interCellInterferers->insert(*iter);
            }
        }
    }
    intraCellInterferers->seal();
    interCellInterferers->seal();
        
// #pragma omp critical
// {
//     std::cout << "In receiver " << receiver->getStation()->getNode()->getName() << " we see the following intra-cell interferers:\n";
//     for (imtaphy::receivers::InterferersSet::iterator iter = intraCellInterferers->getInterferersSet().begin(); iter != intraCellInterferers->getInterferersSet().end(); iter++)
//     {
//         std::cout << "Intra-cell transmission from " << iter->interferingTransmission->getSource()->getName() << " to " << iter->interferingTransmission->getDestination()->getName() << "\n";
//     }
//     std::cout << "In receiver " << receiver->getStation()->getNode()->getName() << " we see the following inter-cell interferers:\n";
//     for (imtaphy::receivers::InterferersSet::iterator iter = interCellInterferers->getInterferersSet().begin(); iter != interCellInterferers->getInterferersSet().end(); iter++)
//     {
//         std::cout << "Inter-cell transmission from " << iter->interferingTransmission->getSource()->getName() << " to " << iter->interferingTransmission->getDestination()->getName() << "\n";
//     }
// }
    
    if (intraCellEstimation)
    {
        // if an intra-cell interference covariance error model is configured, use that
        // for intra cell interference covariance, assume no noise 
        intraCellCovariance = intraCellEstimation->computeNoiseAndInterferenceCovariance(intraCellInterferers, noNoiseMatrix, prb);
    }
    else
    {
        // otherwise compute intra-cell covariance on our own based on potentially mis-estimated serving channels
        intraCellCovariance = computeIntraCellInterferenceCovarianceBasedOnChannelEstimate(intraCellInterferers, prb);
    }
    
    // compute inter-cell interference based on configured error model and add noise here
    interCellCovariance = interCellEstimation->computeNoiseAndInterferenceCovariance(interCellInterferers, noiseOnlyMatrix, prb);

//     std::cout << "Intra-Cell I covariance:\n";
//     imtaphy::detail::displayMatrix(*intraCellCovariance);
//     std::cout << "Inter-Cell I+N covariance:\n";
//     imtaphy::detail::displayMatrix(*interCellCovariance);

    // add intra and inter cell covariance matrices
    imtaphy::detail::matrixAddAtoB(*intraCellCovariance, *interCellCovariance);

//     std::cout << "Sum I+N covariance:\n";
//     imtaphy::detail::displayMatrix(*interCellCovariance);
    
    return interCellCovariance;
}


imtaphy::detail::ComplexFloatMatrixPtr
IntraAndInterCellDistinguisher::computeIntraCellInterferenceCovarianceBasedOnChannelEstimate(imtaphy::receivers::InterferersCollectionPtr interferers, unsigned int prb)
{
    imtaphy::detail::ComplexFloatMatrixPtr intraCellCovariance(new imtaphy::detail::ComplexFloatMatrix(*noNoiseMatrix));
    
    for (imtaphy::receivers::InterferersSet::const_iterator iter = interferers->getInterferersSet().begin(); iter != interferers->getInterferersSet().end(); iter++)
    {
        const imtaphy::receivers::Interferer& interferer = *iter;
        
        unsigned int mI = interferer.interferingTransmission->getNumLayers();
        
        // we use the Tx power of the interfering intra-cell transmission
        // actually, it should be the Tx power of the reference symbols
        // TODO: check whether we can have a dedicated RS power level (for power boosting) when
        // computing the "estimated" serving channels with error. also applies to the filterComputation inside LinearReceiver
        imtaphy::detail::ComplexFloatMatrixPtr channel = receiver->getEstimatedChannel(prb, interferer.interferingLink, interferer.interferingTransmission->getTxPower(prb));
        
//         std::cout << "Transmit power " << interferer.interferingTransmission->getTxPower(prb) << "\n";
//         imtaphy::detail::ComplexFloatMatrix rawChannel(numRxAntennas, interferer.interferingTransmission->getNumTxAntennas(), interferer.interferingLink->getRawChannelMatrix(imtaphy::Downlink, prb));
//         
//         std::cout << "Using raw channel=\n";
//         imtaphy::detail::displayMatrix(rawChannel);
//         
//         std::cout << "Using estimated channel=\n";
//         imtaphy::detail::displayMatrix(*channel);
        
        imtaphy::detail::ComplexFloatMatrixPtr precoding = interferer.interferingTransmission->getPrecodingMatrix(prb);
        
        assure(precoding != imtaphy::detail::ComplexFloatMatrixPtr(), "Invalid precoding matrix");
        assure(precoding->getRows() == interferer.interferingTransmission->getNumTxAntennas(), "Inconsistent info about Tx antennas");
        assure(precoding->getColumns() == mI, "Inconsistent info about num layers");
        
        imtaphy::detail::ComplexFloatMatrix interferersEffectiveChannel(numRxAntennas, mI);

        imtaphy::detail::matrixMultiplyCequalsAB(interferersEffectiveChannel, *channel, *precoding);
        
        // sum = (alpha*A) * (alpha*A)^H + sum
        imtaphy::detail::matrixMultiplyCequalsAlphaSquareTimesAAhermitianPlusC(*intraCellCovariance, // increment this
                                                                                interferersEffectiveChannel, // add the alpha^2 * AA^H
                                                                                static_cast<float>(1.0) // alpha^2
                                                                                );

    }  
    
    return intraCellCovariance;
}

