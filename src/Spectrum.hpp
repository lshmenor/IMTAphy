/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2010
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

#ifndef IMTAPHY_SPECTRUM_HPP
#define IMTAPHY_SPECTRUM_HPP

#include <WNS/pyconfig/View.hpp>
#include <IMTAPHY/interface/TransmissionStatus.hpp>

namespace imtaphy {
    
    typedef enum {
        Downlink = 0,
        Uplink = 1
    } Direction;
    
    class Spectrum 
    {
    public:
        Spectrum(const wns::pyconfig::View& pyConfig);
        
        // for unit test use:
        Spectrum(double centerFreq, double prbBW, unsigned int numDLPRBs, unsigned int numULPRBs);

        // system-wide parameters
        imtaphy::interface::PRB getNumberOfPRBs(Direction direction) const;
        double getSystemCenterFrequencyHz(Direction direction) const;
        double getSystemCenterFrequencyWavelenghtMeters(Direction direction) const;
                        
        // per PRB 
        double getPRBcenterFrequencyHz(Direction direction, imtaphy::interface::PRB prb) const;
        double getPRBbandWidthHz() const;
        
        
    private:
        void setPRBfrequencies();
        
        std::vector<double> systemCenterFrequency;
        double prbBandwidth;
 
        std::vector<std::vector<double> > centerFrequencyPerPRB;
        std::vector<imtaphy::interface::PRB> numberOfPRBs;
    };    
}

#endif

