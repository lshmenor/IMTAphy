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

#include <IMTAPHY/ltea/mac/scheduler/downlink/PU2RCScheduler.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <IMTAPHY/receivers/feedback/PU2RCFeedbackManager.hpp>
#include <algorithm>
#include <fstream>
#include <IMTAPHY/antenna/LinearAntennaArray.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ltea::mac::scheduler::downlink::PU2RCScheduler,
    wns::ldk::FunctionalUnit,
    "ltea.mac.scheduler.downlink.PU2RCScheduler",
    wns::ldk::FUNConfigCreator);


using namespace ltea::mac::scheduler::downlink;

PU2RCScheduler::PU2RCScheduler(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
    SchedulerBase(fun, config),
    codebook(&(wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance())),
    alpha(config.get<double>("throughputSmoothing")),
    blerModel(imtaphy::l2s::TheLTEBlockErrorModel::getInstance()),
    historyExponent(config.get<double>("historyExponent")),
    fillGrid(config.get<bool>("fillGrid")),
    sinrLosses(boost::extents[64][64]) // big enough
{
    assure((alpha > 0.0) && (alpha <= 1.0), "Exponential smothing factor must be 0 < alpha <= 1 where smaller values lead to longer averaging memory");

    wns::probe::bus::ContextProviderCollection localcpc(layer->getNode()->getContextProviderCollection());
    groupSizeContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "groupSize"));
    
    fillLevelContextCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "initialFillLevel"));
    imperfectTransmissionRatioCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "imperfectTransmissionRatio"));
    imperfectReransmissionRatioCollector = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localcpc, "imperfectRetransmissionRatio"));
    
    std::string estimateOther = config.get<std::string>("estimateOther");
    std::transform(estimateOther.begin(), estimateOther.end(), estimateOther.begin(), toupper);

    if (estimateOther == "PERFECT")
    {
        estimateForNonPreferred = PerfectEstimation;
    }
    else if (estimateOther == "INNERPRODUCT")
    {
        estimateForNonPreferred = InnerProduct;
    }
    else if (estimateOther == "NO")
    {
        estimateForNonPreferred = NoEstimation;
    }
    else
    {
        assure(0, "Have to choose one of the available methods to treat non-preferred precoders");
    }
    
    
}


void 
PU2RCScheduler::initScheduler()
{
    pdcchLength = 3;
    provideRel10DMRS = false;
    numRel10CSIrsSets = 0;

    assure(syncHARQ == false, "PU2RCScheduler only supports asynchronous/adaptive HARQ operation, i.e., with HARQ retransmissions controlled by the PU2RC scheduler");

    // Init the throughput history for exponential averaging
    for (unsigned int i = 0; i < allUsers.size(); i++)
    {
        throughputHistory[allUsers[i]] = 1.0;
    }

    assure(numTxAntennas == 4, "Only works with 4 antennas and not with " << numTxAntennas);

    numPRBs = channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink);
    grid = new PU2RCGrid(numPRBs);
    
    numIndices = 20; // TODO: dynamic/config
    
    imtaphy::receivers::feedback::PU2RCFeedbackManager* pu2rcFeedbackManager = dynamic_cast<imtaphy::receivers::feedback::PU2RCFeedbackManager*>(feedbackManager);
    assure(pu2rcFeedbackManager, "Need a PU2RC feedback manager");
    
    pmis = pu2rcFeedbackManager->getPMIs();
    
    for (unsigned int pmi1 = 0; pmi1 < pmis.size(); pmi1++)
        for (unsigned int column1 = 0; column1 < 4; column1++)
            for (unsigned pmi2 = 0; pmi2 < pmis.size(); pmi2++)
                for (unsigned int column2 = 0; column2 < 4; column2++)
                {
                    imtaphy::detail::ComplexFloatMatrixPtr vector1 = codebook->get4TxCodebookColumn(pmis[pmi1], column1);
                    imtaphy::detail::ComplexFloatMatrixPtr vector2 = codebook->get4TxCodebookColumn(pmis[pmi2], column2);
                
                    assure((vector1->getRows() == 4) && (vector2->getRows() == 4), "both should have 4 rows");
                    std::complex<double> complexSum(0.0, 0.0);
                    for (unsigned int i = 0; i < 4; i++)
                    {
                        complexSum += (*vector1)[i][0] * std::conj((*vector2)[i][0]);
                    }
                    
                    double absInnerProduct = std::abs(complexSum);
                    assure((absInnerProduct >= 0.0) && (absInnerProduct <= 0.25), "Precoders are assumed to be unit length; normalized to 1/4 power");
                    
                    absInnerProduct*= 4.0;
                        
                    // avoid -inf as the offset
                    absInnerProduct = std::max(absInnerProduct, 0.01);
                        

                    sinrLosses[codebook->getIndex(pmis[pmi1], column1)][codebook->getIndex(pmis[pmi2], column2)] = wns::Ratio::from_factor(absInnerProduct);   
                }
    
}

