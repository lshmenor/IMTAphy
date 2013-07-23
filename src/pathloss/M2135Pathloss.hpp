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

#ifndef ITUINHPATHLOSS_HPP
#define ITUINHPATHLOSS_HPP

#include <IMTAPHY/pathloss/PathlossModelInterface.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/linkManagement/classifier/ITUClassifier.hpp>
#include <IMTAPHY/linkManagement/classifier/StaticClassifier.hpp>

namespace imtaphy { namespace pathloss {

        class M2135Pathloss :
            public PathlossModelInterface
        {
        public:

            M2135Pathloss(Channel* _channel, wns::pyconfig::View config);

            virtual
            void onWorldCreated();

            virtual 
            wns::Ratio getPathloss(imtaphy::Link* link, double frequencyMHz);


            wns::Ratio getUMaPathloss(imtaphy::Link* link, double frequencyMHz);
            wns::Ratio getUMiPathloss(imtaphy::Link* link, double frequencyMHz);
            wns::Ratio getRMaPathloss(imtaphy::Link* link, double frequencyMHz);
            wns::Ratio getSMaPathloss(imtaphy::Link* link, double frequencyMHz);
            wns::Ratio getInHPathloss(imtaphy::Link* link, double frequencyMHz);
        private:
            double getO2Vshadowing(double mean, double stdDev, wns::Position baseStation, wns::Position mobileStation);
        
            Channel* channel;
            double SMaBuildingHeight;
            double SMaStreetWidth;
            double RMaBuildingHeight;
            double RMaStreetWidth;
            double UMaBuildingHeight;
            double UMaStreetWidth;
            double feederLoss;
            imtaphy::linkclassify::ITUClassifier* ituClassifier;
            imtaphy::linkclassify::StaticClassifier* staticClassifer; // Needed for smallScale calibration

        };
    }}
#endif // ITUINHPATHLOSS_HPP
