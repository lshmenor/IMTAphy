/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2012
 * Institute for Communication Networks (LKN)
 * Associate Institute for Signal Processing (MSV)
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
/* added by Andreas Dotzler - TUM - MSV - dotzler@tum.de -                    */

#ifndef LTEA_MAC_SCHEDULER_DOWNLINK_ZFSCHEDULER_HPP
#define LTEA_MAC_SCHEDULER_DOWNLINK_ZFSCHEDULER_HPP

#include <IMTAPHY/ltea/mac/scheduler/downlink/SchedulerBase.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>
#include <IMTAPHY/detail/NodePtrCompare.hpp>
#include <IMTAPHY/link2System/BlockErrorModel.hpp>
#include <IMTAPHY/receivers/feedback/PU2RCFeedbackManager.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>

namespace ltea { namespace mac { namespace scheduler { namespace downlink {
    class ZFGroup
    {
    public:
        ZFGroup(unsigned int prb_, unsigned int numTxAntennas_):
            compoundChannel(NULL),
            precodingMatrix(NULL),
            prb(prb_),
            numTxAntennas(numTxAntennas_),
            rank(0),
            metric(0)
        {};
        
        ~ZFGroup()
        {
            delete compoundChannel;
            delete precodingMatrix;
        }
        
        ZFGroup(const ZFGroup& other) :
            prb(other.prb),
            numTxAntennas(other.numTxAntennas),
            rank(other.rank),
            metric(other.metric),
            userPositions(other.userPositions),
            channels(other.channels),
            offsets(other.offsets),
            users(other.users)
        {
            if (other.compoundChannel)
            {
                this->compoundChannel = new imtaphy::detail::ComplexFloatMatrix(*other.compoundChannel);
            }
            else
            {
                this->compoundChannel = NULL;
            }
            if (other.precodingMatrix)
            {
                this->precodingMatrix = new imtaphy::detail::ComplexFloatMatrix(*other.precodingMatrix);
            }
            else
            {
                this->precodingMatrix = NULL;
            }

        }
        
        ZFGroup& operator= (const ZFGroup &other)
        {
            if (this != &other)
            {
                delete this->compoundChannel;
                delete this->precodingMatrix;

                
                if (other.compoundChannel)
                {
                    this->compoundChannel = new imtaphy::detail::ComplexFloatMatrix(*other.compoundChannel);
                }
                else
                {
                    this->compoundChannel = NULL;
                }
                if (other.precodingMatrix)
                {
                    this->precodingMatrix = new imtaphy::detail::ComplexFloatMatrix(*other.precodingMatrix);
                }
                else
                {
                    this->precodingMatrix = NULL;
                }
                
                this->userPositions = other.userPositions;
                this->channels = other.channels;
                this->offsets = other.offsets;
                this->users = other.users;
                
                this->prb = other.prb;
                this->numTxAntennas = other.numTxAntennas;
                this->rank = other.rank;
                this->metric = other.metric;
            }
            
        }
        
        void setUser(wns::node::Interface* user, unsigned int position, imtaphy::detail::ComplexFloatMatrixPtr channel)
        {
            assure(channels.size() == userPositions.size(), "Inconsistency");
            assure((userPositions.find(user) == userPositions.end()) ||
                   (userPositions[user] == position), "User already allocated to other position");
            
            if (position >= userPositions.size())
            {
                assure(userPositions.find(user) == userPositions.end(), "User " << user->getName() << " already stored at position " << userPositions[user]);
                assure(position == userPositions.size(), "Can only add one position at a time; current size is " << userPositions.size() << " cannot add at position " << position);
                
                channels.push_back(channel);
                userPositions[user] = position;
                users.push_back(user);
                
                delete precodingMatrix;
                delete compoundChannel;
                
                compoundChannel = new imtaphy::detail::ComplexFloatMatrix(position + 1, numTxAntennas);
                precodingMatrix = new imtaphy::detail::ComplexFloatMatrix(numTxAntennas, position + 1);
                
                // fill compound channel with all previous channel vectors
                for (unsigned int i = 0; i < position; i++)
                {
                    for (unsigned int s = 0; s < numTxAntennas; s++)
                    {
                        (*compoundChannel)[i][s] = conj((*channels[i])[s][0]* std::complex<float>(2.0, 0)); // scale for full power
                    }
                }
            }
            else
            {
                // modifying already allocated posistion: erase old user
                userPositions.erase(users[position]);
                offsets.erase(users[position]);
                // add new user
                users[position] = user;
                userPositions[user] = position;
                channels[position] = channel;
            }
            
            // update that column
            for (unsigned int s = 0; s < numTxAntennas; s++)
            {
                (*compoundChannel)[position][s] = conj((*channels[position])[s][0])* std::complex<float>(2.0, 0); // scale for full power;
            }
            
            // compute inverse
            if (userPositions.size() > 1)
            {   
                rank = imtaphy::detail::pseudoInverse(*compoundChannel, *precodingMatrix);            
            }
            else
            {   // no ZF for a single user, just use the channel (i.e. the feedback precoder)
                assure(position == 0, "Single user but position != 0");
                for (unsigned int s = 0; s < numTxAntennas; s++)
                {
                    (*precodingMatrix)[s][position] = (*channels[position])[s][0] * std::complex<float>(2.0, 0); // scale for full power
                }
                rank = 1;
            }
        }
 
