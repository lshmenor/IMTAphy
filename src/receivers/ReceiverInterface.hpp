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

#ifndef RECEIVERINTERFACE_HPP
#define RECEIVERINTERFACE_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>
#include <IMTAPHY/Transmission.hpp>
#include <IMTAPHY/StationPhy.hpp>

namespace imtaphy {

    class StationPhy;
    class Channel;
    
    namespace receivers {
        class ReceiverInterface
        {
        public:
            ReceiverInterface(StationPhy* station, const wns::pyconfig::View& pyConfigView) {};
                
            virtual void channelInitialized(Channel* channel) = 0;
                
            virtual void receive(TransmissionPtr transmission) = 0;
            virtual void deliverReception(TransmissionPtr transmission) = 0;
            
            virtual void onShutdown() = 0;
                
        };

        typedef imtaphy::StationModuleCreator<ReceiverInterface> ReceiverCreator;
        typedef wns::StaticFactory<ReceiverCreator> ReceiverFactory;

        class LinearReceiver;
        
        template <typename T, typename KIND = T>
        struct ReceiverModuleCreator:
                public ReceiverModuleCreator<KIND, KIND>
        {
            virtual KIND* create(LinearReceiver* receiver,
                                 const wns::pyconfig::View& config)
            {
                return new T(receiver, config);
            }
        };
        
        template <typename KIND>
        struct ReceiverModuleCreator<KIND, KIND>
        {
        public:
            virtual KIND* create(LinearReceiver*,
                                 const wns::pyconfig::View&) = 0;
                
            virtual ~ReceiverModuleCreator()
            {}
        };


        
    }}

#endif // RECEIVERINTERFACE_HPP