PMIGroupRanking
PU2RCScheduler::rankGroups(ltea::mac::scheduler::UserSet& userSet, imtaphy::interface::PRB prb)
{
    PMIGroupRanking result;
    
    // we iterate over all available PMIs, compute per PMI the best group of users with this PMI and its sumMetric
    // the caller of this routine can then pick the best group
    for (unsigned int i = 0; i < pmis.size(); i++)
    {
        unsigned int pmi = pmis[i];
        PMIGroup group;
        group.pmi = pmi;
        
        // per PMI we can simply pick the best user for each column
        // only if there is not a single PMI/CI feedback per user (i.e. if there are multiple PMIs/CIs possible for a user)
        // we might not find the optimum with this greedy approach. Otherwise (single feedback of best PMI/CI per user) 
        // this gives an optimum grouping (also assuming no inter PRB dependencies due to scheduling constraints like retransmissions or
        // other than full buffer traffic
        
        DoubleNodeMultiMapVector perColumnRankings(4);
        
        for (unsigned int column = 0; column < 4; column++)
        {
            for(ltea::mac::scheduler::UserSet::const_iterator iter = userSet.begin(); iter!= userSet.end(); iter++)
            {
                wns::node::Interface* thisUser = *iter;
                
                if (userFits(thisUser, prb, pmi, column))
                {
                    perColumnRankings[column].insert(std::make_pair<double, wns::node::Interface*>(getMetric(thisUser, prb, pmi, column), thisUser));
                }
            }
            if (perColumnRankings[column].size() == 0)
            {
                group.users[column] = NULL;
            }
            else
            {
                group.users[column] = perColumnRankings[column].rbegin()->second;
                group.userMetric[column] = perColumnRankings[column].rbegin()->first;
                group.sumMetric += group.userMetric[column];
            }
        }
        
        if (uniqueUsers(group.users[0], group.users[1], group.users[2], group.users[3]))
        {
            // no users scheduled twice so this is indeed the optimal result
            // save the best group for this PMI into the ranking of PMIs
            result.insert(std::make_pair<double, PMIGroup>(group.sumMetric, group));
            MESSAGE_SINGLE(NORMAL, logger, "Greedy approach found optimal solution on PRB " << prb << " with PMI " << pmi << " with metric " << group.sumMetric);
            MESSAGE_SINGLE(NORMAL, logger, "Grouping with PMI " << group.pmi << " and sum metric = " << group.sumMetric << "\n"
                                            << "Column 1: " << (group.users[0] ? group.users[0]->getName() : "free") << " with metric=" << group.userMetric[0] << "\n"
                                            << "Column 2: " << (group.users[1] ? group.users[1]->getName() : "free") << " with metric=" << group.userMetric[1] << "\n"
                                            << "Column 3: " << (group.users[2] ? group.users[2]->getName() : "free") << " with metric=" << group.userMetric[2] << "\n"
                                            << "Column 4: " << (group.users[3] ? group.users[3]->getName() : "free") << " with metric=" << group.userMetric[3]);

        }
        else
        {
            // if a user got scheduled twice, we have to find the best combination
            MESSAGE_SINGLE(NORMAL, logger, "Greedy approach found invalid solution on PRB " << prb << " with PMI " << pmi << " with metric " << group.sumMetric);
            MESSAGE_SINGLE(NORMAL, logger, "Grouping with PMI " << group.pmi << " and sum metric = " << group.sumMetric << "\n"
                                            << "Column 1: " << (group.users[0] ? group.users[0]->getName() : "free") << " with metric=" << group.userMetric[0] << "\n"
                                            << "Column 2: " << (group.users[1] ? group.users[1]->getName() : "free") << " with metric=" << group.userMetric[1] << "\n"
                                            << "Column 3: " << (group.users[2] ? group.users[2]->getName() : "free") << " with metric=" << group.userMetric[2] << "\n"
                                            << "Column 4: " << (group.users[3] ? group.users[3]->getName() : "free") << " with metric=" << group.userMetric[3]);

            PMIGroup bestCombination = findBestCombination(perColumnRankings, pmi);
            result.insert(std::make_pair<double, PMIGroup>(bestCombination.sumMetric, bestCombination));
            
            MESSAGE_SINGLE(NORMAL, logger, "Exhaustive search corrected solution on PRB " << prb << " with PMI " << pmi << " resulting in metric " << bestCombination.sumMetric);
            MESSAGE_SINGLE(NORMAL, logger, "Grouping with PMI " << bestCombination.pmi << " and sum metric = " << bestCombination.sumMetric << "\n"
                                            << "Column 1: " << (bestCombination.users[0] ? bestCombination.users[0]->getName() : "free") << " with metric=" << bestCombination.userMetric[0] << "\n"
                                            << "Column 2: " << (bestCombination.users[1] ? bestCombination.users[1]->getName() : "free") << " with metric=" << bestCombination.userMetric[1] << "\n"
                                            << "Column 3: " << (bestCombination.users[2] ? bestCombination.users[2]->getName() : "free") << " with metric=" << bestCombination.userMetric[2] << "\n"
                                            << "Column 4: " << (bestCombination.users[3] ? bestCombination.users[3]->getName() : "free") << " with metric=" << bestCombination.userMetric[3]);

//             std::cout << "Grouping with PMI " << bestCombination.pmi << " and sum metric = " << bestCombination.sumMetric << "\n"
//                                             << "Column 1" << (bestCombination.users[0] ? bestCombination.users[0]->getName() : "free") << " with metric=" << bestCombination.userMetric[0] << "\n"
//                                             << "Column 2" << (bestCombination.users[1] ? bestCombination.users[1]->getName() : "free") << " with metric=" << bestCombination.userMetric[1] << "\n"
//                                             << "Column 3" << (bestCombination.users[2] ? bestCombination.users[2]->getName() : "free") << " with metric=" << bestCombination.userMetric[2] << "\n"
//                                             << "Column 4" << (bestCombination.users[3] ? bestCombination.users[3]->getName() : "free") << " with metric=" << bestCombination.userMetric[3] << "\n" << std::flush;

        }
    }
    
    return result;    
}




PMIGroup
PU2RCScheduler::findBestCombination(DoubleNodeMultiMapVector& preliminaryResult, unsigned int pmi)
{
    assure(preliminaryResult.size() == 4, "Should get 4 multimaps in the preliminaryResult vector");


    
    // the preliminaryResult contains the ranking of users per columns but the algorithm detected a user
    // to be scheduled twice per PMI. We now find the best combination by trying all possible combinations.
    // Note that we can restrict the exhaustive search to the 4 best users per column. Together with NULL
    // user, the maximum number of combinations to generate is thus 5^4 = 625
    
    unsigned int counter = 0;
    
    UserAndMetric nullEntry;
    nullEntry.metric = 0;
    nullEntry.user = NULL;
    
    // allow NULL user by adding to the end of candidates (as 5th entry)
    CandidateVector candidatesColumn1;
    for (DoubleNodeMultiMap::const_reverse_iterator iter = preliminaryResult[0].rbegin(); 
         (iter != preliminaryResult[0].rend()) && (counter < 4); 
         iter++, counter++)
    {
//        std::cout << counter +1 << ". candidate on column 1 is: " << iter->second->getName() << "\n";
        UserAndMetric entry;
        entry.user = iter->second;
        entry.metric = iter->first;
        candidatesColumn1.push_back(entry);
    }
    candidatesColumn1.push_back(nullEntry);
    
    counter = 0;
    CandidateVector candidatesColumn2;
    for (DoubleNodeMultiMap::const_reverse_iterator iter = preliminaryResult[1].rbegin(); 
         (iter != preliminaryResult[1].rend()) && (counter < 4); 
         iter++, counter++)
    {
//        std::cout << counter +1 << ". candidate on column 2 is: " << iter->second->getName() << "\n";

        UserAndMetric entry;
        entry.user = iter->second;
        entry.metric = iter->first;
        candidatesColumn2.push_back(entry);
    }
    candidatesColumn2.push_back(nullEntry);
    
    counter = 0;
    CandidateVector candidatesColumn3;
    for (DoubleNodeMultiMap::const_reverse_iterator iter = preliminaryResult[2].rbegin(); 
         (iter != preliminaryResult[2].rend()) && (counter < 4); 
         iter++, counter++)
    {
//                std::cout << counter +1 << ". candidate on column 3 is: " << iter->second->getName() << "\n";

        UserAndMetric entry;
        entry.user = iter->second;
        entry.metric = iter->first;
        candidatesColumn3.push_back(entry);
    }
    candidatesColumn3.push_back(nullEntry);

    counter = 0;
    CandidateVector candidatesColumn4;
    for (DoubleNodeMultiMap::const_reverse_iterator iter = preliminaryResult[3].rbegin(); 
         (iter != preliminaryResult[3].rend()) && (counter < 4); 
         iter++, counter++)
    {
//                std::cout << counter +1 << ". candidate on column 4 is: " << iter->second->getName() << "\n";

        
        UserAndMetric entry;
        entry.user = iter->second;
        entry.metric = iter->first;
        candidatesColumn4.push_back(entry);
    }
    candidatesColumn4.push_back(nullEntry);

    double bestMetric = -1; // don't choose -1 because without feedback best metric will be 0
    std::vector<CandidateVector::const_iterator> bestCombination(4);
    for (CandidateVector::const_iterator cand1 = candidatesColumn1.begin(); cand1 != candidatesColumn1.end(); cand1++)
    {
        for (CandidateVector::const_iterator cand2 = candidatesColumn2.begin(); cand2 != candidatesColumn2.end(); cand2++)
        {
            for (CandidateVector::const_iterator cand3 = candidatesColumn3.begin(); cand3 != candidatesColumn3.end(); cand3++)
            {
                for (CandidateVector::const_iterator cand4 = candidatesColumn4.begin(); cand4 != candidatesColumn4.end(); cand4++)
                {
                    if (uniqueUsers(cand1->user, cand2->user, cand3->user, cand4->user))
                    {
                        // this is a valid combination
                        double sumMetric = cand1->metric + cand2->metric + cand3->metric + cand4->metric;
                        if (sumMetric > bestMetric)
                        {
                            bestMetric = sumMetric;
                            bestCombination[0] = cand1;
                            bestCombination[1] = cand2;
                            bestCombination[2] = cand3;
                            bestCombination[3] = cand4;
                        }
                    }
                }
            }
        }
    }
    PMIGroup result;   
    if (bestMetric < 0)
    {
        // if we could not find a legal combination, return a valid combination by removing duplicate users
        for (unsigned int column = 0; column < 4; column++)
        {
            result.userMetric[column] = preliminaryResult[column].begin()->first;
            result.users[column] = preliminaryResult[column].begin()->second;
            for (unsigned int otherColumn = 0; otherColumn < column; otherColumn++)
            {
                // if this user is duplicate, just keep the best entry
                if (result.users[otherColumn] == result.users[column])
                {
                    if (result.userMetric[otherColumn] > result.userMetric[column])
                    {
                        result.users[column] = NULL;
                        result.userMetric[column] = 0;
                    }
                    else
                    {
                        result.users[otherColumn] = NULL;
                        result.userMetric[otherColumn] = 0;
                    }
                }
            }
        }
        
        result.sumMetric = 0;
        for (unsigned int column = 0; column < 4; column++)
        {
            result.sumMetric += result.userMetric[column];
        }
        
    }
    else
    {
        result.sumMetric = bestMetric;
        for (unsigned int column = 0; column < 4; column++)
        {
//            std::cout << "Best user on column " << column << " is " << (bestCombination[column]->user ? bestCombination[column]->user->getName() : "none")<< " with metric " << bestCombination[column]->metric << "\n" << std::flush;
            result.userMetric[column] = bestCombination[column]->metric;
            result.users[column] = bestCombination[column]->user;
        }
    }
    
    // finally set the pmi for the result
    result.pmi = pmi;
    
    return result;
}

