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

#ifndef IP_IPTABLES_CHAIN_HPP
#define IP_IPTABLES_CHAIN_HPP

#include <IP/IPHeader.hpp>
#include <IP/iptables/filters/FilterInterface.hpp>
#include <IP/iptables/targets/TargetInterface.hpp>
#include <IP/iptables/Rule.hpp>
#include <IP/iptables/IRuleControl.hpp>

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/Command.hpp>

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/service/tl/TCPHeader.hpp>

//#include <list>
#include <map>

namespace ip { namespace iptables {

	/**
	 * @brief Implementation of IPv4 routing functionality
	 */
	class Chain:
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		virtual public wns::ldk::FunctionalUnit,
		public ip::iptables::IRuleControl
	{
	public:
		typedef std::list<ip::iptables::Rule> RuleContainer;

		Chain(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _pyco);

		virtual
		~Chain();

		virtual void
		onFUNCreated();

		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr&) const;

		virtual void
		doSendData(const wns::ldk::CompoundPtr&);

		virtual void
		doWakeup();

		virtual void
		doOnData(const wns::ldk::CompoundPtr&);

		targets::TargetResult
        activateChain(IPCommand*, wns::service::tl::ITCPHeader*, wns::service::tl::IUDPHeader*);

		/**
		 * @brief Derived Classed need to decide here if the IP packet is to be
		 * processed in the outgoing path
		 */
		virtual bool
		activateOutgoing(const IPCommand&) = 0;

		/**
		 * @brief Derived Classed need to decide here if the IP packet is to be
		 * processed in the incoming path
		 */
		virtual bool
		activateIncoming(const IPCommand&) = 0;


		/**
		 * @brief Implementation of IRuleControl.
		 */

		/**
		 * @brief Adds Rule to the RuleContainer.
		 */
		virtual void
		addRule(ip::iptables::Rule);

		/**
		 * @brief Removes all Rules from the RuleContainer with ruleTag ruleTag.
		 */
		virtual void
		removeRules(ip::iptables::Rule::RuleTag ruleTag);

		virtual bool
		hasRules(ip::iptables::Rule::RuleTag ruleTag);

		virtual ip::iptables::Rule
		getRule(ip::iptables::Rule::RuleTag ruleTag);

		virtual void
		showRules();

	protected:
		/**
		 * @brief The logger for this component.
		 */
		wns::logger::Logger log;

	private:
		wns::ldk::CommandReaderInterface* ipHeaderReader;
		RuleContainer rules;
	};


}// iptables
}// ip

#endif // IP_IPTABLES_CHAIN_HPP
