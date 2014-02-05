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

#include <WNS/ldk/tools/Synchronizer.hpp>


using namespace wns::ldk;
using namespace wns::ldk::tools;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Synchronizer, FunctionalUnit, "wns.tools.Synchronizer", FUNConfigCreator);


void
Synchronizer::processIncoming(const CompoundPtr& compound)
{
	++ind;

	getDeliverer()->getAcceptor(compound)->onData(compound);
} // processIncoming


void
Synchronizer::processOutgoing(const CompoundPtr& compound)
{
	assure(hasCapacity(), "received PDU although not accepting.");

	++req;

	buffer = compound;
} // processOutgoing


bool
Synchronizer::hasCapacity() const
{
	return buffer == CompoundPtr();
} // hasCapacity


const CompoundPtr
Synchronizer::hasSomethingToSend() const
{
	return buffer;
} // hasSomethingToSend


CompoundPtr
Synchronizer::getSomethingToSend()
{
	CompoundPtr it;
	it = buffer;
	buffer = CompoundPtr();
	return it;
} // getSomethingToSend



