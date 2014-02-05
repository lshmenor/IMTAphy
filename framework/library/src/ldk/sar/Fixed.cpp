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

#include <WNS/ldk/sar/Fixed.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/Assure.hpp>
#include <WNS/Ttos.hpp>

using namespace wns::ldk;
using namespace wns::ldk::sar;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Fixed, FunctionalUnit, "wns.sar.Fixed", FUNConfigCreator);

Fixed::Fixed(fun::FUN* fuNet, const wns::pyconfig::View& config) :
        SAR<SARCommand>(fuNet, config),
        HasReceptor<>(),
        HasConnector<>(),
        HasDeliverer<>(),
        Delayed<Fixed>(),
        Cloneable<Fixed>(),
        incoming(),
        fragmentNumber(0)
{
} // SimpleSAR

void
Fixed::processIncoming(const CompoundPtr& compound)
{
    incoming.push_back(compound);

    SARCommand* command = getCommand(compound->getCommandPool());

    MESSAGE_BEGIN(NORMAL, logger, m, "");
    m << "received fragment number " << command->magic.fragmentNumber
      << " pos: "
      << command->magic.pos
      << " lastFragment: "
      << ( command->peer.lastFragment ? "yes" : "no" )
        ;
    MESSAGE_END();

    assure(command->magic.fragmentNumber == fragmentNumber,
           "Missing fragment number: " + wns::Ttos(fragmentNumber) +
           ", received fragment number: " + wns::Ttos(command->magic.fragmentNumber));

    if(!command->peer.lastFragment)
    {
        ++fragmentNumber;
        return;
    }

    MESSAGE_BEGIN(NORMAL, logger, m, "");
    m << "-> compound complete, starting reassembly";
    MESSAGE_END();

    CommandPool* reassembledPCI = getFUN()->createCommandPool();
    getFUN()->getProxy()->partialCopy(this, reassembledPCI, compound->getCommandPool());
    CompoundPtr reassembled(new Compound(reassembledPCI, compound->getData()));

    incoming.clear();
    fragmentNumber = 0;

    getDeliverer()->getAcceptor(reassembled)->onData(reassembled);
} // processIncoming

