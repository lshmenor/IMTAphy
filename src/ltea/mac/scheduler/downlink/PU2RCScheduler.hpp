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

#ifndef LTEA_MAC_SCHEDULER_DOWNLINK_PU2RCSCHEDULER_HPP
#define LTEA_MAC_SCHEDULER_DOWNLINK_PU2RCSCHEDULER_HPP

#include <IMTAPHY/ltea/mac/scheduler/downlink/SchedulerBase.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>
#include <IMTAPHY/detail/NodePtrCompare.hpp>
#include <IMTAPHY/link2System/BlockErrorModel.hpp>
#include <iomanip>

namespace ltea { namespace mac { namespace scheduler { namespace downlink {

    typedef struct Coordinate {
            Coordinate(unsigned int prb_, unsigned int column_) :
                prb(prb_),
                column(column_) {}
                
                bool operator<(const Coordinate& other) const
                {
                    return (prb < other.prb ||
                            ((prb == other.prb) && (column < other.column)));
                }
                            
            unsigned int prb;
            unsigned int column;
        } GridCoordinate;
    typedef std::set<GridCoordinate> GridCoordinateSet;

    class PU2RCGrid
    {
    public:
        PU2RCGrid(unsigned int numPRBs_) :
            numPRBs(numPRBs_),
            pmi(numPRBs, -1),
            numberOfUsers(numPRBs_, 0),
            sumMetric(numPRBs_, 0.0),
            users(boost::extents[numPRBs][4]),
            metric(boost::extents[numPRBs][4]),
            sinrOffsets(boost::extents[numPRBs][4])
        {
            reset();
        }
        
        void reset()
        {
            for (unsigned int prb = 0; prb < numPRBs; prb++)
            {
                pmi[prb] = -1;
                sumMetric[prb] = 0.0;
                numberOfUsers[prb] = 0;
                for (unsigned int column = 0; column < 4; column++)
                {
                    users[prb][column] = NULL;
                    metric[prb][column] = 0.0;
                    sinrOffsets[prb][column] = wns::Ratio::from_factor(1.0);
                }
            }
            
            perUserCoordinates.clear();
            perUserPRBs.clear();
        }
                
        void addEntry(unsigned int prb, unsigned int pmi_, unsigned int column, wns::node::Interface* user, double metric_, wns::Ratio sinrOffset)
        {
            assure(prb < numPRBs, "Invalid PRB");
            assure(column < 4, "Invalid column");
            assure(users[prb][column] == NULL, "Entry already filled");
            assure((pmi[prb] == -1) || (pmi[prb] == pmi_), "PMI was already set to " << pmi[prb]);
            assure(!userAlreadyScheduledOnPRB(user, prb), "User already scheduled into this row cannot add to column " << column);

            pmi[prb] = pmi_;
            users[prb][column] = user;
            metric[prb][column] = metric_;
            sumMetric[prb] += metric_;
            numberOfUsers[prb] += 1;
            
            perUserCoordinates[user].insert(GridCoordinate(prb, column));
            perUserPRBs[user].insert(prb);
            sinrOffsets[prb][column] = sinrOffset;
        }

        void updateEntry(unsigned int prb, unsigned int column, wns::node::Interface* user, double metric_, wns::Ratio sinrOffset)
        {
            assure(prb < numPRBs, "Invalid PRB");
            assure(column < 4, "Invalid column");
            assure(users[prb][column] != NULL, "Entry was empty");
            assure(!userAlreadyScheduledOnPRB(user, prb) || users[prb][column] == user, "User already scheduled to other slot");
                
            perUserCoordinates[users[prb][column]].erase(GridCoordinate(prb, column));
            perUserPRBs[users[prb][column]].erase(prb);
            
            users[prb][column] = user;
            sumMetric[prb] -= metric[prb][column];
            metric[prb][column] = metric_;
            sumMetric[prb] += metric_;
            
            perUserCoordinates[user].insert(GridCoordinate(prb, column));
            perUserPRBs[user].insert(prb);
            sinrOffsets[prb][column] = sinrOffset;
        }
        
        void deleteEntry(unsigned int prb, unsigned int column, wns::node::Interface* user)
        {
            assure(prb < numPRBs, "Invalid PRB");
            assure(column < 4, "Invalid column");
            assure(users[prb][column] == user, "User inconsistent: have " << users[prb][column]->getName() << " but want to delete " << user->getName());
            
            perUserCoordinates[user].erase(GridCoordinate(prb, column));
            perUserPRBs[user].erase(prb);
            sumMetric[prb] -= metric[prb][column];
            metric[prb][column] = 0;
            users[prb][column] = NULL;
            numberOfUsers[prb] -= 1;
            
            sinrOffsets[prb][column] = wns::Ratio::from_factor(1.0);
            
            if (numberOfUsers[prb] == 0)
                pmi[prb] = -1;
            
        }
        