        float getPrecoderNorm(wns::node::Interface* user)
        {
            assure(userPositions.find(user) != userPositions.end(), "Unknown user");
            unsigned int pos = userPositions[user];
            
            float norm = 0.0;
            for (unsigned int s = 0; s < numTxAntennas; s++)
            {
                norm += std::norm((*precodingMatrix)[s][pos]);
            }
            
            return sqrt(norm);
        }
 
        imtaphy::detail::ComplexFloatMatrixPtr getPrecoder(wns::node::Interface* user)
        {
            assure(userPositions.find(user) != userPositions.end(), "Unknown user");
            unsigned int pos = userPositions[user];
            
            imtaphy::detail::ComplexFloatMatrixPtr precoder = imtaphy::detail::ComplexFloatMatrixPtr(new imtaphy::detail::ComplexFloatMatrix(numTxAntennas, 1));
            for (unsigned int s = 0; s < numTxAntennas; s++)
            {
                (*precoder)[s][0] = (*precodingMatrix)[s][pos];
            }
            
            imtaphy::detail::scaleMatrixA(*precoder, 1.0f / getPrecoderNorm(user));
            
            return precoder;
        }
 
        unsigned int getRank() {return rank;}
 
        void setSINROffset(wns::node::Interface* user, wns::Ratio offset)
        {
            assure(userPositions.find(user) != userPositions.end(), "Unknown user");
            offsets[user] = offset;
        }
        
        wns::Ratio getSINROffset(wns::node::Interface* user)
        {
            assure(userPositions.find(user) != userPositions.end(), "Unknown user");
            
            return offsets[user];
        }
        
        std::vector<wns::node::Interface*> getUsers() {return users;}
        void setMetric(float metric_) {metric = metric_;}
        float getMetric() {return metric;}
        
        unsigned int getPRB() const {return prb;}
        
    private:
        imtaphy::detail::ComplexFloatMatrix* compoundChannel;
        imtaphy::detail::ComplexFloatMatrix* precodingMatrix;
        
        std::map<wns::node::Interface*, unsigned int> userPositions;
        std::vector<imtaphy::detail::ComplexFloatMatrixPtr> channels;
        std::vector<wns::node::Interface*> users;
        std::map<wns::node::Interface*, wns::Ratio> offsets;
        
        unsigned int prb;
        unsigned int numTxAntennas;
        
        unsigned int rank;
        float metric;
    };  

        
    class ZFScheduler :
        public SchedulerBase,
        public wns::Cloneable<ZFScheduler>
    {
        
    public:
        ZFScheduler(wns::ldk::fun::FUN*, const wns::pyconfig::View&);
        
    protected:
        void initScheduler();
        void doScheduling();
        
        
        imtaphy::receivers::LteRel8Codebook<float>* codebook;
        
        std::map<wns::node::Interface*, double, imtaphy::detail::WnsNodeInterfacePtrCompare> throughputHistory;
        double alpha;

        
    private:
        enum Feedback {
            PU2RC,
            Rank1
        };
        
        
        imtaphy::detail::ComplexFloatMatrixPtr getChannelVector(wns::node::Interface* user, unsigned int prb);
        unsigned int getChannelVectorId(wns::node::Interface* user, unsigned int prb);
        wns::Ratio getEstimatedSINR(wns::node::Interface* user, unsigned int prb);
        wns::Ratio getExactSINR(wns::node::Interface* user, unsigned int prb);
        wns::Ratio getSINRCorrection(wns::node::Interface* user, unsigned int prb);
        
        void updateZFFeedback(ltea::mac::scheduler::UserSet& allUsers);
        ZFGroup computeZFforPRB(ltea::mac::scheduler::UserSet newTransmissionUsers, unsigned int prb);
        imtaphy::l2s::BlockErrorModel* blerModel;     
        void computeSchedulingResult(ZFGroup best, std::map<wns::node::Interface*,SchedulingResult>* col_allocation);
   
        unsigned int numPRBs;  
        std::map<wns::node::Interface*,double> throughputThisTTI;
        void doLinkAdapationAndRegisterTransmissions();

        
        std::map<wns::node::Interface*,imtaphy::receivers::feedback::PU2RCFeedback*> pu2rcFeedback;
        std::map<wns::node::Interface*,imtaphy::receivers::feedback::LteRel8DownlinkFeedbackPtr> rel8Feedback;

        
        wns::probe::bus::ContextCollectorPtr groupSizeContextCollector;
        Feedback feedbackMode;
    };
}}}}


#endif 


