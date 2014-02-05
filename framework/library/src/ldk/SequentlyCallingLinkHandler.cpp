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

#include <WNS/ldk/SequentlyCallingLinkHandler.hpp>

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/PyConfigCreator.hpp>

#include <WNS/Assure.hpp>
#include <WNS/Exception.hpp>

using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(SequentlyCallingLinkHandler,
				     LinkHandlerInterface,
				     "wns.ldk.SequentlyCallingLinkHandler",
				     wns::ldk::PyConfigCreator);

SequentlyCallingLinkHandler::SequentlyCallingLinkHandler(const wns::pyconfig::View& _config) :
	inAction(false),
	pendingCompoundsContainingFUs(),
	sendDataPending(0),
	sendDataFUCompound(NULL, CompoundPtr()),
	wakeupFUs(),
	inWakeup(false),
	wakeupFUsInWakeup(),
	onDataFUCompounds(),

	traceCompoundJourney(_config.get<bool>("traceCompoundJourney")),
	isAcceptingLogger(_config.get<wns::pyconfig::View>("isAcceptingLogger")),
	sendDataLogger(_config.get<wns::pyconfig::View>("sendDataLogger")),
	wakeupLogger(_config.get<wns::pyconfig::View>("wakeupLogger")),
	onDataLogger(_config.get<wns::pyconfig::View>("onDataLogger"))
{
} // SequentlyCallingLinkHandler

bool
SequentlyCallingLinkHandler::isAcceptingForwarded(FunctionalUnit* fu, const CompoundPtr& compound)
{
	if (sendDataPending)
	{
		MESSAGE_BEGIN(NORMAL, isAcceptingLogger, m, fu->getFUN()->getName());
		m << " setting FU to be woken up";
		MESSAGE_END();

		/**
		 * @todo: ksw,msg
		 * - implement getting the calling FU in a better way
		 */
		if (compound->getCallingFU())
			pendingCompoundsContainingFUs.push_front(compound->getCallingFU());
		else if (fu->getReceptor()->size() == 1)
			pendingCompoundsContainingFUs.push_front(fu->getReceptor()->get().front());
		else
			throw wns::Exception("Don't know calling FU");

		return false;
	}

	bool isAccepting = doIsAccepting(fu, compound);

	MESSAGE_BEGIN(NORMAL, isAcceptingLogger, m, fu->getFUN()->getName());
	m << " function isAccepting(...) of FU "
	  << fu->getName() << " called: FU is ";
	if (isAccepting)
		m << "accepting";
	else
		m << "not accepting";
	m << ", compound: " << compound.getPtr();
	MESSAGE_END();

	return isAccepting;
} // isAcceptingForwarded

void
SequentlyCallingLinkHandler::sendDataForwarded(FunctionalUnit* fu, const CompoundPtr& compound)
{
	MESSAGE_BEGIN(NORMAL, sendDataLogger, m, fu->getFUN()->getName());
	m << " queueing sendData(...) call of FU "
	  << fu->getName()
	  << ", compound: " << compound.getPtr();
	MESSAGE_END();

	compound->setCallingFU(fu);

	if (!inAction)
		sendDataHandler(fu, compound);
	else
	{
		++sendDataPending;
		sendDataFUCompound = FUCompound(fu, compound);
	}
} // sendDataForwarded

void
SequentlyCallingLinkHandler::sendDataHandler(FunctionalUnit* fu, const CompoundPtr& compound)
{
	inAction = true;

	sendDataForwarded(fu, compound);

	mainHandler();

	inAction = false;
} // sendDataHandler

void
SequentlyCallingLinkHandler::wakeupForwarded(FunctionalUnit* fu)
{
	MESSAGE_BEGIN(NORMAL, wakeupLogger, m, fu->getFUN()->getName());
	m << " queueing wakeup(...) call of FU "
	  << fu->getName();
	MESSAGE_END();

	if (!inAction)
		wakeupHandler(fu);
	else
	{
		if (inWakeup)
			wakeupFUsInWakeup.push_back(fu);
		else
			wakeupFUs.push_back(fu);
	}
} // wakeupForwarded