void 
PU2RCScheduler::doScheduling()
{
    unsigned int numImperfectRetransmissions = 0;
    unsigned int numImperfectTransmissions = 0;
    
    throughputThisTTI.clear();
    
    grid->reset();
    
    ltea::mac::scheduler::UserSet retransmissionUsers = harq->getUsersWithRetransmissions();
    
    typedef std::map<wns::node::Interface*, unsigned int, imtaphy::detail::WnsNodeInterfacePtrCompare> UserIntMap;
    typedef std::map<wns::node::Interface*, wns::ldk::CompoundPtr, imtaphy::detail::WnsNodeInterfacePtrCompare> UserCompoundMap;

    UserIntMap retransmissionSizes;
    UserCompoundMap retransmissions;
    
    for (ltea::mac::scheduler::UserSet::const_iterator iter = retransmissionUsers.begin(); iter != retransmissionUsers.end(); iter++)
    {
        wns::node::Interface* user = *iter;
        
        unsigned int processId = harq->getProcessWithNextRetransmissions(user);
        assure(harq->hasRetransmission(user, processId, 0), "Users is supposed to have retransmission but not for the first TB");
        
        wns::ldk::CompoundPtr retransmission = harq->getRetransmission(user, processId, 0);
        
        assure(dciReader->commandIsActivated(retransmission->getCommandPool()), 
                    "Retransmissions are expected to already have their DCI information stored from the first transmission attempt");

        ltea::mac::DownlinkControlInformation* dci = dciReader->readCommand<ltea::mac::DownlinkControlInformation>(retransmission->getCommandPool());
        
        unsigned int numPRBs = dci->local.prbPowerPrecoders.size();
        
        retransmissionSizes[user] = numPRBs;
        retransmissions[user] = retransmission;
    }
        
    ltea::mac::scheduler::UserSet allUsers(retransmissionUsers.begin(), retransmissionUsers.end());
    ltea::mac::scheduler::UserSet newTransmissionUsers;

    ltea::mac::scheduler::UserSet activeUsers = usersPRBManager.getActiveUsers(); //ltea::mac::scheduler::UsersPRBManager
    for (ltea::mac::scheduler::UserSet::const_iterator iter = activeUsers.begin(); iter != activeUsers.end(); iter++)
    {
        if (retransmissionUsers.find(*iter) == retransmissionUsers.end())
        {
            // not a retransmission user
            allUsers.insert(*iter);
            newTransmissionUsers.insert(*iter);
        }
    }

    // no point in doing anything else if there are no users at all
    if (allUsers.size() == 0)
        return;

    
    updatePU2RCFeedback(allUsers);
    
    
    unsigned int numInitialResources = 0;
    for (unsigned int prb = 0; prb < numPRBs; prb++)
    {
        PMIGroupRanking ranking = rankGroups(allUsers, prb);
        
        if (ranking.size() == 0)
            continue;
        
        // ranking is a std::multimap that is inheritely sorted from smallest metric to largest; 
        // "rbegin" returns the reverse iterator pointing to the element with the highest metric
        
        PMIGroup bestGroup = ranking.rbegin()->second;
        
        assure(bestGroup.userMetric.size() == 4, "Invalid number of user metrics in group");
        assure(bestGroup.users.size() == 4, "Invalid number of users in group");
        
        for (unsigned int column = 0; column < 4; column++)
        {
            if (bestGroup.users[column] != NULL)
            {
                assure(userFits(bestGroup.users[column], prb, bestGroup.pmi, column), "Invalid user selected");

                wns::Ratio sinrOffset = estimateSINROffset(bestGroup.users[column], prb, bestGroup.pmi, column);
                grid->addEntry(prb, bestGroup.pmi, column, bestGroup.users[column], bestGroup.userMetric[column], sinrOffset);
                numInitialResources++;
            }
        }
        
        // if no user at all was selected (e.g. because no feedback is available at startup), force someone in
        if (grid->getNumAlreadyAllocatedResources(prb) == 0)
        {
            numImperfectTransmissions++;
            wns::Ratio sinrOffset = estimateSINROffset(*(allUsers.begin()), prb, pmis[0], 0);
            grid->addEntry(prb, pmis[0], 0, *(allUsers.begin()), 0, sinrOffset);
        }
    }
    
    // log how much of the grid could be filled by the initial scheduling routine (i.e., how uch has to be fixed
    fillLevelContextCollector->put(static_cast<double>(numInitialResources)/static_cast<double>(4*numPRBs));

    
    ltea::mac::scheduler::UserSet scheduledNewTransmissions;
    for (ltea::mac::scheduler::UserSet::iterator iter = newTransmissionUsers.begin(); iter != newTransmissionUsers.end(); iter++)
    {
        if (grid->getNumPRBsPerUser(*iter) > 0)
        {
            scheduledNewTransmissions.insert(*iter);
        };
    }
    
    MESSAGE_SINGLE(NORMAL, logger, "Initial scheduling done. Grid:\n" << *grid);
    
    // fix grid: 
    // - make retransmissions of right sizes (prbs)
    // - then fill emtpy spots

    ltea::mac::scheduler::UserSet retransmissionsTooManyPRBs;
    ltea::mac::scheduler::UserSet retransmissionsTooFewPRBs;
    
    for (std::map<wns::node::Interface*, unsigned int>::iterator iter = retransmissionSizes.begin(); iter != retransmissionSizes.end(); iter++)
    {
        MESSAGE_SINGLE(NORMAL, logger, "User " << iter->first->getName() << " needs a retransmission with " << retransmissionSizes[iter->first] 
                                    << " PRBs, currently has " << grid->getNumPRBsPerUser(iter->first));

        if (grid->getNumPRBsPerUser(iter->first) > iter->second)
        {
            retransmissionsTooManyPRBs.insert(iter->first);
        }
        else if (grid->getNumPRBsPerUser(iter->first) < iter->second)
        {
            retransmissionsTooFewPRBs.insert(iter->first);
        }
    }
    
    // first check if we can give any of the too many PRBs from a retransmission to one of the retransmissions that has too few
    ltea::mac::scheduler::UserSet retransmissionsTooManyPRBsBeforeUpdate = retransmissionsTooManyPRBs;
    for (ltea::mac::scheduler::UserSet::iterator iterTooMany = retransmissionsTooManyPRBsBeforeUpdate.begin(); iterTooMany != retransmissionsTooManyPRBsBeforeUpdate.end(); iterTooMany++)
    {
        // no point to loop through coordinates if no candidates
        if (retransmissionsTooFewPRBs.size() == 0)
            break;
        
        // TODO: we could add a ranking here
        GridCoordinateSet initalCoordinates = grid->getUserGridCoordinates(*iterTooMany);
        for (GridCoordinateSet::iterator coordIter = initalCoordinates.begin(); coordIter != initalCoordinates.end(); coordIter++)
        {
            for (ltea::mac::scheduler::UserSet::iterator iterTooFew = retransmissionsTooFewPRBs.begin(); iterTooFew != retransmissionsTooFewPRBs.end(); iterTooFew++)
            {
                //if (userToIndexLookup[*iterTooFew][coordIter->prb] == codebook->getIndex(grid->getPMI(coordIter->prb), coordIter->column))
                if (userFits(*iterTooFew, coordIter->prb, grid->getPMI(coordIter->prb), coordIter->column) &&
                    !grid->userAlreadyScheduledOnPRB(*iterTooFew, coordIter->prb))
                {
                    wns::Ratio sinrOffset = estimateSINROffset(*iterTooFew, coordIter->prb, grid->getPMI(coordIter->prb), coordIter->column);
                    grid->updateEntry(coordIter->prb, coordIter->column, *iterTooFew, getMetric(*iterTooFew, coordIter->prb, grid->getPMI(coordIter->prb), coordIter->column), sinrOffset);
                    
                    if (retransmissionSizes[*iterTooFew] == grid->getNumPRBsPerUser(*iterTooFew))
                    {
                        retransmissionsTooFewPRBs.erase(*iterTooFew);
                    }
                    break; // break out of loop over too few users
                }
            }
            if (retransmissionSizes[*iterTooMany] == grid->getNumPRBsPerUser(*iterTooMany))
            {
                retransmissionsTooManyPRBs.erase(*iterTooMany);
                break; // break out of loop over grid coordinates
            }
        }
    }
    
    MESSAGE_SINGLE(NORMAL, logger, "Grid after transfering PRBs from rich to poor retransmission:\n" << *grid);
    
    retransmissionsTooManyPRBsBeforeUpdate = retransmissionsTooManyPRBs;
    for (ltea::mac::scheduler::UserSet::iterator iterTooMany = retransmissionsTooManyPRBsBeforeUpdate.begin(); iterTooMany != retransmissionsTooManyPRBsBeforeUpdate.end(); iterTooMany++)    
    {
        // TODO: we could add a ranking here:
        // - avoids always adding the same users
        // - allows picking best substitutes
        
        GridCoordinateSet initalCoordinates = grid->getUserGridCoordinates(*iterTooMany);
        for (GridCoordinateSet::iterator coordIter = initalCoordinates.begin(); coordIter != initalCoordinates.end(); coordIter++)
        {
            // only consider users that have already been scheduled
//            for (ltea::mac::scheduler::UserSet::iterator newUserIter = newTransmissionUsers.begin(); newUserIter != newTransmissionUsers.end(); newUserIter++)
           for (ltea::mac::scheduler::UserSet::iterator newUserIter = scheduledNewTransmissions.begin(); newUserIter != scheduledNewTransmissions.end(); newUserIter++)
            {   
                // if (userToIndexLookup[*newUserIter][coordIter->prb] == codebook->getIndex(grid->getPMI(coordIter->prb), coordIter->column))
                if (userFits(*newUserIter, coordIter->prb, grid->getPMI(coordIter->prb), coordIter->column) &&
                    !grid->userAlreadyScheduledOnPRB(*newUserIter, coordIter->prb))
                {
                    wns::Ratio sinrOffset = estimateSINROffset(*newUserIter, coordIter->prb, grid->getPMI(coordIter->prb), coordIter->column);
                    grid->updateEntry(coordIter->prb, coordIter->column, *newUserIter, getMetric(*newUserIter, coordIter->prb, grid->getPMI(coordIter->prb), coordIter->column), sinrOffset);
                }
            }
            
            
            
            if (retransmissionSizes[*iterTooMany] == grid->getNumPRBsPerUser(*iterTooMany))
            {
                retransmissionsTooManyPRBs.erase(*iterTooMany);
                break; // break out of loop over grid coordinates
            }
        }
        
        if (retransmissionsTooManyPRBs.find(*iterTooMany) != retransmissionsTooManyPRBs.end())
        {
            while (retransmissionSizes[*iterTooMany] != grid->getNumPRBsPerUser(*iterTooMany))
            {
                // finding perfect matches was not enough, now forcibly remove some
                grid->deleteEntry(grid->getUserGridCoordinates(*iterTooMany).begin()->prb,
                                  grid->getUserGridCoordinates(*iterTooMany).begin()->column, *iterTooMany);
            }
            retransmissionsTooManyPRBs.erase(*iterTooMany);
        }
    }
    
    MESSAGE_SINGLE(NORMAL, logger, "Grid after reducing too big retransmission to their desired size:\n" << *grid);
   
    assure(retransmissionsTooManyPRBs.size() == 0, "There should be no retransmission with too many PRBs allocated");
    
    // rank the retransmissions with too few PRBs by the total number of PRBs they need and start with the biggest one
    // the reason is that we don't, e.g., 4 small retransmissions to block one PRB which is needed for a retransmission over all PRBs 
    std::multimap<unsigned int, wns::node::Interface*> retransmissionsTooFewPRBsBySize;
    for (ltea::mac::scheduler::UserSet::iterator iterTooFew = retransmissionsTooFewPRBs.begin(); iterTooFew != retransmissionsTooFewPRBs.end(); iterTooFew++)
    {
        retransmissionsTooFewPRBsBySize.insert(std::make_pair<unsigned int, wns::node::Interface*>(retransmissionSizes[*iterTooFew], *iterTooFew));
    }
    
    for (std::multimap<unsigned int, wns::node::Interface*>::reverse_iterator iterTooFew = retransmissionsTooFewPRBsBySize.rbegin(); iterTooFew != retransmissionsTooFewPRBsBySize.rend(); iterTooFew++)
    {
        wns::node::Interface* user = iterTooFew->second;
        
        MESSAGE_SINGLE(NORMAL, logger, "Taking care of user " << (user)->getName() << " who currently has not enough retransmissions." << *grid);
        // search and add PRBs to get retransmissions to desired size
        
        // first search for PRBs that have free space where the retransmission is not yet scheduled
        for (unsigned int prb = 0; prb < numPRBs; prb++)
        {
            if (grid->getNumAlreadyAllocatedResources(prb) != 0)
            {
                if ((grid->getNumAlreadyAllocatedResources(prb) < 4) && !grid->userAlreadyScheduledOnPRB(user, prb))
                {
                    for (unsigned int column = 0; column < 4; column++)
                    {
                        if (grid->resourceFree(prb, column))
                        {
                            numImperfectRetransmissions++;
                            wns::Ratio sinrOffset = wns::Ratio::from_dB(-42); // mark this retransmission as being imperfect 
                            grid->addEntry(prb, grid->getPMI(prb), column, user, 0, sinrOffset);

                            break;
                        }
                    }
                }
            }
            else // this row was free, so assign desired pmi/ci
            {
                // imtaphy::receivers::CodebookColumn preferred =  *(codebook->getCodebookColumns(userToIndexLookup[user][prb]).begin());
                imtaphy::receivers::CodebookColumn preferred = getPreferredCodebookColumn(user, prb);
                grid->addEntry(prb, preferred.pmi, preferred.column, user, 0, wns::Ratio::from_factor(1.0));
            }
            
            if (retransmissionSizes[user] == grid->getNumPRBsPerUser(user))
            {
                retransmissionsTooFewPRBs.erase(user);
                break;  // found enough free space for this retransmission
            }
        }
        
        if (retransmissionSizes[user] != grid->getNumPRBsPerUser(user))
        {
            MESSAGE_SINGLE(NORMAL, logger, "Have to force some one out");
            // we could not find enough free space for retransmission so we have to force some 
            // users out
            
            // start with first PRB, make sure the PRB is not already given to this retransmission
            for (unsigned int prb = 0; prb < numPRBs; prb++)
            {
                if (!grid->userAlreadyScheduledOnPRB(user, prb))
                {
                    // retransmission user not yet scheduled here, so kick one of the 4 users 
                    // but make sure it is not also a retransmission

                    // build ranking of users so that we can kick the worst one in this row
                    std::multimap<double, unsigned int> ranking;
                    for (unsigned int column = 0; column < 4; column++)
                    {
                        ranking.insert(std::make_pair<double, unsigned int>(grid->getMetric(prb, column), column));
                    }
                    for (std::multimap<double, unsigned int>::const_iterator kickIter = ranking.begin(); kickIter != ranking.end(); kickIter++)
                    {
                        // kick this one of it is not a retransmission or if it is a retransmission that we have not 
                        // fixed yet (remember: we start with the biggest and move on to the smaller retransmissions)
                        if ((retransmissions.find(grid->getUser(prb, kickIter->second)) == retransmissions.end()) ||
                            (retransmissionsTooFewPRBs.find(grid->getUser(prb, kickIter->second)) != retransmissionsTooFewPRBs.end()))
                        {
                            // this is not a retransmission, so kick this one
                            numImperfectRetransmissions++;
                            wns::Ratio sinrOffset = estimateSINROffset(user, prb, grid->getPMI(prb), kickIter->second); 
                            grid->updateEntry(prb, kickIter->second, user, 0, sinrOffset);
                            break;
                        }
                    }
                }

                if (retransmissionSizes[user] == grid->getNumPRBsPerUser(user))
                {
                    retransmissionsTooFewPRBs.erase(user);
                    break;  // kicked enough users to make space for this retransmission
                }
            }
        } 
    } // end of loop over retransmissions

    MESSAGE_SINGLE(NORMAL, logger, "Grid after increasing too small retransmission:\n" << *grid);

    // we could not fit all retransmission in, so delete some, i.e., postpone them for future TTI
    for (ltea::mac::scheduler::UserSet::iterator iter = retransmissionsTooFewPRBs.begin(); iter != retransmissionsTooFewPRBs.end(); iter++)
    {
        MESSAGE_SINGLE(NORMAL, logger, "Retransmission to user " << (*iter)->getName() << " left. Needs " << retransmissionSizes[*iter] << " but has only " << grid->getNumPRBsPerUser(*iter) << ". Deleting retransmission");
        grid->removeUser(*iter);
        
        retransmissionSizes.erase(*iter);
        retransmissions.erase(*iter);
        retransmissionsTooFewPRBs.erase(*iter);
    }
    
    MESSAGE_SINGLE(NORMAL, logger, "Grid after deleting retransmission which did not fit in:\n" << *grid);
    
    // now we should have fitted all retransmissions into the grid. now let's fill all empty spots
    // to assure the unitary property for each transmission. 
    
    if (fillGrid)
    {
    
        // we begin by checking for empty rows which can occur if too many retransmissions were removed before
        for (unsigned int prb = 0; prb < numPRBs; prb++)
        {
            if (grid->getNumAlreadyAllocatedResources(prb) == 0)
            {
                
                
                PMIGroupRanking ranking = rankGroups(scheduledNewTransmissions, prb);
                
                if (ranking.size() == 0)
                {
                    // if we could not get a group of already scheduled users, try all users without retransmissions
                    ranking = rankGroups(newTransmissionUsers, prb);
                }   
                if (ranking.size() == 0)
                {
                    MESSAGE_SINGLE(NORMAL, logger, "Unable to fill empty row, cannot find any non-retransmission user for PRB " << prb);
                    continue;
                }
                
                PMIGroup bestGroup = ranking.rbegin()->second;
                
                assure(bestGroup.userMetric.size() == 4, "Invalid number of user metrics in group");
                assure(bestGroup.users.size() == 4, "Invalid number of users in group");
            
                for (unsigned int column = 0; column < 4; column++)
                {
                    if (bestGroup.users[column] != NULL)
                    {
                        assure(userFits(bestGroup.users[column], prb, bestGroup.pmi, column), "Invalid user selected");
                        wns::Ratio sinrOffset = estimateSINROffset(bestGroup.users[column], prb, bestGroup.pmi, column);
                        grid->addEntry(prb, bestGroup.pmi, column, bestGroup.users[column], bestGroup.userMetric[column], sinrOffset);
                    }
                }
                
            }
        }
        
        MESSAGE_SINGLE(NORMAL, logger, "Grid after fixing empty rows:\n" << *grid);
        
        
        // now, fill all remaining holes
        for (unsigned int prb = 0; prb < numPRBs; prb++)
        {
            // this PRB is already full or we were not able to fill the empty row above, no point in trying againg
            if ((grid->getNumAlreadyAllocatedResources(prb) == 4) ||
                (grid->getNumAlreadyAllocatedResources(prb) == 0))
                continue;
            
            for (unsigned int column = 0; column < 4; column++)
            {
                // skip the already taken spots
                if (!grid->resourceFree(prb, column))
                    continue;

                // when we reach this point, we have users in the queue and an empty spot in this PRB
                // check which users potentially fit here and rank them according to some metric 
                // (i.e., figure out who is least worst off)
                    
                std::multimap<double, wns::node::Interface*> ranking;
                for (UserSet::const_iterator iter = newTransmissionUsers.begin(); iter != newTransmissionUsers.end(); iter++)
                {
                    if (grid->userAlreadyScheduledOnPRB(*iter, prb))
                        continue;

                    double bonus = 0;
                    if (scheduledNewTransmissions.find(*iter) != scheduledNewTransmissions.end())
                    {
                        // add some bonus to users who are already scheduled so that their 
                        // allocations get increased. Avoids adding new users with small allocations (e.g. just one PRB)
                        bonus = 1000.0;
                    }
                    
                    ranking.insert(std::make_pair<double, wns::node::Interface*>(bonus + getMetric(*iter, prb, grid->getPMI(prb), column), *iter));
                }
                
                // now put the best user into the empty slot - hopefully we've found someone
                if (ranking.size() > 0)
                {
                    numImperfectTransmissions++;
                    wns::Ratio sinrOffset = estimateSINROffset(ranking.rbegin()->second, prb, grid->getPMI(prb), column);
                    grid->addEntry(prb, grid->getPMI(prb), column, ranking.rbegin()->second, ranking.rbegin()->first, sinrOffset);
                }
                
                
            }
        } // end over all prbs to fill emtpy spots
    }
    
    MESSAGE_SINGLE(NORMAL, logger, "The complete grid:\n" << *grid);
    
    // now, all retransmissions should be satisfied, all empty spots should be filled and we can 
    // start to build retransmission and new transmission objects from the grid
    
    unsigned int numRetransmissionResources = 0;
    for (UserCompoundMap::iterator iter = retransmissions.begin(); iter != retransmissions.end(); iter++)
    {
        wns::node::Interface* user = iter->first;
        
        ltea::mac::DownlinkControlInformation* dci = dciReader->readCommand<ltea::mac::DownlinkControlInformation>(iter->second->getCommandPool());
        
        assure(grid->getNumPRBsPerUser(user) == dci->local.prbPowerPrecoders.size(), "Retransmission has wrong number of PRBs allocated");

        // the old and new allocations are supposed to have the same size (#PRBs), so we can iterate in lockstep through
        // the previous and new resource allocation and update
        
        imtaphy::interface::PrbPowerPrecodingMap newPRBPowerPrecoding;
        
        // clear the old tracing dict because it contains potentially wrong PRBs
        dci->magic.prbTracingDict.clear();
        
        imtaphy::interface::PrbPowerPrecodingMap::iterator resourceIter = dci->local.prbPowerPrecoders.begin();
        GridCoordinateSet::iterator coordinateIter = grid->getUserGridCoordinates(user).begin();
        for (; resourceIter != dci->local.prbPowerPrecoders.end(); resourceIter++, coordinateIter++)
        {
            ltea::mac::SchedulingTracingDict tracing;
            tracing["TBsize"] = grid->getNumPRBsPerUser(user); // number of PRBs
            tracing["CI"] = coordinateIter->column;
            tracing["PMI"] = grid->getPMI(coordinateIter->prb);
            tracing["SINRoff"] = grid->getSINROffset(user, coordinateIter->prb, coordinateIter->column).get_dB();
            tracing["Metric"] = grid->getMetric(coordinateIter->prb, coordinateIter->column);
            
            dci->magic.prbTracingDict[coordinateIter->prb] = tracing;
            numRetransmissionResources++;
            newPRBPowerPrecoding[coordinateIter->prb].power = resourceIter->second.power;
            newPRBPowerPrecoding[coordinateIter->prb].precoding = codebook->get4TxCodebookColumn(grid->getPMI(coordinateIter->prb), coordinateIter->column);
        }

        dci->local.prbPowerPrecoders = newPRBPowerPrecoding;
        
        txService->registerTransmission(user,
                                        std::vector<wns::ldk::CompoundPtr>(1, iter->second),
                                        1, // number of layers
                                        dci->local.prbPowerPrecoders
                                        );
        harq->retransmissionStarted(user, dci->peer.processID, 0);
    }
        
    unsigned int numNewTransmissionResources = 0;
    for (ltea::mac::scheduler::UserSet::iterator iter = newTransmissionUsers.begin(); iter != newTransmissionUsers.end(); iter++)        
    {
        wns::node::Interface* user = *iter;

        assure(retransmissionUsers.find(user) == retransmissionUsers.end(), "New transmission should not be to a retransmission user");
        
 
        const GridCoordinateSet allocatedResources = grid->getUserGridCoordinates(user);

        if (allocatedResources.size() == 0)
            continue;
        
        // new transmission
        
        SchedulingResult allocation;
        allocation.scheduledUser = user;
        allocation.rank = 1;
        
        imtaphy::receivers::feedback::PU2RCFeedback* pu2rcFeedback = dynamic_cast<imtaphy::receivers::feedback::PU2RCFeedback*>(feedbackManager->getFeedback(user, scheduleForTTI).get());
        assure(pu2rcFeedback, "No PU2RCFeedback");

        for (GridCoordinateSet::iterator coordinateIter = allocatedResources.begin(); coordinateIter != allocatedResources.end(); coordinateIter++)
        {
            numNewTransmissionResources++;
            
            ltea::mac::SchedulingTracingDict tracing;
            tracing["TBsize"] = allocatedResources.size(); // number of PRBs
            tracing["CI"] = coordinateIter->column;
            tracing["PMI"] = grid->getPMI(coordinateIter->prb);
            tracing["SINRoff"] = grid->getSINROffset(user, coordinateIter->prb, coordinateIter->column).get_dB();
            tracing["Metric"] = grid->getMetric(coordinateIter->prb, coordinateIter->column);
            
            
            allocation.prbPowerPrecodingMap[coordinateIter->prb].power = txPowerdBmPerPRB;
            allocation.prbPowerPrecodingMap[coordinateIter->prb].precoding = codebook->get4TxCodebookColumn(grid->getPMI(coordinateIter->prb), coordinateIter->column);
            allocation.prbPowerOffsetForLA[coordinateIter->prb] = grid->getSINROffset(user, coordinateIter->prb, coordinateIter->column);
            
//             std::cout << "grid->getSINROffset=" << allocation.prbPowerOffsetForLA[coordinateIter->prb] 
//                       << " esimateOffset=" << estimateSINROffset(user, coordinateIter->prb, grid->getPMI(coordinateIter->prb), coordinateIter->column) << "\n";
            
            // for the sinr estimation probing
            // could be more elegant
            
            imtaphy::receivers::CodebookColumn bestVector = getPreferredCodebookColumn(user, coordinateIter->prb); 
            
            tracing["PerfSINRest"] = (*(usersSINRs[user]))[coordinateIter->prb][grid->getPMI(coordinateIter->prb)][coordinateIter->column].get_dB();
            tracing["best+offset"] = ((*(usersSINRs[user]))[coordinateIter->prb][bestVector.pmi][bestVector.column] +  allocation.prbPowerOffsetForLA[coordinateIter->prb]).get_dB();

            
            // the magic SINR is based on unquantized estimation from the receiver
            pu2rcFeedback->magicSINRs[coordinateIter->prb][0] = (*(usersSINRs[user]))[coordinateIter->prb][bestVector.pmi][bestVector.column] +  allocation.prbPowerOffsetForLA[coordinateIter->prb];
    
                      
            allocation.prbAndTracingInfo[coordinateIter->prb] = tracing;
                      
        }
        
        scheduledUsers.push_back(allocation);
    }
    
    if (numRetransmissionResources > 0)
    {
        imperfectReransmissionRatioCollector->put(static_cast<double>(numImperfectRetransmissions) / static_cast<double>(numRetransmissionResources));
    }
    if (numNewTransmissionResources > 0)
    {
        imperfectTransmissionRatioCollector->put(static_cast<double>(numImperfectTransmissions) / static_cast<double>(numNewTransmissionResources));
    }
    
    MESSAGE_SINGLE(NORMAL, logger, "Grid fill level after fixing is " << static_cast<double>(numRetransmissionResources + numNewTransmissionResources) / static_cast<double>(numPRBs * 4));
    
    MESSAGE_SINGLE(NORMAL, logger, "Retransmission imperfection: " << numImperfectRetransmissions << " out of " << numRetransmissionResources);
    MESSAGE_SINGLE(NORMAL, logger, "Transmission imperfection: " << numImperfectTransmissions << " out of " << numNewTransmissionResources);
    
    
    // when scheduling is done, let the base class perform the Link Adaptation and register the transmissions
    doLinkAdapationAndRegisterTransmissions();

    // update based on the final link adaptation decisions
    for (ltea::mac::scheduler::UserSet::const_iterator iter = allUsers.begin(); iter != allUsers.end(); iter++)
    {
        throughputHistory[*iter] = (1.0 - alpha) * throughputHistory[*iter] + alpha * throughputThisTTI[*iter];
        
        MESSAGE_SINGLE(NORMAL, logger, "Updating user " << (*iter)->getName() << "'s throughput by current throughput of "
                                    << throughputThisTTI[*iter] << " to epx. average of " << throughputHistory[*iter] << "\n");
    }

    
}


