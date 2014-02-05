/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WNS/scheduler/strategy/staticpriority/persistentvoip/TBChoser.hpp>
#include <sstream>
#include <limits>

using namespace wns::scheduler::strategy::staticpriority::persistentvoip;


STATIC_FACTORY_REGISTER_WITH_CREATOR(First, ITBChoser, "First", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(BestFit, ITBChoser, "BestFit", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(WorstFit, ITBChoser, "WorstFit", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(Random, ITBChoser, "Random", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(Smallest, ITBChoser, "Smallest", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(Previous, ITBChoser, "Previous", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(LargestSpace, ITBChoser, "LargestSpace", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(SmallestSpace, ITBChoser, "SmallestSpace", wns::PyConfigViewCreator);

TBChoser::TBChoser(const wns::pyconfig::View& config) :
    returnRandom(config.get<bool>("returnRandom"))
{
}

TBChoser::~TBChoser()
{
}

Frame::SearchResult
TBChoser::choseTB(const Frame::SearchResultSet& tbs)
{
    assure(!tbs.empty(), "Cannot chose from empty set");

    Frame::SearchResultSet srs = doChoseTB(tbs);

    Frame::SearchResult sr;
    if(srs.size() > 1)
        sr = pickOne(srs);    
    else
    {
        sr = *(srs.begin());
    }

    assure(sr.success, "Invalid TB");
    assure(sr.length >= sr.tbLength, "TB too small");

    return sr;

}

Frame::SearchResult
TBChoser::pickOne(const Frame::SearchResultSet& srs)
{
    if(returnRandom)
    {
        Frame::SearchResultSet::iterator it;
        unsigned int r = rnd_() * srs.size();
        it = srs.begin();
        for(int i = 0; i < r; i++)
            it++;
        return *it;
    }
    else
    {
        return *(srs.begin());
    }
}

First::First(const wns::pyconfig::View& config) :
    TBChoser(config)
{
}

Frame::Frame::SearchResultSet
First::doChoseTB(const Frame::SearchResultSet& tbs)
{
    Frame::SearchResultSet results;
    results.insert(*(tbs.begin()));
    return results; 
}

BestFit::BestFit(const wns::pyconfig::View& config) :
    TBChoser(config)
{
}

Frame::SearchResultSet
BestFit::doChoseTB(const Frame::SearchResultSet& tbs)
{
    Frame::SearchResultSet theBest;
    Frame::SearchResultSet::iterator it;
    unsigned int bestFit = std::numeric_limits<unsigned int>::max();
    for(it = tbs.begin(); it != tbs.end(); it++)
    {
        unsigned int waste = it->length - it->tbLength;
        if(waste < bestFit)
        {
            bestFit = waste;
            theBest.clear();
            theBest.insert(*it);
        }
        if(waste == bestFit)
            theBest.insert(*it);
    }
    assure(bestFit < std::numeric_limits<unsigned int>::max(), "Failed to find best TB");

    return theBest;
}

WorstFit::WorstFit(const wns::pyconfig::View& config) :
    TBChoser(config)
{
}

Frame::SearchResultSet
WorstFit::doChoseTB(const Frame::SearchResultSet& tbs)
{
    Frame::SearchResultSet::iterator it;
    Frame::SearchResultSet theBest;
    unsigned int worstFit = 0;
    for(it = tbs.begin(); it != tbs.end(); it++)
    {
        unsigned int waste = it->length - it->tbLength; 
        if(waste > worstFit)
        {
            worstFit = waste;
            theBest.clear();
            theBest.insert(*it);
        }
        if(waste == worstFit)
        {
            theBest.insert(*it);
        }
    }

    return theBest;
}

Random::Random(const wns::pyconfig::View& config) :
    TBChoser(config)
{
}

Frame::SearchResultSet
Random::doChoseTB(const Frame::SearchResultSet& tbs)
{
    Frame::SearchResultSet::iterator it;

    unsigned int r = rnd_() * tbs.size();

    it = tbs.begin();
    for(int i = 0; i < r; i++)
        it++;

    Frame::SearchResultSet results;
    results.insert(*it);

    return results;
}

Smallest::Smallest(const wns::pyconfig::View& config) :
    TBChoser(config)
{
}

Frame::SearchResultSet
Smallest::doChoseTB(const Frame::SearchResultSet& tbs)
{
    Frame::SearchResultSet::iterator it;
    Frame::SearchResultSet theBest;
    unsigned int smallest = std::numeric_limits<unsigned int>::max();;
    for(it = tbs.begin(); it != tbs.end(); it++)
    {
        if(it->tbLength < smallest)
        {
            smallest = it->tbLength;
            theBest.clear();
            theBest.insert(*it);
        }
        if(it->tbLength == smallest)
            theBest.insert(*it);
    
    }
    assure(smallest < std::numeric_limits<unsigned int>::max(), "Failed to find smallest TB");

    return theBest;    
}

SmallestSpace::SmallestSpace(const wns::pyconfig::View& config) :
    TBChoser(config)
{
}

Frame::SearchResultSet
SmallestSpace::doChoseTB(const Frame::SearchResultSet& tbs)
{
    Frame::SearchResultSet::iterator it;
    Frame::SearchResultSet theBest;
    unsigned int smallest = std::numeric_limits<unsigned int>::max();;
    for(it = tbs.begin(); it != tbs.end(); it++)
    {
        if(it->length < smallest)
        {
            smallest = it->length;
            theBest.clear();
            theBest.insert(*it);
        }
        if(it->length == smallest)
            theBest.insert(*it);
    }
    assure(smallest < std::numeric_limits<unsigned int>::max(), "Failed to find smallest space");

    return theBest;    
}

LargestSpace::LargestSpace(const wns::pyconfig::View& config) :
    TBChoser(config)
{
}

Frame::SearchResultSet
LargestSpace::doChoseTB(const Frame::SearchResultSet& tbs)
{
    Frame::SearchResultSet::iterator it;
    Frame::SearchResultSet theBest;
    unsigned int largest = 0;
    for(it = tbs.begin(); it != tbs.end(); it++)
    {
        if(it->length > largest)
        {
            largest = it->length;
            theBest.clear();
            theBest.insert(*it);
        }
        if(it->length == largest)
            theBest.insert(*it);
    }
    assure(0, "Failed to find largest space");

    return theBest;    
}

Previous::Previous(const wns::pyconfig::View& config) :
    TBChoser(config)
{
    std::string tbChoserName = config.get<std::string>("fallbackChoser.__plugin__");
    fallbackChoser_ = ITBChoser::Factory::creator(
        tbChoserName)->create(config.get("fallbackChoser"));
}

Previous::~Previous()
{
    delete fallbackChoser_;
}

Frame::SearchResultSet
Previous::doChoseTB(const Frame::SearchResultSet& tbs)
{
    Frame::SearchResult result;
    if(history_.find(tbs.begin()->cid) == history_.end())
    {
        result = fallbackChoser_->choseTB(tbs);
        assure(result.cid == tbs.begin()->cid, "CIDs do not match"); 

        history_[tbs.begin()->cid] = std::set<unsigned int>();
    }
    else
    {
        std::set<unsigned int> previous = history_.find(tbs.begin()->cid)->second;
        assure(!previous.empty(), "List of past RBs is empty.");

        unsigned int bestMatch = 0;
        Frame::SearchResultSet theBest;
        Frame::SearchResultSet::iterator it;
        for(it = tbs.begin(); it != tbs.end(); it++)
        {
            int match = 0;
            for(int i = 0; i < it->tbLength; i++)
            {
                if(previous.find(i + it->tbStart) != previous.end())
                {
                    match++;
                }
            }
            if(match > bestMatch)
            {
                bestMatch = match;
                theBest.clear();
                theBest.insert(*it);
            }
            else if(match > 0 && match == bestMatch)
            {
                theBest.insert(*it);
            }
        }
        if(bestMatch > 0)
        {
            assure(!theBest.empty(), "No best TB candidate set.");
            history_[tbs.begin()->cid].clear();
            /* We do the picking because we need the result now */
            result = pickOne(theBest);
        }
        else
        {
            result = fallbackChoser_->choseTB(tbs);
        }
    }
    for(int i = 0; i < result.tbLength; i++)
    {
        history_[tbs.begin()->cid].insert(i + result.tbStart);
    }
    Frame::SearchResultSet results;
    results.insert(result);
    return results;
}

