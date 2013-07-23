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

#ifndef LTEA_MAC_SCHEDULER_USERSPRBMANAGER_HPP
#define LTEA_MAC_SCHEDULER_USERSPRBMANAGER_HPP

#include <WNS/node/Interface.hpp>
#include <IMTAPHY/detail/NodePtrCompare.hpp>
#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <set>

namespace ltea { namespace mac { namespace scheduler {

    typedef std::set<imtaphy::interface::PRB> PRBSet; 
    typedef std::set<wns::node::Interface*, imtaphy::detail::WnsNodeInterfacePtrCompare> UserSet;
    typedef std::map<imtaphy::interface::PRB, UserSet> PRBUserSetMap;
    typedef std::vector<wns::Power> PowerVector;
    
    class UsersPRBManager
    {
        public:
            UsersPRBManager(unsigned int numPRBs_, wns::Power defaultMaxPowerPerPRB_) :
                numPRBs(numPRBs_),
                defaultMaxPowerPerPRB(defaultMaxPowerPerPRB_),
                maxPowerPerPRB(numPRBs, defaultMaxPowerPerPRB)
            {
                for (unsigned int prb = 0; prb < numPRBs; prb++)
                {
                    prbsAndUsers[prb] = UserSet();
                }
            }

            bool isActive(wns::node::Interface* user) const
            {
                return (activeUsers.find(user) != activeUsers.end());
            }

            UserSet getActiveUsers() const
            {
               return activeUsers;
            }
            
            UserSet getActiveUsers(unsigned int prb) const
            {
                // return the set of active users for that prb or an empty set if not found
                PRBUserSetMap::const_iterator iter;
                if ((iter = prbsAndUsers.find(prb)) != prbsAndUsers.end())
                {
                    return iter->second;
                }
                else
                {
                    return UserSet();
                }
            }

            void restrictUserToPRBs(wns::node::Interface* user, PRBSet prbs)
            {
                for (PRBUserSetMap::iterator iter = prbsAndUsers.begin(); 
                        iter != prbsAndUsers.end(); iter++)
                {
                    // Erase user from PRBs not included in the "prbs" set
                    if (prbs.find(iter->first) == prbs.end())
                    {
                        iter->second.erase(user);
                    }
                }
            }
            
            void addActiveUser(wns::node::Interface* user)
            {
                // add user to set of active users and allow him on all PRBs
                for (PRBUserSetMap::iterator iter = prbsAndUsers.begin(); 
                        iter != prbsAndUsers.end(); iter++)
                {
                    iter->second.insert(user);
                }
                
                activeUsers.insert(user);
            }

            void removeActiveUser(wns::node::Interface* user)
            {
                // remove from all user sets and from the set of active users
                for (PRBUserSetMap::iterator iter = prbsAndUsers.begin(); 
                     iter != prbsAndUsers.end(); iter++)
                {
                    iter->second.erase(user);
                }
                
                activeUsers.erase(user);
            }
            
            void markPRBused(unsigned int prb)
            {
                prbsAndUsers.erase(prb);
            }
            
            imtaphy::interface::PRBVector getPRBsAvailable(wns::node::Interface* user)
            {
                imtaphy::interface::PRBVector result;
                
                // Return all PRBs for which user is in the user set
                for (PRBUserSetMap::const_iterator iter = prbsAndUsers.begin(); 
                     iter != prbsAndUsers.end(); iter++)
                {
                    if (iter->second.find(user) != iter->second.end())
                    {
                        result.push_back(iter->first);
                    }
                }
                
                return result;
            }
            
            unsigned int getNumPRBsAvailable(wns::node::Interface* user)
            {
                unsigned int result = 0;
                
                for (PRBUserSetMap::const_iterator iter = prbsAndUsers.begin(); 
                     iter != prbsAndUsers.end(); iter++)
                {
                    if (iter->second.find(user) != iter->second.end())
                    {
                        result++;
                    }
                }
                
                return result;
            }
            
            imtaphy::interface::PRBVector getPRBsAvailable() const
            {
                imtaphy::interface::PRBVector result(prbsAndUsers.size());
                
                unsigned int i;
                PRBUserSetMap::const_iterator iter;
                for (iter = prbsAndUsers.begin(), i = 0; 
                     iter != prbsAndUsers.end(); iter++, i++)
                {
                    result[i] = iter->first;
                }
                
                return result;
            }


            unsigned int getNumPRBsAvailable()
            {
                return prbsAndUsers.size();
            }
           
            bool prbAvailable(unsigned int prb) const
            {
                return (prbsAndUsers.find(prb) != prbsAndUsers.end());
            }
           
            void reset()
            {
                prbsAndUsers.clear();
                
                for (unsigned int prb = 0; prb < numPRBs; prb++)
                {
                    prbsAndUsers[prb] = UserSet();
                }
                
                maxPowerPerPRB = PowerVector(numPRBs, defaultMaxPowerPerPRB);
                perUserPowerRestrictions.clear();
            }
            
            
            
            wns::Power getAvailablePower(wns::node::Interface* user, imtaphy::interface::PRB prb)
            {
                assure(prb < numPRBs, "Invalid PRB");
                
                if (perUserPowerRestrictions.find(user) == perUserPowerRestrictions.end())
                {
                    return maxPowerPerPRB[prb];
                }
                else
                {
                    return perUserPowerRestrictions[user][prb];
                }
            }

            wns::Power getAvailablePower(imtaphy::interface::PRB prb)
            {
                assure(prb < numPRBs, "Invalid PRB");
                
                return maxPowerPerPRB[prb];
            }

            
            void restrictPower(imtaphy::interface::PRB prb, wns::Power power)
            {
                assure(prb < numPRBs, "Invalid PRB");
                maxPowerPerPRB[prb] = power;
            }
            
            void restrictPower(wns::node::Interface* user, imtaphy::interface::PRB prb, wns::Power power)
            {
                assure(prb < numPRBs, "Invalid PRB");
                    
                if (perUserPowerRestrictions.find(user) == perUserPowerRestrictions.end())
                {
                    perUserPowerRestrictions[user] = maxPowerPerPRB;
                }
                
                perUserPowerRestrictions[user][prb] = power;
            }
            
        private:
            unsigned int numPRBs;
            PRBUserSetMap prbsAndUsers;
            UserSet activeUsers;
            wns::Power defaultMaxPowerPerPRB;
            PowerVector maxPowerPerPRB;
            std::map<wns::node::Interface*, PowerVector> perUserPowerRestrictions;
            
    };
    
}}}
#endif