void 
PU2RCScheduler::updatePU2RCFeedback(ltea::mac::scheduler::UserSet& allUsers)
{
    userToIndexLookup.clear();
        
    for(ltea::mac::scheduler::UserSet::const_iterator iter = allUsers.begin(); iter!= allUsers.end(); iter++)
    {
        wns::node::Interface* thisUser = *iter;
        imtaphy::receivers::feedback::PU2RCFeedback* pu2rcFeedback = dynamic_cast<imtaphy::receivers::feedback::PU2RCFeedback*>(feedbackManager->getFeedback(thisUser, scheduleForTTI).get());
        assure(pu2rcFeedback, "No PU2RCFeedback");
        
        usersSINRs[thisUser] =  &(pu2rcFeedback->sinrMatrix);
        
        
        // TODO: this metric cache should go away and the estimateExpectedThroughput could be done more nicely as well
        
        userToIndexLookup[thisUser] = std::vector<unsigned int>(numPRBs);
        for (unsigned int prb = 0; prb < numPRBs; prb++)
        {
            
            unsigned int index = codebook->getIndex(pu2rcFeedback->pmi[prb], pu2rcFeedback->columnIndicator[prb]);
            userToIndexLookup[thisUser][prb] = index;
        }
    }
}

inline
bool
PU2RCScheduler::userFits(wns::node::Interface* user, unsigned int prb, unsigned int pmi, unsigned int column)
{
    // if we do not operation in some kind of estimation mode, we cannot schedule a user into
    // a resource that was not returned as the best choice
    if (estimateForNonPreferred == NoEstimation)
    {
        if ((userToIndexLookup[user][prb] == codebook->getIndex(pmi, column)))
        {
            return true;
        }
        else 
        {
            return false;
        }
    }
    else
    {
        return true;
    }
}

