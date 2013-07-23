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

#include <IMTAPHY/ltea/rlc/RadioBearerID.hpp>

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/CommandReaderInterface.hpp>
#include <DLL/UpperConvergence.hpp>
#include <boost/functional/hash.hpp>

using namespace ltea::rlc;

STATIC_FACTORY_REGISTER_WITH_CREATOR(RadioBearerIDBuilder,
                                     wns::ldk::KeyBuilder,
                                     "ltea.rlc.RadioBearerID",
                                     wns::ldk::FUNConfigCreator);


RadioBearerID::RadioBearerID(const RadioBearerIDBuilder* factory,
	       const wns::ldk::CompoundPtr& compound)
{
  wns::ldk::CommandPool* commandPool = compound->getCommandPool();

  dll::UpperCommand* pdcpCommand = factory->friends.pdcpReader->readCommand<dll::UpperCommand>(commandPool);

  // TODO: move pdcpCommand to own class that includes a EPS/radio bearer
  size_t hash = 0; // the hash value (ID) should be the same regardless of the direction -> min/max
  boost::hash_combine(hash, std::min(pdcpCommand->peer.sourceMACAddress.getInteger(), pdcpCommand->peer.targetMACAddress.getInteger()));
  boost::hash_combine(hash, std::max(pdcpCommand->peer.sourceMACAddress.getInteger(), pdcpCommand->peer.targetMACAddress.getInteger()));
  
  // basically casting unsigned to signed int but as long as it is still unique 
  // negative flowIDs should not make a difference
  radioBearerID = -static_cast<wns::service::dll::FlowID>(hash); 
}

RadioBearerID::RadioBearerID(wns::service::dll::FlowID _radioBearerID)
{
  radioBearerID = _radioBearerID;
}

bool
RadioBearerID::operator<(const wns::ldk::Key& _other) const
{
  assureType(&_other, const RadioBearerID*);
  const RadioBearerID* other = static_cast<const RadioBearerID*>(&_other);
  return radioBearerID < other->radioBearerID;
}

std::string
RadioBearerID::str() const
{
  std::stringstream ss;
  ss << "RadioBearerID (currently hash of target and source MAC address): " << radioBearerID;
  return ss.str();
}

RadioBearerIDBuilder::RadioBearerIDBuilder(const wns::ldk::fun::FUN* _fun,
			     const wns::pyconfig::View& /* config */) :
  fun(_fun)
{
}

void
RadioBearerIDBuilder::onFUNCreated()
{
  friends.pdcpReader = fun->getProxy()->getCommandReader("pdcp");
}

wns::ldk::ConstKeyPtr
RadioBearerIDBuilder::operator () (const wns::ldk::CompoundPtr& compound, int /*direction*/) const
{
  return wns::ldk::ConstKeyPtr(new RadioBearerID(this, compound));
} // operator()
