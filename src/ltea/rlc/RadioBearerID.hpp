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

// based on code from openWNS with the following license:

/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
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

#ifndef LTEA_RLC_RADIOBEARERID_HPP
#define LTEA_RLC_RADIOBEARERID_HPP

#include <WNS/service/dll/FlowID.hpp>
#include <WNS/ldk/Key.hpp>

namespace ltea { namespace rlc { 

class RadioBearerIDBuilder;

class RadioBearerID :
    public wns::ldk::Key
{
public:
  RadioBearerID(const RadioBearerIDBuilder* factory, const wns::ldk::CompoundPtr& compound);

  RadioBearerID(wns::service::dll::FlowID _radioBearerID);
  
  std::string str() const;
  
  bool operator<(const wns::ldk::Key& other) const;
  
  wns::service::dll::FlowID radioBearerID;
};

class RadioBearerIDBuilder :
    public wns::ldk::KeyBuilder
{
public:
  RadioBearerIDBuilder(const wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
  
  virtual void onFUNCreated();
  
  virtual wns::ldk::ConstKeyPtr operator () (const wns::ldk::CompoundPtr& compound, int /*direction*/) const;

  const wns::ldk::fun::FUN* fun;

  struct Friends {
    wns::ldk::CommandReaderInterface* pdcpReader;
  } friends;
};

} // rlc
} // ltea

#endif 
