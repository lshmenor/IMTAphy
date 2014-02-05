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

#include <WNS/ldk/SuspendSupport.hpp>
#include <WNS/ldk/SuspendedInterface.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/probe/bus/utils.hpp>

#include <WNS/Assure.hpp>


using namespace wns::ldk;

SuspendSupport::SuspendSupport(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _config) :
	suspending(false),
	suspended(false),
	si(NULL),
	suspendStartTime(0.0),
	useProbe(_config.get<bool>("useSuspendProbe")),
        timeBufferEmpty(wns::probe::bus::collector(_fun->getLayer()->getContextProviderCollection(),
						   _config,
						   "suspendProbeName")),
	fun(_fun),
	config(_config)
{
} // SuspendSupport


SuspendSupport::SuspendSupport(const SuspendSupport& other) :
	SuspendableInterface(),
	suspending(other.suspending),
	suspended(other.suspended),
	si(other.si),
	suspendStartTime(other.suspendStartTime),
	useProbe(other.useProbe),
	timeBufferEmpty(wns::probe::bus::collector(other.fun->getLayer()->getContextProviderCollection(),
						   other.config,
						   "suspendProbeName")),
	fun(other.fun),
	config(other.config)
{
} // SuspendSupport


SuspendSupport::~SuspendSupport()
{} // ~SuspendSupport


void
SuspendSupport::doSuspend(SuspendedInterface* _si)
{
	assure(_si, "SuspendedInterface not set");

	si = _si;
	suspending = true;
	suspended = false;

	suspendStartTime = wns::simulator::getEventScheduler()->getTime();

	trySuspend();
} // doSuspend


bool
SuspendSupport::isSuspending() const
{
	return suspending;
} // isSuspending


void
SuspendSupport::trySuspend() const
{
	if (suspending && onSuspend() && !suspended)
	{
		suspended = true;

		if (useProbe)
		{
			timeBufferEmpty->put(static_cast<double>(wns::simulator::getEventScheduler()->getTime() - suspendStartTime));
		}

		si->suspended(this);
	}
} // trySuspend



