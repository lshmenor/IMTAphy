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

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_PERSISTENTVOIP_TBCHOSER_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_PERSISTENTVOIP_TBCHOSER_HPP

#include <WNS/scheduler/strategy/staticpriority/persistentvoip/ResourceGrid.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority { namespace persistentvoip {

class ITBChoser
{
    public:         
        typedef wns::PyConfigViewCreator<ITBChoser> Creator;
        typedef wns::StaticFactory<Creator> Factory;

        virtual Frame::SearchResult
        choseTB(const Frame::SearchResultSet& tbs) = 0;

    private:
        virtual Frame::SearchResultSet
        doChoseTB(const Frame::SearchResultSet& tbs) = 0;
};

class TBChoser :
    public ITBChoser
{
    public:         
        typedef wns::Creator<ITBChoser> Creator;
        typedef wns::StaticFactory<Creator> Factory;

        TBChoser(const wns::pyconfig::View& config);
        ~TBChoser();

        virtual Frame::SearchResult
        choseTB(const Frame::SearchResultSet& tbs);

    protected:
        ITBChoser* equalChoser_;
        wns::distribution::StandardUniform rnd_;

        virtual Frame::SearchResult
        pickOne(const Frame::SearchResultSet& tbs);

    private:
        virtual Frame::SearchResultSet
        doChoseTB(const Frame::SearchResultSet& tbs) = 0;

        bool returnRandom;
};

class First :
    public TBChoser
{
    public:
        First(const wns::pyconfig::View& config);

    private:       
        virtual Frame::SearchResultSet
        doChoseTB(const Frame::SearchResultSet& tbs);
};

class BestFit :
    public TBChoser
{        
    public:
        BestFit(const wns::pyconfig::View& config);

    private:       
        virtual Frame::SearchResultSet
        doChoseTB(const Frame::SearchResultSet& tbs);
};

class WorstFit :
    public TBChoser
{         
    public:
        WorstFit(const wns::pyconfig::View& config);

    private:       
        virtual Frame::SearchResultSet
        doChoseTB(const Frame::SearchResultSet& tbs);
};

class Random :
    public TBChoser
{       
    public:
        Random(const wns::pyconfig::View& config);

    private:       
        virtual Frame::SearchResultSet
        doChoseTB(const Frame::SearchResultSet& tbs);
};

class Smallest :
    public TBChoser
{
    public:
        Smallest(const wns::pyconfig::View& config);

    private:       
        virtual Frame::SearchResultSet
        doChoseTB(const Frame::SearchResultSet& tbs);
};

class SmallestSpace :
    public TBChoser
{
    public:
        SmallestSpace(const wns::pyconfig::View& config);

    private:       
        virtual Frame::SearchResultSet
        doChoseTB(const Frame::SearchResultSet& tbs);
};

class LargestSpace :
    public TBChoser
{
    public:
        LargestSpace(const wns::pyconfig::View& config);

    private:       
        virtual Frame::SearchResultSet
        doChoseTB(const Frame::SearchResultSet& tbs);
};

class Previous :
    public TBChoser
{       
    public:
        Previous(const wns::pyconfig::View& config);
        ~Previous();

    private:       
        virtual Frame::SearchResultSet
        doChoseTB(const Frame::SearchResultSet& tbs);

        std::map<ConnectionID, std::set<unsigned int> > history_;

        ITBChoser* fallbackChoser_;
        
};


}}}}}

#endif