inline
imtaphy::receivers::CodebookColumn 
PU2RCScheduler::getPreferredCodebookColumn(wns::node::Interface* user, unsigned int prb)
{
    return *(codebook->getCodebookColumns(userToIndexLookup[user][prb]).begin());
}

inline
double 
PU2RCScheduler::getMetric(wns::node::Interface* user, unsigned int prb, unsigned int pmi, unsigned int column)
{
    imtaphy::receivers::feedback::PU2RCFeedback* pu2rcFeedback = dynamic_cast<imtaphy::receivers::feedback::PU2RCFeedback*>(feedbackManager->getFeedback(user, scheduleForTTI).get());
    assure(pu2rcFeedback, "No PU2RCFeedback");

    wns::Ratio expectedSINR = blerModel->getSINRthreshold(pu2rcFeedback->cqiTb1[prb]);
    
    
    // depending on the mode (see estimateSINROffset routine), we apply a correction to the  quantized SINR that we got from feedback
    wns::Ratio estimatedOffset = estimateSINROffset(user, prb, pmi, column);
    expectedSINR += estimatedOffset;
    
    // limit the SINR to the useful range
    if (expectedSINR > wns::Ratio::from_dB(22))
    {
        expectedSINR = wns::Ratio::from_dB(22);
    }
    double expectedThroughput = log2(1.0 + expectedSINR.get_factor());
    
    return expectedThroughput / pow(throughputHistory[user], historyExponent);
}

