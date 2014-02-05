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

#include <WNS/ldk/crc/CRCFilter.hpp>
#include <WNS/ldk/crc/CRC.hpp>
#include <WNS/ldk/fun/FUN.hpp>

using namespace wns::ldk::crc;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(CRCFilter,
                     FunctionalUnit,
                     "wns.crc.CRCFilter",
                     FUNConfigCreator);


CRCFilter::CRCFilter(fun::FUN* fuNet, const wns::pyconfig::View& _config) :
    fu::Plain<CRCFilter>(fuNet),
    SuspendSupport(fuNet, _config),
    friends(),
    fus(),
    config(_config),
    logger(_config.get("logger"))
{
    friends.crc = NULL;
} // CRCFilter


void
CRCFilter::onFUNCreated()
{
    friends.crc = getFUN()->findFriend<CRC*>(config.get<std::string>("crc"));
    if (!friends.crc->isMarking())
        throw wns::Exception("CRC FU (friend) has to be set to mode MARKING");
    for (int i = 0; i < config.len("fus"); ++i)
        fus.push_back(getFUN()->findFriend<FunctionalUnit*>(config.get<std::string>("fus", i)));
}  // onFUNCreated


void
CRCFilter::doOnData(const CompoundPtr& compound)
{
    Command* command = getFUN()->getProxy()->getCommand(compound->getCommandPool(), friends.crc);
    assureType(command, CRCCommand*);
    CRCCommand* crcCommand = dynamic_cast<CRCCommand*>(command);

    if (crcCommand->local.checkOK)
    {
        MESSAGE_SINGLE(NORMAL, logger, "CRCheck - success");
        getDeliverer()->getAcceptor(compound)->onData(compound);
        return;
    }
    else
    {
        CommandPool* commandPool = compound->getCommandPool();

        for (unsigned int i = 0; i < fus.size(); ++i)
        {
            if (getFUN()->getProxy()->commandIsActivated(commandPool, fus.at(i)))
            {
                MESSAGE_SINGLE(NORMAL, logger, "CRCheck - failed, but compound contains at least one activated command of listed FUs\n"
                           "Hence, forwarding compound to upper FU anyway");
                crcCommand->local.checkOK = true;
                getDeliverer()->getAcceptor(compound)->onData(compound);
                return;
            }
        }
    }

    MESSAGE_SINGLE(NORMAL, logger, "CRCheck - failed - dropping compound");
} // doOnData