        void removeUser(wns::node::Interface* user)
        {
            GridCoordinateSet resources = perUserCoordinates[user];
            for (GridCoordinateSet::iterator iter = resources.begin(); iter != resources.end(); iter++)
            {
                deleteEntry(iter->prb, iter->column, user);
            }
            
            assure(perUserPRBs[user].size() == 0, "PRBs left after deleting");
            assure(perUserCoordinates[user].size() == 0, "Resources left after deleting");
        }
        
        unsigned int getNumPRBsPerUser(wns::node::Interface* user)
        {
            return perUserCoordinates[user].size();
        }
        
        bool userAlreadyScheduledOnPRB(wns::node::Interface* user, unsigned int prb)
        {
            assure(prb < numPRBs, "Invalid PRB");

            if (perUserPRBs[user].find(prb) == perUserPRBs[user].end())
                return false;
            else
                return true;
        }
        
        unsigned int getPMI(unsigned int prb)
        {
            assure(prb < numPRBs, "Invalid PRB");
            assure(pmi[prb] != -1, "PMI was not yet set");
            
            return pmi[prb];
        }
        
        double getMetric(unsigned int prb, unsigned int column)
        {
            assure(prb < numPRBs, "Invalid PRB");
            assure(column < 4, "Invalid column");

            return metric[prb][column];
        }
        
        wns::node::Interface* getUser(unsigned int prb, unsigned int column)
        {
            assure(prb < numPRBs, "Invalid PRB");
            assure(column < 4, "Invalid column");
            assure(users[prb][column] != NULL, "No user yet allocated to this resource");
            
            return users[prb][column];
            
        }
        
        unsigned int getNumAlreadyAllocatedResources(unsigned int prb)
        {
            assure(prb < numPRBs, "Invalid PRB");

            return numberOfUsers[prb];
        }
        
        bool resourceFree(unsigned int prb, unsigned int column)
        {
            assure(prb < numPRBs, "Invalid PRB");
            assure(column < 4, "Invalid column");
            
            return (users[prb][column] == NULL);
        }
        
        friend std::ostream& operator <<(std::ostream &str, const PU2RCGrid& grid)
        {
            str << "PRB\tPMI\tColumn1\t\t\tColumn2\t\t\tColumn3\t\t\tColumn4\t\t\tMetric\n";
            
            
            // http://stackoverflow.com/questions/2436004/how-do-i-correctly-organize-output-into-columns
            for (unsigned int prb = 0; prb < grid.numPRBs; prb++)
            {
                str << prb << "\t" << grid.pmi[prb] << "\t";
                for (unsigned int column = 0; column < 4; column++)
                {
                    if (grid.users[prb][column] == NULL)
                    {
                        str << "free\t\t\t";
                    }
                    else
                    {
                        str << grid.users[prb][column]->getName() << "\t" << std::setw(8) << grid.sinrOffsets[prb][column].get_dB() << "\t";
                    }
                }
                str << grid.sumMetric[prb] << "\n";
            }
            
            for (std::map<wns::node::Interface*, GridCoordinateSet >::const_iterator iter = grid.perUserCoordinates.begin(); iter != grid.perUserCoordinates.end(); iter++)
            {
                str << "\nUser " << iter->first->getName() << ":\n";
                
                if (iter->second.size() == 0)
                    continue;
                
                for (std::set<GridCoordinate>::iterator cIter = iter->second.begin(); cIter != iter->second.end(); cIter++)
                {
                    str << "(" << cIter->prb << ", " << cIter->column << "), ";
                }
                str << "\n";
            }
            
            return str;
        }
        
        void setSINROffset(wns::node::Interface* user, unsigned int prb, unsigned int column, wns::Ratio offset)
        {
            assure(prb < numPRBs, "Invalid PRB");
            assure(column < 4, "Invalid column");
            assure(users[prb][column] == user, "Different user scheduled on requested resource");
            
            sinrOffsets[prb][column] = offset;
        }
        
        wns::Ratio getSINROffset(wns::node::Interface* user, unsigned int prb, unsigned int column)
        {
            assure(prb < numPRBs, "Invalid PRB");
            assure(column < 4, "Invalid column");
            assure(users[prb][column] != NULL, "No user scheduled on requested resource");
            
            return sinrOffsets[prb][column];
        }
        
        
        const GridCoordinateSet& getUserGridCoordinates(wns::node::Interface* user)
        {
            // the user might not be included in the map but an empty set will be created when this is calld
            
            return perUserCoordinates[user];
        }
        
