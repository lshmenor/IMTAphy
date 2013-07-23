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

#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/linkManagement/classifier/LinkClassifierInterface.hpp>
#include <IMTAPHY/spatialChannel/No.hpp>
#include <IMTAPHY/ChannelModuleCreator.hpp>

#include <IMTAPHY/Channel.hpp>
#include <WNS/distribution/DiscreteUniform.hpp>

using namespace imtaphy;

LinkManager::LinkManager(Channel* channel_, StationList bsList, StationList msList, wns::pyconfig::View config_) :
    baseStations(bsList),
    mobileStations(msList),
    referenceTxPower(wns::Power::from_dBm(0)), // we have to assume something
    channel(channel_),
    config(config_),
    logger(config_.getView("logger")),
    useSCMforRSRP(config_.get<bool>("useSCMforRSRP"))
{
    // read shift vectors if they are available from the config
    if (config.knows("wraparoundShiftVectors") && 
        !config.isNone("wraparoundShiftVectors")) 
    {
        int shiftListLength = config.len("wraparoundShiftVectors");
 
        for (int i=0; i < shiftListLength; i++) 
        {
            const wns::pyconfig::View& shiftVectorConfig = config.getView("wraparoundShiftVectors", i);
            double x = shiftVectorConfig.get<double>("x");
            double y = shiftVectorConfig.get<double>("y");
            wraparoundShiftVectors.push_back(wns::geometry::Vector(x,y,0.0));
        }
    }

    
    // could be empty config from stub
    if (config.knows("classifier"))
    {
        wns::pyconfig::View linkClassifierConfig = config.get("classifier");
        std::string plugin = linkClassifierConfig.get<std::string>("nameInChannelFactory"); // name under which the C++ implementation of the model is registered
        LinkClassifierCreator* lcc = LinkClassifierFactory::creator(plugin);
        linkClassifier = lcc->create(channel, linkClassifierConfig);
        linkClassifier->onWorldCreated();
    }
    
    links.clear();
    
    for (StationList::const_iterator bsIter = baseStations.begin(); 
         bsIter!=baseStations.end() ; bsIter++)
        for (StationList::const_iterator msIter = mobileStations.begin(); 
             msIter!=mobileStations.end() ; msIter++) 
        {
            wns::Position wrappedPosition = getWrapAroundPosition((*bsIter)->getPosition(), (*msIter)->getPosition());
                
            linktype::LinkTypes linktype = linkClassifier->classifyLink(**bsIter, **msIter, wrappedPosition);
                
            Link* link = new Link(*bsIter, *msIter, linktype.scenario, linktype.propagation, linktype.outdoorPropagation, linktype.userLocation);
            links.push_back(link);
                
            link->wrappedMSposition = wrappedPosition;
        }
            
    // add all links to a map so they can be looked up by either of the stations
    for (LinkVector::const_iterator iter = links.begin(); iter != links.end(); iter++)
    {
        StationPhy* bs = (*iter)->getBS();
        StationPhy* ms = (*iter)->getMS();
        
        
#ifndef WNS_NDEBUG
        if (linksPerStation.find(bs) != linksPerStation.end())
        {
            assure(linksPerStation[bs].find(ms) == linksPerStation[bs].end(),
                   " link registered twice!");
        }
        if (linksPerStation.find(ms) != linksPerStation.end())
        {
            assure(linksPerStation[ms].find(bs) == linksPerStation[ms].end(),
                   " link registered twice!");
        }
#endif 

        linksPerStation[ms][bs] = *iter;
        linksPerStation[bs][ms] = *iter;
    }
}


wns::Position
LinkManager::getWrapAroundPosition(wns::Position bsPosition, wns::Position originalMSposition)

{   
    // The idea here is that we allow to surround the simulation scenario's cells with "virtual
    // copies" of themselves in order to have each cell surrounded by the same number of interferers
    // as the center cell. This is achieved by passing a list of so-called wrap-around shift vectors
    // to the LinkManager. These vectors indicate by which distance from the center of the scenario
    // (i.e. where the inner-most site is) we have to shift a copy in the x-y plane to make, e.g., a 
    // cell on the upper right adjacent to a cell on the lower left. We need a total of 6 such "copies"
    // to surround the scenario cells from all sides.
    // In this implementation, only the mobiles' positions get wrapped, the base stations remain at their
    // initial positions. If a mobile's positions gets wrapped depends on the link, that is, on the 
    // position of the respective base station in the link relation. The wrapped position is set to 
    // that position that gives the least distance to the respective base station. Candidate positions
    // are the mobile's original position and all #wrapAroundShiftVectors shifted positions.
    // All computations involving angles, distances etc. are then performed taking the wrappedMSposition
    // for each link. This can make the same mobile station appear to be at completely different positions
    // depending on which base station the link is going to. For models that do not exhibit a correlation
    // between links to different base station sites (like ITU-R M.2135) this is not a problem. For other
    // models (e.g. Correlated Shadowing according to 802.16m EMD), or for determining the O2V/O2I shadowing 
    // the original position of the mobile has to be used
   
   
    double shortest =  (bsPosition - originalMSposition).abs();
    double current;
    int index = -1;

    wns::Position wrapAroundPosition;
    for (unsigned int i = 0; i < wraparoundShiftVectors.size(); i++)
    {
        if ((current = (bsPosition - (originalMSposition + wraparoundShiftVectors[i])).abs()) < shortest)
        {
            shortest = current;
            index = i;
        }
    }
       
    if (index == -1)
        wrapAroundPosition = originalMSposition;
    else
    {
        wrapAroundPosition = originalMSposition + wraparoundShiftVectors[index];
    }
       
    return wrapAroundPosition;
}




