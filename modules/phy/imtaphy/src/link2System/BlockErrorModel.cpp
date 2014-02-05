/*******************************************************************************
* This file is part of IMTAphy
* _____________________________________________________________________________
*
* Copyright (C) 2011-12
* Institute for Communication Networks (LKN)
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


#include <IMTAPHY/link2System/BlockErrorModel.hpp>
#include <algorithm>

using namespace imtaphy::l2s;
using namespace imtaphy::detail;

namespace imtaphy { namespace detail {
    double
    getSinr(unsigned int index, unsigned int numSINRs, double minSINR, double maxSINR)
    {
        return (maxSINR - minSINR) * static_cast<double>(index) / static_cast<double>(numSINRs) + minSINR;
    }
}}

BlockErrorModel::BlockErrorModel() :
    blerLUTs(3),
    minCR(3, 0.0),
    maxCR(3, 0.0),
    keys(3),
    offsets(3),
    sinrOffsetQPSK(wns::Ratio::from_dB(0.0)),
    sinrOffsetQAM16(wns::Ratio::from_dB(0.0)),
    sinrOffsetQAM64(wns::Ratio::from_dB(0.0)),
    sinrThresholds(16)
{
#include "matlab/output.txt"

    // Lookup tables are 3D:
    // first dimension: sinr values with "numSINRs" entries (230)
    // second dimension: codeRates with "numCodeRates" entries (6)
    // third dimension: blockLengths with "numBlockLengths" entries (4)

    // we separate by QPSK (index 0), QAM16 (index 1), QAM64 (index 2)

    typedef boost::multi_array_ref<double, 2> BLERCurvesType;

    std::vector<BLERCurvesType*> curves(3);
    
    curves[0] = new BLERCurvesType(&(QPSK[0][0]), boost::extents[numSINRs][numQPSKcodeRates*numBlockLengths]);
    curves[1] = new BLERCurvesType(&(QAM16[0][0]), boost::extents[numSINRs][numQAM16codeRates*numBlockLengths]);
    curves[2] = new BLERCurvesType(&(QAM64[0][0]), boost::extents[numSINRs][numQAM64codeRates*numBlockLengths]);


    std::vector<std::vector<unsigned int> > shapes(3);
    for (unsigned int m = 0; m < 3; m++)
    {
        shapes[m] = std::vector<unsigned int>(3);
        shapes[m][0] = numSINRs;
        shapes[m][2] = numBlockLengths;
    }
    shapes[0][1] = numQPSKcodeRates;
    shapes[1][1] = numQAM16codeRates;
    shapes[2][1] = numQAM64codeRates;

    for (unsigned int m = 0; m < 3; m++)
    {
        keys[m] = std::vector<std::vector<float> >(3);
        keys[m][0] = std::vector<float>(sinrs, sinrs + numSINRs);
        keys[m][2] = std::vector<float>(blockLengths, blockLengths+ numBlockLengths);
    }

    keys[0][1] = std::vector<float>(qpskCodeRates, qpskCodeRates + numQPSKcodeRates);
    keys[1][1] = std::vector<float>(qam16CodeRates, qam16CodeRates + numQAM16codeRates);
    keys[2][1] = std::vector<float>(qam64CodeRates, qam64CodeRates + numQAM64codeRates);

    minSINR = *std::min_element(keys[0][0].begin(), keys[0][0].end());
    maxSINR = *std::max_element(keys[0][0].begin(), keys[0][0].end());

    for (unsigned int m = 0; m < 3; m++)
    {
        minCR[m] = *std::min_element(keys[m][1].begin(), keys[m][1].end());
        maxCR[m] = *std::max_element(keys[m][1].begin(), keys[m][1].end());
        minBL = *std::min_element(keys[m][2].begin(), keys[m][2].end());
        maxBL = *std::max_element(keys[m][2].begin(), keys[m][2].end());
    }

    // we could add a debug check that all keys are monotonic
    // depending on whether we are in debug mode or not, we want to have the
    // LookupTable perform checks whether all elements accessed are first initialized

    for (unsigned int m = 0; m < 3; m++)
    {
        for (unsigned int i = 0; i < keys[m][2].size(); i++)
        {
            // maybe: have a logarithmic mapping of the block length keys?
            keys[m][2][i] = keys[m][2][i];
        }

#ifdef WNS_NDEBUG
        blerLUTs[m] = new BLERLookUpTable(shapes[m], keys[m], false);
#else
        blerLUTs[m] = new BLERLookUpTable(shapes[m], keys[m], true);
#endif  
    }

    std::vector<unsigned int> position(3);

    for (unsigned int m = 0; m < 3; m++)
    {
        offsets[m] = std::vector<float>(shapes[m][1] - 1);
        for (unsigned int cr = 0; cr < shapes[m][1]; cr++)
        {
            // to average offsets over block lengths and sinrs
            unsigned int totalDistance = 0;
            unsigned int numCompared = 0;
            
            for (unsigned int bl = 0; bl < shapes[m][2]; bl++)
            {
                unsigned int column =  shapes[m][1] * bl + cr;
                
                // we want to make sure that the curves are monotonously decreasing
                // to be able to measure their "distance" below
                // In the raw link level simulations it might be that there are 
                // BLERs like 1.0, 1.0, 0.9998, 1.0, 1.0, ... because of one 
                // single block error that a occured at a SINR a little below the 
                // point where the curves start "to fall"
                for (unsigned int sinr = shapes[m][0]-1; sinr >= 1; --sinr)
                {
                    if ((*curves[m])[sinr - 1][column]  < (*curves[m])[sinr][column])
                        (*curves[m])[sinr - 1][column]  = (*curves[m])[sinr][column];
                }
                    
                for (unsigned int sinr = 0; sinr < shapes[m][0]; sinr++)
                {
                    if (cr < shapes[m][1] - 1)
                    { // there is another higher code rate for this blocklength
                        // find the average "distance" along the SINR-axis of the curves

                        double currentBLER = (*curves[m])[sinr][column];
                        if ((currentBLER < 0.6) && (currentBLER > 0.2))
                        {
                            unsigned int higherCRsinr;
                            for (higherCRsinr = 0; higherCRsinr < shapes[m][0]; higherCRsinr++)
                                if ((*curves[m])[higherCRsinr][column+1] <= (*curves[m])[sinr][column])
                                    break;
                            
                            totalDistance += (higherCRsinr - sinr);
                            numCompared++;
                        }
                    }

                    // set the entry in the lookup table
                    position[0] = sinr;
                    position[1] = cr;
                    position[2] = bl;

                    blerLUTs[m]->fill(position, (*curves[m])[sinr][column]);
                }
                
               
            }
            if (cr < shapes[m][1] - 1)
            {
                offsets[m][cr] = float(totalDistance)/float(numCompared)/10.;
 /*               std::cout << "For modulation=" << m  
                            << " distance from CR=" << keys[m][1][cr] 
                            << " to CR=" << keys[m][1][cr+1] 
                            << " is " << offsets[m][cr] << " dBs of SINR\n"; */
            }
        }
    }
    
    // init offsets used to shift BLER curves
    // this might be necessary to perfectly align link level results with 3GPP
    // calibration data
    
    unsigned int assumeNumPRBs = 20; // default
    wns::pyconfig::View imtaphyConfigView(wns::simulator::getInstance()->getConfiguration().getView("modules").getView("imtaphy"));
    
    if (imtaphyConfigView.knows("blerConfig") && !imtaphyConfigView.isNone("blerConfig"))
    {
        wns::pyconfig::View blerConfigView(imtaphyConfigView.get("blerConfig"));
        
        if (blerConfigView.knows("shiftQPSKdB") && !blerConfigView.isNone("shiftQPSKdB"))
        {
            sinrOffsetQPSK = wns::Ratio::from_dB(blerConfigView.get<double>("shiftQPSKdB"));
        }
        if (blerConfigView.knows("shiftQAM16dB") && !blerConfigView.isNone("shiftQAM16dB"))
        {
            sinrOffsetQAM16 = wns::Ratio::from_dB(blerConfigView.get<double>("shiftQAM16dB"));
        }
        if (blerConfigView.knows("shiftQAM64dB") && !blerConfigView.isNone("shiftQAM64dB"))
        {
            sinrOffsetQAM64 = wns::Ratio::from_dB(blerConfigView.get<double>("shiftQAM64dB"));
        }
        
        if (blerConfigView.knows("assumeNumPRBs") && !blerConfigView.isNone("assumeNumPRBs"))
        {
            assumeNumPRBs = blerConfigView.get<unsigned int>("assumeNumPRBs");
            assure((assumeNumPRBs >= 1) && (assumeNumPRBs < 100), "bad number of PRBs to assume");
        }        
    }
    
    // determine suitable thresholds to switch between CQIs
    unsigned int sinrResolution = 350;
    double minSINR = -10.0;
    double maxSINR = 25.0;
    boost::multi_array<double, 2> blers(boost::extents[sinrResolution][15]);
    
    for (unsigned int sinr = 0; sinr < sinrResolution; sinr++)
    {
        for (unsigned int cqi = 0; cqi < 15; cqi++)
        {
            imtaphy::l2s::ModulationScheme modulation = imtaphy::l2s::CQITable[cqi+1].modulation;
            double codeRate = imtaphy::l2s::CQITable[cqi+1].codeRate;
           
            unsigned int assumeBlocksize = static_cast<float>(((14-3)*12 - 6) * assumeNumPRBs * modulation.getBitsPerSymbol()) * codeRate;
            assumeBlocksize = std::max(40u, assumeBlocksize);
           
            blers[sinr][cqi] = getBlockErrorRate(wns::Ratio::from_dB(getSinr(sinr, sinrResolution, minSINR, maxSINR)),
                                                 modulation,
                                                 codeRate,
                                                 assumeBlocksize);

        }
    }
    
    std::vector<double> thresholdsFactor(16);
    
    sinrThresholds[0] = wns::Ratio::from_dB(-15.0);
    thresholdsFactor[0] = sinrThresholds[0].get_factor();
    
    for (unsigned int cqi = 0; cqi < 15; cqi++)
    {
        for (unsigned int sinr = 0; sinr < sinrResolution; sinr++)
        {
            if (blers[sinr][cqi] < 0.1)
            {
                wns::Ratio currentSINR = wns::Ratio::from_dB(getSinr(sinr, sinrResolution, minSINR, maxSINR));
                sinrThresholds[cqi+1] = currentSINR;
                thresholdsFactor[cqi+1] = sinrThresholds[cqi+1].get_factor();
                
//                std::cout << "CQI " << cqi + 1 << " best switching point at " << currentSINR << "\n";
                break;
            }
        }
    }
    
    std::vector<unsigned int> shape(1, 16);
    lut = new TableType(shape, std::vector<std::vector<double> >(1, thresholdsFactor), false);
    std::vector<unsigned int> lutPosition(1);
    for (unsigned int i = 0; i < 16; i++)
    {
        lutPosition[0] = i;
        lut->fill(lutPosition, i);
    }
   
}


