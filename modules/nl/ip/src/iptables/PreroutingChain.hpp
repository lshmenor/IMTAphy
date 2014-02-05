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

#ifndef IP_IPTABLES_PREROUTINGCHAIN_HPP
#define IP_IPTABLES_PREROUTINGCHAIN_HPP

#include <WNS/ldk/fun/FUN.hpp>
#include <IP/iptables/Chain.hpp>
#include <IP/iptables/IRuleControl.hpp>
#include <IP/iptables/targets/TargetInterface.hpp>
#include <IP/iptables/targets/DLLFlowIDTarget.hpp>

namespace ip { namespace iptables {

	class PreroutingChain:
		public Chain,
		public wns::Cloneable<PreroutingChain>
	{

	public:
		PreroutingChain(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _pyco);

		virtual void
		onFUNCreated();

		virtual bool
		activateOutgoing(const IPCommand&) { return false; }

		virtual bool
		activateIncoming(const IPCommand&) { return true; }

	private:
		wns::ldk::fun::FUN* fun;
		ip::iptables::IRuleControl* postroutingChain;
		wns::logger::Logger log;
		ip::iptables::targets::DLLFlowIDTarget* dllFlowIDTarget;
	};

} // iptables
} // ip

#endif // IP_IPTABLES_PREROUTINGCHAIN_HPP