void 
LinkManager::onPathlossAndShadowingReady()
{
    // precompute wideband loss
    for (LinkVector::const_iterator iter = links.begin();
         iter != links.end(); iter++)
    {
        wns::Ratio pathloss = channel->computePathloss(*iter);
        wns::Ratio shadowing = channel->computeShadowing(*iter);
             
        StationPhy* ms = (*iter)->getMS();
        StationPhy* bs = (*iter)->getBS();
            
            
        wns::Ratio msAntennaGain = ms->getAntenna()
            ->getGain(channel->getAzimuth((*iter)->getWrappedMSposition(),
                                          bs->getPosition()),
                      channel->getElevation((*iter)->getWrappedMSposition(),
                                            bs->getPosition()));
                                        
        wns::Ratio bsAntennaGain = bs->getAntenna()
            ->getGain(channel->getAzimuth(bs->getPosition(),
                                          (*iter)->getWrappedMSposition()),
                      channel->getElevation(bs->getPosition(),
                                            (*iter)->getWrappedMSposition()));

        // the wideband loss is supposed to contain the antenna patterns
        // the spatial channel model also includes them but in that case 
        // only pathloss - shadowing are take into account
        // It is assumed that rxPower = txPower - pathloss + shadowing so we have to substract shadowing
        // Also for SCM links (no known yet) we take the widebandLoss (including patterns) to determine the association
        (*iter)->widebandLoss = pathloss - shadowing - msAntennaGain - bsAntennaGain;
        (*iter)->shadowing = shadowing;
        (*iter)->pathloss = pathloss;
    }
}

void LinkManager::computeRSRPbasedOnSCM(imtaphy::scm::SpatialChannelModelInterface< float >* scm, imtaphy::Spectrum* spectrum)
{
    // initialize RSRP to 0
    for (LinkVector::const_iterator iter = links.begin(); iter != links.end(); iter++)
    {
        (*iter)->RSRP = wns::Power::from_mW(0.0);
    }
    
    
    // Sesia p. 465: "The path-loss compensation component is based on the UE's estimate of the downlink path-loss, which 
    // can be derived from the UE's measurement of Reference Signal Received Power (RSRP) (see Section 13.4.1.1) and 
    // the known transmission power of the downlink Reference Signals (RSs), which is broadcast by the eNodeB
    // In order to obtain a reasonable indication of the uplink path-loss, the UE should filter the downlink path-loss estimate 
    // with a suitable time-window to remove the effect of fast fading but not shadowing. Typical filter lengths are 
    // between 100 and 500 ms for effective operation."

    // do the RSRP computation based on the direction (UL/DL) for which more PRBs are configured
    imtaphy::Direction direction = spectrum->getNumberOfPRBs(imtaphy::Uplink) > spectrum->getNumberOfPRBs(imtaphy::Downlink) ? imtaphy::Uplink : imtaphy::Downlink;

    // evolve to multiple time instances to allow for averaging over time-selective channel
    unsigned int count = 0;
    for (unsigned int tti = 1; tti < 500; tti += 100)
    {
        scm->evolve(static_cast<double>(tti) / 1000.0);

        for (LinkVector::const_iterator iter = links.begin(); iter != links.end(); iter++)
        {
            unsigned int numMSantenna = (*iter)->getMS()->getAntenna()->getNumberOfElements();
            unsigned int numBSantenna = (*iter)->getBS()->getAntenna()->getNumberOfElements();
            double gain = 0.0;

            for (unsigned int prb = 0; prb < spectrum->getNumberOfPRBs(direction); prb = prb + 1)
            {
                imtaphy::detail::ComplexFloatMatrixPtr channelMatrix = (*iter)->getChannelMatrix(direction, prb);

                gain += pow(imtaphy::detail::matrixNorm(*channelMatrix), 2.0);
            }

            gain /= static_cast<double>(spectrum->getNumberOfPRBs(direction));

            // compute the linear average over frequency bins of the current channel gain
            // normalize transmit power by transmit antennas s
            (*iter)->RSRP += wns::Power::from_mW((referenceTxPower.get_mW() / static_cast<double>(numMSantenna*numBSantenna)) * gain);
        } // end loop over all links

        count++;
    } // end loop over multiple time instances


    // compute arithmetic mean over time samples
    for (LinkVector::const_iterator iter = links.begin(); iter != links.end(); iter++)
    {
        (*iter)->RSRP = (*iter)->RSRP / static_cast<double>(count);
    }

    
}