void
SequentlyCallingLinkHandler::wakeupHandler(FunctionalUnit* fu)
{
	inAction = true;

	wakeupForwarded(fu);

	mainHandler();

	inAction = false;
} // wakeupHandler

void
SequentlyCallingLinkHandler::onDataForwarded(FunctionalUnit* fu, const CompoundPtr& compound)
{
	MESSAGE_BEGIN(NORMAL, onDataLogger, m, fu->getFUN()->getName());
	m << " queueing onData(...) call of FU "
	  << fu->getName()
	  << ", compound: " << compound.getPtr();
	MESSAGE_END();

	if (!inAction)
		onDataHandler(fu, compound);
	else
		onDataFUCompounds.push_back(FUCompound(fu, compound));
} // onDataForwarded

void
SequentlyCallingLinkHandler::onDataHandler(FunctionalUnit* fu, const CompoundPtr& compound)
{
	inAction = true;

	onDataForwarded(fu, compound);

	mainHandler();

	inAction = false;
} // onDataHandler

void
SequentlyCallingLinkHandler::mainHandler()
{
	while(true)
	{
		while (sendDataFUCompound.fu)
		{
			assure(sendDataPending < 2, "more than one compound is pending");

			MESSAGE_BEGIN(NORMAL, sendDataLogger, m, sendDataFUCompound.fu->getFUN()->getName());
			m << " function sendData(...) of FU "
			  << sendDataFUCompound.fu->getName() << " called"
			  << ", compound: " << sendDataFUCompound.compound.getPtr();
			MESSAGE_END();

#ifndef WNS_NO_LOGGING
			if (traceCompoundJourney && sendDataFUCompound.compound)
				sendDataFUCompound.compound->visit(sendDataFUCompound.fu); // JOURNEY
#endif

			FUCompound sendDataFUCompoundHelp = sendDataFUCompound;

			sendDataPending = 0;
			sendDataFUCompound.fu = NULL;
			sendDataFUCompound.compound = CompoundPtr();

			doSendData(sendDataFUCompoundHelp.fu, sendDataFUCompoundHelp.compound);
		}

		if (!pendingCompoundsContainingFUs.empty())
		{
			FunctionalUnit* pendingCompoundsContainingFUHelp = pendingCompoundsContainingFUs.front();
			pendingCompoundsContainingFUs.pop_front();

			doWakeup(pendingCompoundsContainingFUHelp);

			continue;
		}

		if (!onDataFUCompounds.empty())
		{
			FUCompound onDataFUCompoundHelp = onDataFUCompounds.front();
			onDataFUCompounds.pop_front();

			MESSAGE_BEGIN(NORMAL, onDataLogger, m, onDataFUCompoundHelp.fu->getFUN()->getName());
			m << " function onData(...) of FU "
			  << onDataFUCompoundHelp.fu->getName() << " called"
			  << ", compound: " << onDataFUCompoundHelp.compound.getPtr();
			MESSAGE_END();

#ifndef WNS_NO_LOGGING
			if (traceCompoundJourney && onDataFUCompoundHelp.compound)
				onDataFUCompoundHelp.compound->visit(onDataFUCompoundHelp.fu); // JOURNEY
#endif

			doOnData(onDataFUCompoundHelp.fu, onDataFUCompoundHelp.compound);

			continue;
		}

		if (!wakeupFUs.empty())
		{
			FunctionalUnit* wakeupFUHelp = wakeupFUs.front();
			wakeupFUs.pop_front();

			MESSAGE_BEGIN(NORMAL, wakeupLogger, m, wakeupFUHelp->getFUN()->getName());
			m << " function wakeup(...) of FU "
			  << wakeupFUHelp->getName() << " called";
			MESSAGE_END();

			inWakeup = true;

			doWakeup(wakeupFUHelp);
			wakeupFUs.insert(wakeupFUs.begin(),
					 wakeupFUsInWakeup.begin(),
					 wakeupFUsInWakeup.end());
			wakeupFUsInWakeup.clear();

			inWakeup = false;
		}
		else
		{
			break;
		}
	}
} // mainHandler


