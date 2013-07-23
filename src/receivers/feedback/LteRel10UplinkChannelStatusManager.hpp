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

#ifndef IMTAPHY_RECEIVERS_FEEDBACK_LTEREL10UPLINKCHANNELSTATUSMANAGER_HPP
#define IMTAPHY_RECEIVERS_FEEDBACK_LTEREL10UPLINKCHANNELSTATUSMANAGER_HPP


#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/node/Interface.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/receivers/LinearReceiver.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>

#include <IMTAPHY/link2System/MMIBeffectiveSINR.hpp>
#include <IMTAPHY/link2System/Modulations.hpp>
#include <IMTAPHY/receivers/feedback/LteFeedback.hpp>
#include <IMTAPHY/link2System/LteCQIs.hpp>

#include <WNS/Observer.hpp>
#include <IMTAPHY/interface/IMTAphyObserver.hpp>
#include <IMTAPHY/detail/NodePtrCompare.hpp>

namespace imtaphy { namespace receivers { namespace feedback { 
    
    class UplinkChannelStatusManagerInterface
    {
    public:
        virtual
        ~UplinkChannelStatusManagerInterface(){}
        
        virtual LteRel10UplinkChannelStatusPtr 
        getChannelState(wns::node::Interface* node, unsigned int ttiNumber) = 0;

        virtual void 
        updatePRBsForUserNow(wns::node::Interface* user, imtaphy::interface::PRBVector& prbs) = 0;

        virtual void 
        registerBS(imtaphy::StationPhy* baseStation, 
                        std::vector< imtaphy::StationPhy* >& associatedMobileStations, 
                        imtaphy::Channel* channel) = 0;

        virtual void 
        setReferencePerPRBTxPowerForUser(wns::node::Interface* node, wns::Power referencePower) = 0;

        static UplinkChannelStatusManagerInterface*
        getCSM();
    };
    
