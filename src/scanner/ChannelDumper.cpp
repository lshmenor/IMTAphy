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

#include <IMTAPHY/scanner/ChannelDumper.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/probe/bus/ContextProvider.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>

#include <IMTAPHY/linkManagement/LinkManager.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <boost/bind.hpp>
#include <limits>

using namespace imtaphy::scanner;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ChannelDumper,
    wns::node::component::Interface,
    "imtaphy.ChannelDumper",
    wns::node::component::ConfigCreator);

ChannelDumper::ChannelDumper(wns::node::Interface* _node, const wns::pyconfig::View& _pyco) : 
    LteScanner(_node, _pyco)
{
 
}


// This is to be used in a pseudo mobile that simply writes the CIR of all links to a file
// the file can be loaded into matlab by saying eval('CIRCTFdump')

void 
ChannelDumper::beforeTTIover(unsigned int tti)
{
    if (tti == 3) // start from TTI=3 because before we don't know how long this is going
    {
        ff.open("CIRCTFdump.m");
        
        layout = channel->getSpatialChannelModel()->getChannelLayout();
        ff << "K = " << layout.K                << ";\n";
        ff << "U = " << layout.U                << ";\n";
        ff << "S = " << layout.S                << ";\n";
        ff << "N = " << layout.N                << ";\n";
        ff << "F = " << layout.F[imtaphy::Downlink]                << ";\n";
        ff << "T = " << numTTIs - 3               << ";\n";
        ff << "CIR = zeros(K,U,S,N,T);\n";
        ff << "CTF = zeros(K,U,S,F,T);\n";
    }
    
    if (tti >= 3)
    {        
        for(unsigned int k=0; k < layout.K; k++)
            for(unsigned int u=0; u < layout.U; u++)
                for(unsigned int s=0; s < layout.S; s++)
                    for (unsigned int n = 0; n < layout.N; n++)
                    {
                        ff  << "CIR("
                            << k + 1
                            << ", "
                            << u + 1
                            << ", "
                            << s + 1
                            << ", "
                            << n + 1
                            << ", "
                            << tti - 2
                            << ") = "
                            << channel->getSpatialChannelModel()->getCurrentCIR(k, imtaphy::Downlink, u, s, n)
                            << ";\n";
                    }
                    
         for(unsigned int k=0; k < layout.K; k++)
            for(unsigned int u=0; u < layout.U; u++)
                for(unsigned int s=0; s < layout.S; s++)
                    for (unsigned int f = 0; f < layout.F[0]; f++)
                    {
                        ff  << "CTF("
                            << k + 1
                            << ", "
                            << u + 1
                            << ", "
                            << s + 1
                            << ", "
                            << f + 1
                            << ", "
                            << tti - 2
                            << ") = "
                            << channel->getSpatialChannelModel()->getCurrentCTF(k, imtaphy::Downlink, u, s, f)
                            << ";\n";
                    }
                   
    }
    
    if (tti > numTTIs - 2)
        ff.close();
}

