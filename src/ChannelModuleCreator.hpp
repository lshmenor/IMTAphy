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

#ifndef CHANNELMODULECREATOR_HPP
#define CHANNELMODULECREATOR_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>

// this could be templated as well
#define SCMPRECISION float

namespace imtaphy {

    class LosLookupInterface;
    namespace pathloss {
        class PathlossModelInterface;
    }
    namespace shadowing {
        class ShadowingModelInterface;
    }
    namespace scm {
        template <typename PRECISION>
        class SpatialChannelModelInterface;
    }
    namespace linkclassify {
        class LinkClassifierInterface;
    }   
        
    class Channel;
    /**
     * @brief Creator implementation to be used with StaticFactory.
     *
     * Useful for constructors with a Channel pointer and pyconfig::View parameter.
     *
     */
    template <typename T, typename KIND = T>
    struct ChannelModuleCreator :
            public ChannelModuleCreator<KIND, KIND>
    {
        virtual KIND* create(
            Channel* channel,
            const wns::pyconfig::View& config)
        {
            return new T(channel, config);
        }
    };

    template <typename KIND>
    struct ChannelModuleCreator<KIND, KIND>
    {
    public:
        virtual KIND* create(
            Channel*,
            const wns::pyconfig::View&) = 0;

        virtual ~ChannelModuleCreator()
        {}
    };
        
    typedef ChannelModuleCreator<linkclassify::LinkClassifierInterface> LinkClassifierCreator;
    typedef wns::StaticFactory<LinkClassifierCreator> LinkClassifierFactory;

    typedef ChannelModuleCreator<pathloss::PathlossModelInterface> PathlossModelCreator;
    typedef wns::StaticFactory<PathlossModelCreator> PathlossModelFactory;
        
    typedef ChannelModuleCreator<shadowing::ShadowingModelInterface> ShadowingModelCreator;
    typedef wns::StaticFactory<ShadowingModelCreator> ShadowingModelFactory;
        
    typedef ChannelModuleCreator<scm::SpatialChannelModelInterface<SCMPRECISION> > SpatialChannelModelCreator;
    typedef wns::StaticFactory<SpatialChannelModelCreator> SpatialChannelModelFactory;
        
}

#endif // CHANNELMODULECREATOR_HPP