void LinkManager::computeRSRPbasedOnWBL()
{
    for (LinkVector::const_iterator iter = links.begin(); iter != links.end(); iter++)
    {
        (*iter)->RSRP = wns::Power::from_mW(referenceTxPower.get_mW() / (*iter)->getWidebandLoss().get_factor());
    }
}

void LinkManager::doAfterSCMinit(imtaphy::scm::SpatialChannelModelInterface< float >* scm, imtaphy::Spectrum* spectrum)
{
    
    for (LinkVector::const_iterator iter = links.begin(); iter != links.end(); iter++)
        (*iter)->initComplexFloatChannelMatrices<float>(spectrum, scm);

    std::string scmCriterion = config.get<std::string>("scmLinkCriterion");
    
    if ((scmCriterion == "all") || (scmCriterion == "All"))
    {
        if (useSCMforRSRP)
        {
            computeRSRPbasedOnSCM(scm, spectrum);
        }
        else
        {
            computeRSRPbasedOnWBL();
        }
        determineServingLinks();
    }
    else
    {
        // we have to do this in all cases (even if we don't have SCM links at all)
        for (LinkVector::const_iterator iter = links.begin(); iter != links.end(); iter++)
            (*iter)->initComplexFloatChannelMatrices<float>(spectrum, scm);
    }
    
    MESSAGE_BEGIN(NORMAL, logger, m, "");
        for (unsigned int i = 0; i < links.size(); i++)
        {
            Link* link = links[i];

            
            m << "Link k=" << i << " (BS: " << link->getBS()->getName()
                << ", MS: " << link->getMS()->getName() << ") Scenario: "
                << link->getScenario() << " Propagation: " << link->getPropagation()
                << " widebandLoss = " << link->getWidebandLoss()
                << " SCM: " << link->isSCM()
                << " RSRP: " << link->getRSRP();
            
            if (link->isSCM())
            {
                m << " SCM Link ID =" << link->getSCMlinkId() << "\n";
            }
            else
            {
                m << "\n";
            }
        }
    MESSAGE_END();
}

void LinkManager::doBeforeSCMinit()
{
    // determine SCM links (based on serving links which were computed based on widebandloss)
    scmLinks.clear();
    
    std::string scmCriterion = config.get<std::string>("scmLinkCriterion");
    
    if ((scmCriterion == "all") || (scmCriterion == "All"))
    {   // add all links
    for (LinkVector::const_iterator iter = links.begin();
         iter < links.end(); iter++)
         {
             scmLinks.push_back(*iter);
         }
         
    }
    else
    {
        // if we don't have SCM on all links, we have to rely on the WBL for computing the serving links
        computeRSRPbasedOnWBL();
        determineServingLinks();
        
        if ((scmCriterion == "serving") || (scmCriterion == "Serving"))
        {
            // only add serving links
            for (LinkMap::const_iterator iter = servingLinks.begin(); iter != servingLinks.end(); iter++)
                scmLinks.push_back(iter->second);
        }
        else // no SCM links
        {
            scmLinks.clear();
        }
    }
        

    // set the scmLinkId for easy access from the spatial channel model:
    for (unsigned int i = 0; i < scmLinks.size(); i++)
        scmLinks[i]->scmLinkId = i;
        
    MESSAGE_BEGIN(NORMAL, logger, m, "");
        for (unsigned int i = 0; i < links.size(); i++)
        {
            Link* link = links[i];

            m   << "Link k=" << i << " (BS: " << link->getBS()->getName()
                << ", MS: " << link->getMS()->getName() << ") Scenario: "
                << link->getScenario() << " Propagation: " << link->getPropagation()
                //        << " wrappedMSposition " << link->getWrappedMSposition()
                << " widebandLoss = " << link->getWidebandLoss()
                << " SCM: " << link->isSCM();
            
            if (link->isSCM())
            {
                m << " SCM Link ID =" << link->getSCMlinkId() << "\n";
            }
            else
            {
                m << "\n";
            }
        }
    MESSAGE_END();
}