    private:
        
        unsigned int numPRBs;
        std::vector<int> pmi;
        std::vector<unsigned int> numberOfUsers;
        std::vector<double> sumMetric;
        boost::multi_array<wns::node::Interface*, 2> users;
        boost::multi_array<double, 2> metric;
        std::map<wns::node::Interface*, std::set<unsigned int> > perUserPRBs;
        boost::multi_array<wns::Ratio, 2> sinrOffsets;
        std::map<wns::node::Interface*, GridCoordinateSet> perUserCoordinates;
    };    
    
    
    typedef std::vector<wns::node::Interface*> UserVector;
    typedef std::vector<double> MetricVector;
    

    class PMIGroup
    {
    public:
        PMIGroup() :
            sumMetric(0),
            pmi(0),
            users(4, static_cast<wns::node::Interface*>(NULL)),
            userMetric(4, 0)
            {}
            
        double sumMetric;
        unsigned int pmi;
        UserVector users;
        MetricVector userMetric;
    };
    
    // TODO: we might want to replace the multimaps with priority_queues for the rankings
    
    typedef std::multimap<double, PMIGroup> PMIGroupRanking;
    typedef std::multimap<double, wns::node::Interface*> DoubleNodeMultiMap;
    typedef std::vector<DoubleNodeMultiMap> DoubleNodeMultiMapVector;
        
    typedef struct {
        wns::node::Interface* user;
        double metric;
    } UserAndMetric;
    typedef std::vector<UserAndMetric> CandidateVector;

    enum EstimateOther {
        NoEstimation,
        PerfectEstimation,
        InnerProduct
    };
    
    class PU2RCScheduler :
        public SchedulerBase,
        public wns::Cloneable<PU2RCScheduler>
    {
        
    public:
        PU2RCScheduler(wns::ldk::fun::FUN*, const wns::pyconfig::View&);

        
    protected:
        void initScheduler();
        void doScheduling();
        
        PMIGroupRanking rankGroups(ltea::mac::scheduler::UserSet& userSet, imtaphy::interface::PRB prb);
        
        imtaphy::receivers::LteRel8Codebook<float>* codebook;
        
        std::map<wns::node::Interface*, double, imtaphy::detail::WnsNodeInterfacePtrCompare> throughputHistory;
        double alpha;
        
        void doLinkAdapationAndRegisterTransmissions();
        
    private:
        // feedback helper functions
        bool userFits(wns::node::Interface* user, unsigned int prb, unsigned int pmi, unsigned int column);
        double getMetric(wns::node::Interface* user, unsigned int prb, unsigned int pmi, unsigned int column);

        imtaphy::receivers::CodebookColumn getPreferredCodebookColumn(wns::node::Interface* user, unsigned int prb);
                
        PMIGroup findBestCombination(DoubleNodeMultiMapVector& preliminaryResult, unsigned int pmi);

        bool uniqueUsers(wns::node::Interface* user1, wns::node::Interface* user2, wns::node::Interface* user3, wns::node::Interface* user4)
        {
            if ((((user1 != user2) && (user1 != user3) && (user1 != user4)) || (user1 == NULL)) &&
                (((user2 != user3) && (user2 != user4)) || (user2 == NULL)) &&
                (((user3 != user4) || (user3 == NULL))))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        wns::Ratio estimateSINROffset(wns::node::Interface* user, unsigned int prb, unsigned int pmi, unsigned int column);
        
        void updatePU2RCFeedback(ltea::mac::scheduler::UserSet& allUsers);
        imtaphy::l2s::BlockErrorModel* blerModel;
        wns::probe::bus::ContextCollectorPtr groupSizeContextCollector;
        wns::probe::bus::ContextCollectorPtr fillLevelContextCollector;
        wns::probe::bus::ContextCollectorPtr imperfectTransmissionRatioCollector;
        wns::probe::bus::ContextCollectorPtr imperfectReransmissionRatioCollector;
        
        
        PU2RCGrid* grid;
        unsigned int numPRBs;
        
        unsigned int numIndices;
        std::map<wns::node::Interface*, std::vector<unsigned int> > userToIndexLookup;
        std::map<wns::node::Interface*, boost::multi_array<wns::Ratio, 3>* > usersSINRs;
        
        boost::multi_array<wns::Ratio, 2> sinrLosses;
        
        std::vector<unsigned int> pmis;
        double historyExponent;
        bool fillGrid;
        EstimateOther estimateForNonPreferred;
        std::map<wns::node::Interface*, double> throughputThisTTI;

    };
}}}}


#endif 


