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

#include "SimpleLinkHandler.hpp"

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/PyConfigCreator.hpp>

#include <WNS/module/Base.hpp>

using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(SimpleLinkHandler,
                                     LinkHandlerInterface,
                                     "wns.ldk.SimpleLinkHandler",
                                     wns::ldk::PyConfigCreator);

SimpleLinkHandler::SimpleLinkHandler(const wns::pyconfig::View& _config) :
    config(_config),
    traceCompoundJourney(config.get<bool>("traceCompoundJourney")),
    isAcceptingLogger(config.get<wns::pyconfig::View>("isAcceptingLogger")),
    sendDataLogger(config.get<wns::pyconfig::View>("sendDataLogger")),
    wakeupLogger(config.get<wns::pyconfig::View>("wakeupLogger")),
    onDataLogger(config.get<wns::pyconfig::View>("onDataLogger"))
{
} // SimpleLinkHandler

bool
SimpleLinkHandler::isAccepting(IConnectorReceptacle* cr, const CompoundPtr& compound)
{
    MESSAGE_BEGIN(VERBOSE, isAcceptingLogger, m, cr->getFU()->getFUN()->getName());
    m << " calling doIsAccepting of FU "
      << cr->getFU()->getName();
    MESSAGE_END();

    bool isAccepting = doIsAccepting(cr, compound);

    MESSAGE_BEGIN(VERBOSE, isAcceptingLogger, m, cr->getFU()->getFUN()->getName());
    m << " function isAccepting(...) of FU "
      << cr->getFU()->getName() << " called: FU is ";
    if (isAccepting)
        m << "accepting";
    else
        m << "not accepting";
    m << "\ncompound: " << compound.getPtr();
    MESSAGE_END();

    return isAccepting;
} // isAcceptingForwarded

void
SimpleLinkHandler::sendData(IConnectorReceptacle* cr, const CompoundPtr& compound)
{
    MESSAGE_BEGIN(VERBOSE, sendDataLogger, m, cr->getFU()->getFUN()->getName());
    m << " function sendData(...) of FU "
      << cr->getFU()->getName() << " called"
      << "\ncompound: " << compound.getPtr();
    MESSAGE_END();

#ifndef WNS_NO_LOGGING
    if (traceCompoundJourney && compound)
        compound->visit(cr->getFU()); // JOURNEY
#endif

    doSendData(cr, compound);
} // sendDataForwarded

void
SimpleLinkHandler::wakeup(IReceptorReceptacle* rr)
{
    MESSAGE_BEGIN(VERBOSE, wakeupLogger, m, rr->getFU()->getFUN()->getName());
    m << " function wakeup(...) of FU "
      << rr->getFU()->getName() << " called";
    MESSAGE_END();

    doWakeup(rr);
} // wakeupForwarded

void
SimpleLinkHandler::onData(IDelivererReceptacle* dr, const CompoundPtr& compound)
{
    MESSAGE_BEGIN(VERBOSE, onDataLogger, m, dr->getFU()->getFUN()->getName());
    m << " function onData(...) of FU "
      << dr->getFU()->getName() << " called"
      << "\ncompound: " << compound.getPtr();
    MESSAGE_END();

#ifndef WNS_NO_LOGGING
    if (traceCompoundJourney && compound)
        compound->visit(dr->getFU()); // JOURNEY
#endif

    doOnData(dr, compound);
} // onDataForwarded


