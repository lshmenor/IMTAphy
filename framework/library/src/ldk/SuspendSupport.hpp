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

#ifndef WNS_LDK_SUSPENDSUPPORT_HPP
#define WNS_LDK_SUSPENDSUPPORT_HPP

#include <WNS/ldk/SuspendableInterface.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

namespace wns { namespace ldk {

	namespace fun {
		class FUN;
	} // fun

	class SuspendedInterface;

	class SuspendSupport :
		virtual public SuspendableInterface
	{
	public:
		SuspendSupport(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _config);

		SuspendSupport(const SuspendSupport& other);

		virtual
		~SuspendSupport();

		virtual void
		doSuspend(SuspendedInterface* _si);

		virtual bool
		isSuspending() const;

		virtual void
		trySuspend() const;

		virtual bool
		onSuspend() const = 0;

	private:
		bool suspending;
		mutable bool suspended;
		SuspendedInterface* si;

		double suspendStartTime;

		bool useProbe;

		wns::probe::bus::ContextCollectorPtr timeBufferEmpty;

		wns::ldk::fun::FUN* fun;
		wns::pyconfig::View config;
	}; // SuspendSupport

} // ldk
} // wns

#endif // WNS_LDK_SUSPENDSUPPORT_HPP