wns::Ratio 
PU2RCScheduler::estimateSINROffset(wns::node::Interface* user, unsigned int prb, unsigned int pmi, unsigned int column)
{
    if (userToIndexLookup[user][prb] == codebook->getIndex(pmi, column))
    {
        // perfect match, no offset
        return wns::Ratio::from_factor(1.0);
    }
    else
    {
        switch (estimateForNonPreferred)
        {
            case PerfectEstimation:
            {
                // this is just the difference between the best and the other
                // the SINR for the best will still come from the quantized feedback, just the difference is perfect
                imtaphy::receivers::CodebookColumn bestVector = getPreferredCodebookColumn(user, prb); 
                return (*(usersSINRs[user]))[prb][pmi][column] - (*(usersSINRs[user]))[prb][bestVector.pmi][bestVector.column];   
                break;
            }
            case InnerProduct:
            {
                // entries in sinrLosses should be negative (in dB)
                imtaphy::receivers::CodebookColumn bestVectorIds = getPreferredCodebookColumn(user, prb); 
                return sinrLosses[codebook->getIndex(bestVectorIds.pmi, bestVectorIds.column)][codebook->getIndex(pmi, column)];
                break;
            }
            case NoEstimation:
            {
                return wns::Ratio::from_dB(-10);
                break;
            }
            default:
                assure(0, "Invalid mode");
        }
    }
}