void imtaphy::LinkManager::determineServingLinks()
{
    typedef std::multimap<wns::Power, Link*> PowerLinksMap;
    
    wns::Ratio handoverMargin = config.get<wns::Ratio>("handoverMargin");
    
    // identify the serving BS for each MS and the list of MS served by each BS
    for (StationList::const_iterator iter = mobileStations.begin();
         iter != mobileStations.end(); iter++)
         {
             LinkMap linksForThisMS = linksPerStation[*iter];
             PowerLinksMap sortedLinks;
             
             // find for each mobile the strongest link (i.e., the link to the serving BS)
             for (LinkMap::const_iterator linkIter = linksForThisMS.begin();
                  linkIter != linksForThisMS.end(); linkIter++)
                  {
                      sortedLinks.insert(std::make_pair<wns::Power, Link*>((linkIter->second)->getRSRP(), linkIter->second));
                  }
                  
                  // this should never happen because the mobiles are taken from the links
                  assure(sortedLinks.size(), "Mobile without any link to a base station");
             
             PowerLinksMap::const_reverse_iterator strongestLink = sortedLinks.rbegin();

             MESSAGE_SINGLE(NORMAL, logger, "Strongest link to " << (*iter)->getName() << " comes from "
                                                                 << strongestLink->second->getBS()->getName() << " with WBL="
                                                                 << strongestLink->second->getWidebandLoss()
                                                                 << " and RSRP=" << strongestLink->first 
                                                                 << " HO margin=" << handoverMargin);

             // if we are given a handover margin > 0dB we will randomly select one of the
             // strongest links that is within the specified handover margin
             
             LinkVector strongestLinks;
             strongestLinks.clear();
             
             strongestLinks.push_back(strongestLink->second);
             
             PowerLinksMap::const_reverse_iterator nextStrongestLink = strongestLink;
             nextStrongestLink++;
             
             // consider all links that are within handoverMargin of the strongest link.
             // note that the strongest link has the smallest wideBandLoss
             while ((nextStrongestLink != sortedLinks.rend()) &&
                    (strongestLink->first.get_mW() / nextStrongestLink->first.get_mW() <= handoverMargin.get_factor()))
             {
                 strongestLinks.push_back(nextStrongestLink->second);
                 MESSAGE_SINGLE(NORMAL, logger,  "Next-Strongest link to " << (*iter)->getName() << " comes from "
                                                                           << nextStrongestLink->second->getBS()->getName() << " with WBL="
                                                                           << nextStrongestLink->second->getWidebandLoss()
                                                                           << " and RSRP=" << nextStrongestLink->first);
                 nextStrongestLink++;
             }
             
             wns::distribution::DiscreteUniform dis(0, strongestLinks.size() - 1,
                                                    wns::simulator::getRNG());
             
             unsigned int index = static_cast<unsigned int>(dis());
             
             MESSAGE_SINGLE(NORMAL, logger, "" << index << "-strongest link chosen: WBL= " << strongestLinks[index]->getWidebandLoss());

             Link* servingLink = strongestLinks[index];
             // store the link that serves this mobile station
             servingLinks[*iter] = servingLink;
             
             // add this served link to the map of links served by that BS:
             servedLinks[servingLink->getBS()][*iter] = servingLink;
         }
         
}

LinkMap 
LinkManager::getAllLinksForStation(StationPhy* station) 
{
    // TODO: maybe better return emtpy list?
    assure(linksPerStation.find(station) != linksPerStation.end(), "No links");
    
    return linksPerStation[station];
}

LinkMap 
LinkManager::getServedLinksForBaseStation(StationPhy* baseStation) 
{
    if (servedLinks.find(baseStation) != servedLinks.end())
        return servedLinks[baseStation];
    else
        return LinkMap();
}

Link* 
LinkManager::getServingLinkForMobileStation(StationPhy* mobileStation) 
{
    // TODO: maybe better return emtpy list?
    assure(servingLinks.find(mobileStation) != servingLinks.end(), "No links");
    
    return servingLinks[mobileStation];
}

Link* 
LinkManager::getLink(StationPhy* station1, StationPhy* station2)
{
    assure(linksPerStation.find(station1) != linksPerStation.end(), "Link not found");
    assure(linksPerStation[station1].find(station2) != linksPerStation[station1].end(),
           "Link not found");
           
    return linksPerStation[station1][station2];
}


void
LinkManagerStub::addLink(Link* link, bool isSCM)
{
    links.push_back(link);
    
    linksPerStation[link->getBS()][link->getMS()] = link;
    linksPerStation[link->getMS()][link->getBS()] = link;
    
        
    if (isSCM)
    {
        int oldSize = scmLinks.size();
        scmLinks.push_back(link);
        link->scmLinkId = oldSize;
    }
    else
        link->scmLinkId = -1;
}

LinkVector 
LinkManager::getSCMLinks()
{
    return scmLinks;
}

LinkVector 
LinkManager::getAllLinks()
{
    return links;
}