    class LteRel10UplinkChannelStatusManager :
        public UplinkChannelStatusManagerInterface,
        public wns::Observer<imtaphy::interface::IMTAphyObserver>
    {
        class LteRel10UplinkChannelStatusContainer
        {
        public:
            LteRel10UplinkChannelStatusContainer(wns::node::Interface* user, 
                                                 unsigned int numTotalPRBs_, 
                                                 std::vector<unsigned int> offsetFirstUpdate, 
                                                 unsigned int updatePeriod, 
                                                 unsigned int bufferLength_) :
                myUser(user),
                numTotalPRBs(numTotalPRBs_),
                nextUpdateAtTTI(offsetFirstUpdate),
                updateNow(numTotalPRBs, false),
                periodicity(updatePeriod),
                channelStatus(numTotalPRBs),
                bufferLength(bufferLength_)
            {
                assure(nextUpdateAtTTI.size() == numTotalPRBs, "Need to provide as many offsets as we have PRBs");
                assure(updatePeriod > 0, "update period cannot be zero");
                for (unsigned int i = 0; i < offsetFirstUpdate.size(); i++)
                    assure(offsetFirstUpdate[i] > 0, "TTIs start counting at 1 so offset (first TTI to update) has to be at least 1");
                assure(bufferLength > updatePeriod, "Buffer too small");
                
                for (unsigned int i = 0; i < bufferLength; i++)
                    ringBuffer.push_back(LteRel10UplinkChannelStatusPtr(new LteRel10UplinkChannelStatus(numTotalPRBs)));

            };
            

            virtual void updateChannelStatus(unsigned int prb, wns::Ratio sinr1, wns::Ratio sinr2, unsigned int pmi, unsigned int estimatedInTTI)
            {
                // basic version does not have averaging etc.
                channelStatus.pmi[prb] = pmi;
                channelStatus.sinrsTb1[prb] = sinr1;
                channelStatus.sinrsTb2[prb] = sinr2;
                channelStatus.estimatedInTTI[prb] = estimatedInTTI;
                
            }
            
            virtual void updateRank(unsigned int rank)
            {
                assure((rank > 0) && (rank <= 4), "Rank should be between 1 and 4");
                
                // advanced versions should invalidate pmi/sinr history when rank changes!?
            }
            
            LteRel10UplinkChannelStatusPtr getChannelState(int tti)
            {
                unsigned int index = ((tti % bufferLength) + bufferLength) % bufferLength; 
                return ringBuffer[index];
            }

            void updatePRBsNow(imtaphy::interface::PRBVector& prbs)
            {
                for (unsigned int f = 0; f < prbs.size(); f++)
                {
                    assure((prbs[f] >= 0) && (prbs[f] < numTotalPRBs), "Invalid PRB");
                    
                    updateNow[prbs[f]] = true;
                }
            }
            
            imtaphy::interface::PRBVector getPRBsToUpdate(unsigned int tti)
            {
                imtaphy::interface::PRBVector result;
                
                for (unsigned int prb = 0; prb < numTotalPRBs; prb++)
                {
/*                    #pragma omp critical
                    {
                    std::cout << "At TTI=" << tti << " updateNow[prb]=" << updateNow[prb] << " nextUpdateAtTTI[prb]=" << nextUpdateAtTTI[prb] << "\n"; 
                    }*/
                    if (updateNow[prb] || (nextUpdateAtTTI[prb] <= tti))
                    {
                        result.push_back(prb);
                    }
                }
                
                return result;
            }
            
            void ttiOver(unsigned int tti)
            {
                // store the current channel state estimates into the ringBuffer for the 
                // future. Some values might be overwritten before periodicity is over but
                // who cares

                for (unsigned int t = tti; t < tti + periodicity; t++)
                {
                    ringBuffer[t % bufferLength]->rank = channelStatus.rank;

                    for (unsigned int prb = 0; prb < numTotalPRBs; prb++)
                    {
                        ringBuffer[t % bufferLength]->pmi[prb] = channelStatus.pmi[prb];
                        ringBuffer[t % bufferLength]->sinrsTb1[prb] = channelStatus.sinrsTb1[prb];
                        ringBuffer[t % bufferLength]->sinrsTb2[prb] = channelStatus.sinrsTb2[prb];
                        ringBuffer[t % bufferLength]->estimatedInTTI[prb] = channelStatus.estimatedInTTI[prb];
                    }
                }
                
                for (unsigned int prb = 0; prb < numTotalPRBs; prb++)
                {
                    updateNow[prb] = false;
                    if (nextUpdateAtTTI[prb] == tti)
                    {
                        nextUpdateAtTTI[prb] += periodicity;
                    }                    
//                    std::cout << "TTI " << tti << " over, next update for PRB " << prb << " at " <<  nextUpdateAtTTI[prb] << "\n";
                }
            }
        private:
            wns::node::Interface* myUser;
            unsigned int numTotalPRBs;
            std::vector<unsigned int> nextUpdateAtTTI;
            std::vector<bool> updateNow;
            unsigned int periodicity;
            LteRel10UplinkChannelStatus channelStatus; // that's the most recent one
            
            // and that's the history
            std::vector<LteRel10UplinkChannelStatusPtr> ringBuffer;
            unsigned int bufferLength;
        };
    

        
    public:
        LteRel10UplinkChannelStatusManager(const wns::pyconfig::View& config);

        virtual void 
        registerBS(imtaphy::StationPhy* baseStation, 
                        std::vector< imtaphy::StationPhy* >& associatedMobileStations, 
                        imtaphy::Channel* channel);
        
        virtual void 
        setReferencePerPRBTxPowerForUser(wns::node::Interface* node, wns::Power referencePower);

        virtual LteRel10UplinkChannelStatusPtr 
        getChannelState(wns::node::Interface* node, unsigned int ttiNumber);
                        
        virtual void 
        updatePRBsForUserNow(wns::node::Interface* user, imtaphy::interface::PRBVector& prbs);
        
        // IMTAphy observer interface:
        void onNewTTI(unsigned int ttiNumber) {};
        void beforeTTIover(unsigned int ttiNumber);
        
    private:
        void doUpdate(imtaphy::StationPhy* receivingStation, unsigned int ttiNumber);
        unsigned int performRankUpdate(imtaphy::StationPhy* receivingStation);
        void determineSINRsAndCQIs(imtaphy::StationPhy* receivingStation, LteRel10UplinkChannelStatusPtr feedback, unsigned int rank);
        void determineNoPrecodingSINRs(imtaphy::StationPhy* baseStation, 
                                       imtaphy::StationPhy* userStation, 
                                       LteRel10UplinkChannelStatusContainer* channelStatus, 
                                       unsigned int rank,
                                       unsigned int tti);
        
        
        enum PrecodingModes{ ClosedLoopCodebookBased, NoPrecoding };

   
        wns::pyconfig::View config;
        unsigned int numPRBs;
        
        std::string precodingModeString;
        enum PrecodingModes precodingMode;
        
        unsigned int srsPeriod;
        unsigned int statusDelay;
        unsigned int bufferLength;
        std::vector<unsigned int> srsOffsets;
        
        bool initialized;
        imtaphy::receivers::LteRel8Codebook<float>* codebook;
        
 

        typedef std::vector<imtaphy::StationPhy*> StationVector;
        typedef std::map<imtaphy::StationPhy*, LteRel10UplinkChannelStatusContainer*, StationPhyPtrCompare> UserStatusMap;
        typedef std::map<imtaphy::StationPhy*, std::vector<imtaphy::StationPhy*>, StationPhyPtrCompare> StationStationVectorMap; 
        
        imtaphy::l2s::EffectiveSINRModelInterface* effSINRModel;
        std::map<imtaphy::StationPhy*, wns::Power, StationPhyPtrCompare> referencePowerMap;
        std::map<wns::node::Interface*, imtaphy::StationPhy*, imtaphy::detail::WnsNodeInterfacePtrCompare> node2StationLookup;
        
        std::map<imtaphy::StationPhy*, imtaphy::receivers::LinearReceiver*, StationPhyPtrCompare> receivers;
        
        UserStatusMap usersChannelStates;
        StationVector baseStations;
        StationStationVectorMap associatedUsers;
        bool enabled;
        
        wns::logger::Logger logger;
    };
}}}


#endif 