void 
PU2RCScheduler::doLinkAdapationAndRegisterTransmissions()
{
#ifndef WNS_NDEBUG 
    std::vector<std::vector<wns::Ratio> > patterns;
#endif

    
    for (std::list<SchedulingResult>::iterator iter = scheduledUsers.begin(); iter != scheduledUsers.end(); iter++)
    {
        std::vector<wns::ldk::CompoundPtr> transportBlocks; 
        transportBlocks.clear();
        
        unsigned int numPRBs = iter->prbPowerPrecodingMap.size();
        
        double user_rate = 0.0;
        
        assure(iter->rank == 1, "Only suports rank-1 transmission");
        
        rankContextCollector->put(iter->rank);

        ltea::mac::la::downlink::LinkAdaptationResult laResult0;
        laResult0 = linkAdaptation->performLinkAdaptation(iter->scheduledUser,
                                                        0,                   // this is the first "spatial" transport block 
                                                        iter->prbPowerOffsetForLA,      //
                                                        scheduleForTTI,      //
                                                        1,// number of layers for this TB  
                                                        pdcchLength,
                                                        provideRel10DMRS,
                                                        numRel10CSIrsSets
                                                        );
        
    
        unsigned int tb0Size = mcsLookup->getSize(laResult0.mcsIndex, numPRBs, 1);
        wns::ldk::CompoundPtr compound0 = queue->getHeadOfLinePDUSegment(iter->scheduledUser, tb0Size);                                                                    

        MESSAGE_SINGLE(NORMAL, logger, "On the physical layer a transport block of " << tb0Size << " bits (" << numPRBs << " PRBs, MCS " << laResult0.mcsIndex << ") has been allocated for user "
                                        << iter->scheduledUser->getName() << ", payload size is " 
                                        << compound0->getLengthInBits() << " bits. Still queued: " 
                                        << queue->numBitsForUser(iter->scheduledUser) << " bits." );
            
        user_rate += compound0->getLengthInBits();
        
#ifndef WNS_NDEBUG
        if (tb0Size > compound0->getLengthInBits() + compound0->getPCI()->getSize())
            MESSAGE_SINGLE(NORMAL, logger, "WARNING: Reservation on PHY (and thus maybe number of occupied resource blocks) is larger than needed: " 
                                            << tb0Size << " > " << compound0->getLengthInBits() << " + " << compound0->getPCI()->getSize() << " bits.");
#endif                                                                     

            
        tbSizeContextCollector->put(tb0Size, boost::make_tuple("Stream", 0));

        ltea::mac::DownlinkControlInformation* dci0 = activateCommand( compound0->getCommandPool() );
        dci0->peer.assignedToLayers = std::vector<unsigned int>(1,1);
        dci0->peer.blockSize = tb0Size;
        dci0->peer.codeRate = laResult0.codeRate;
        dci0->peer.modulation = laResult0.modulation;
        dci0->local.prbPowerPrecoders = iter->prbPowerPrecodingMap;
        dci0->magic.mcsIndex = laResult0.mcsIndex;
        dci0->magic.estimatedLinkAdaptationSINR = laResult0.estimatedSINR;
        dci0->magic.direction = imtaphy::Downlink;
        dci0->magic.spatialID = 0;
        dci0->magic.id = iter->scheduledUser->getNodeID() * 100000 + scheduleForTTI* 10 + 0;
        dci0->magic.prbTracingDict = iter->prbAndTracingInfo;

        // join scheduling and link adaptation tracing dicts
        for (ltea::mac::PRBSchedulingTracingDictMap::iterator dictIter = dci0->magic.prbTracingDict.begin(); dictIter != dci0->magic.prbTracingDict.end(); dictIter++)
        {
            dictIter->second.insert(laResult0.prbTracingDict[dictIter->first].begin(), laResult0.prbTracingDict[dictIter->first].end());
        }
        
        imtaphy::receivers::feedback::PU2RCFeedback* feedback = dynamic_cast<imtaphy::receivers::feedback::PU2RCFeedback*>(feedbackManager->getFeedback(iter->scheduledUser, scheduleForTTI).get());
        assure(feedback, "no PU2RC feedback");
        
        double linearAvgEstSINR = 0.0;
        int linearAvgEstSINRCounter = 0;
        
        for (imtaphy::interface::PrbPowerPrecodingMap::const_iterator prbIter =  iter->prbPowerPrecodingMap.begin(); prbIter != iter->prbPowerPrecodingMap.end(); prbIter++)
        {
            // this is just for the sinr mismatch: not showing the BS's knowledge
            linearAvgEstSINR += (feedback->sinrMatrix[prbIter->first][feedback->pmi[prbIter->first]][feedback->columnIndicator[prbIter->first]] + iter->prbPowerOffsetForLA[prbIter->first]).get_factor();
            linearAvgEstSINRCounter++;
            
#ifndef WNS_NDEBUG 
            if ((prbIter->first == 1) && (myStation->getNode()->getNodeID() <= 3) && ((scheduleForTTI % 10 == 0)))
            {
                patterns.push_back(std::vector<wns::Ratio>(360));
                imtaphy::antenna::LinearAntennaArray* antennaArray = dynamic_cast<imtaphy::antenna::LinearAntennaArray*>(myStation->getAntenna());
                assure(antennaArray, "No antenna array");
                antennaArray->computeAntennaPattern(*(dci0->local.prbPowerPrecoders[prbIter->first].precoding), patterns[patterns.size()-1], channel->getSpectrum()->getSystemCenterFrequencyWavelenghtMeters(imtaphy::Downlink));
            }
#endif            
        }

        assure(linearAvgEstSINRCounter, "No PRB?");
        dci0->magic.estimatedLinearAvgSINR = wns::Ratio::from_factor(linearAvgEstSINR / static_cast<double>(linearAvgEstSINRCounter));
        ////////////
        
        transportBlocks.push_back(compound0);


        
        harq->storeScheduledTransportBlocks(iter->scheduledUser, transportBlocks);
        txService->registerTransmission(iter->scheduledUser,
                                        transportBlocks,
                                        iter->rank, // number of layers
                                        iter->prbPowerPrecodingMap);
        
        throughputThisTTI[iter->scheduledUser] += user_rate;
    }
    
#ifndef WNS_NDEBUG
    // don't write too much output: only BSs 1-3 and only every 10 TTIs
    // and only on PRB 1
//    if ((myStation->getNode()->getNodeID() <= 3) && ((scheduleForTTI % 10) == 0) && (best.getPRB() == 1))
    if ((myStation->getNode()->getNodeID() <= 3) && ((scheduleForTTI % 10) == 0))
    {
        std::ofstream patternFile;
        std::stringstream ss;
        ss << wns::simulator::getConfiguration().get<std::string>("outputDir") << "/pattern_" << myStation->getNode()->getName() << "_TTI" << scheduleForTTI << ".dat";
        patternFile.open(ss.str().c_str());

        for (unsigned int i = 0; i < 360; i++)
        {
            patternFile << i;
            for (unsigned int userId = 0; userId < patterns.size(); userId++)
            {
                patternFile << "\t" << patterns[userId][i].get_factor();
            }
            patternFile << "\n";
        }
        patternFile.close();
    }
#endif
    
    // unblock the queues
    getReceptor()->wakeup();
}