double
BlockErrorModel::getBlockErrorRate(wns::Ratio sinr, 
                                imtaphy::l2s::ModulationScheme modulation, 
                                float codeRate, unsigned int blockSize) const
{
    // blockSize is the pure transport block size, have to add 24 CRC bits
    // see also  ModulationScheme::getEffectiveCodeRate
    if (blockSize + 24 > 6144) 
    { 
        // we have to consider code block segmentation

        // We assume that the effective SINR does not differ significantly between the code blocks
        // which is hopefully the case because the blocks are mapped to the time frequency-grid
        // in a frequency-first fashion, cf. the Farooq Khan (2009), p. 256

        // we have to drop the whole transport block if any of the code blocks is not decoded correctly
        // so totalBLER = 1 - \prod_i (1-BLER_i)

        // If the transport block has to be segmented, it is divided into multiple roughly identically
        // sized code blocks. This is to avoid bad performance of a very small remainder. The allowed
        // sizes depend on the turbo interleaver sizes. Here, we simply assume arbitrary equal sizes
        // because the interleaver sizes are quite fine granular.

        // For a reference, see Farooq Khan (2009) Section 11.4
       

        double bler;
        unsigned int numBlocks = ((blockSize + 24) + 6120 - 1) / 6120; // this is ceil

        // we assume all code blocks have identical sizes, for correct procedure see 36.212 5.1.2
        unsigned int segmentedBlockSize = (blockSize + 24*(1+numBlocks)) / numBlocks;
        assure(segmentedBlockSize <= 6144, "Code Block Segment is too big"); 
        
        double perCBsuccessProb = 1.0 - getBlockErrorRate(sinr, modulation, codeRate, segmentedBlockSize);
       
        bler = 1.0 - pow(perCBsuccessProb, static_cast<double>(numBlocks));
//         std::cout << "TBsize=" << blockSize << " gives " << numBlocks << " code blocks of size << " << segmentedBlockSize 
//                   << " at SINR=" << sinr << " with CR=" << codeRate << " they have indiv. BLER of " << 1.0-perCBsuccessProb << " gives total BLER=" << bler << "\n";
        return bler;
    }
    // else: small enough for single code block

    // transport block size + 24 transport block CRC
    // bler curves already consider 24 bit CRC
    float blockLength = static_cast<float>(blockSize);

    assure((codeRate > 0) && (codeRate <= 1), "Code rate bigger than 0 and <= 1 required");
    assure(blockSize > 0, "block size must be positive");

    unsigned int m = modulation.getBitsPerSymbol() / 2 - 1;
    assure((m >= 0) && (m <= 2), "unsupported modulation");

   
    
    double lookForCodeRate;
    wns::Ratio adjustedSINR;
    if (codeRate <= minCR[m])
    {
        // We can treat a lower code rate as if some bits of the with "minCodeRate" encoded bits
        // are repeated and thus their received power or SINRs are added up (in fact, this is basically what happens in LTE)
        wns::Ratio gain = wns::Ratio::from_factor(minCR[m] / codeRate);
        adjustedSINR = sinr + gain;

        lookForCodeRate = minCR[m];
    }
    else if (codeRate >= maxCR[m])
    {
        // TODO: (February 2012) this actually does not work as intended. We should provide more link level 
        // results (maybe also investigate why the are a bit shifted). However, during operation, 
        // this extrapolation is usually not needed so that it should not cause problems at the moment
        // this we could model as if some bits were punctured away, i.e., their power is lost
        wns::Ratio loss = wns::Ratio::from_factor(codeRate / maxCR[m]);
        adjustedSINR = sinr - loss;
        lookForCodeRate = maxCR[m];
    }
    else // codeRate is in the middle between two known ones
    {
        
        unsigned int crIndex;
        for (crIndex = 0; codeRate > keys[m][1][crIndex]; crIndex++)
            ;
        
        assure(crIndex > 0, "Must be greater because we previously check for that case");
        
        
        // Take the curve for the lower code rate but shift the sinr by the corresponding SINR
        // distance to the next bigger code rate
        double fraction = (codeRate - keys[m][1][crIndex-1]) / (keys[m][1][crIndex] - keys[m][1][crIndex-1]);
        
        assure((fraction >= 0) && (fraction <= 1), "something is messed up");

        if (fraction < 0.5)
        {
            lookForCodeRate = keys[m][1][crIndex-1];
            adjustedSINR = sinr - wns::Ratio::from_dB(offsets[m][crIndex-1] * fraction);
        }
        else
        {
            lookForCodeRate = keys[m][1][crIndex];
            adjustedSINR = sinr + wns::Ratio::from_dB(offsets[m][crIndex-1] * (1.0 - fraction));
        }

    }
    
    if (blockLength < minBL)
        blockLength = minBL;
    if (blockLength > maxBL)
        blockLength = maxBL;

    
    switch (m)
    {
        case 0:
            adjustedSINR += sinrOffsetQPSK;
            break;
        case 1:
            adjustedSINR += sinrOffsetQAM16;
            break;
        case 2:
            adjustedSINR += sinrOffsetQAM64;
            break;
    }
    
    
    // we could store all SINRs as factors, avoiding log/exp conversions
    if (adjustedSINR.get_dB() < minSINR)
        adjustedSINR = wns::Ratio::from_dB(minSINR);
    if (adjustedSINR.get_dB() > maxSINR)
        adjustedSINR = wns::Ratio::from_dB(maxSINR);

   
    
    std::vector<float> lookForKeys(3);
    lookForKeys[0] = adjustedSINR.get_dB();
    lookForKeys[1] = lookForCodeRate;
    lookForKeys[2] = blockLength;

    BLERLookUpTable::HyperCubeType lookupResult(imtaphy::detail::getResultShape<BLERLookUpTable::HyperCubeType, 3>());
    lookupResult = blerLUTs[m]->getHyperCube(lookForKeys);
    
    return tri.linear(lookupResult, lookForKeys);
}
