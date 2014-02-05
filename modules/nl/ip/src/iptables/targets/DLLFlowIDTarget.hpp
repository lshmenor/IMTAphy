/******************************************************************************
 * IPModule   Internet Protocol Implementation                                *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 ******************************************************************************/

#ifndef IP_IPTABLES_TARGETS_DLLFLOWIDTARGET_HPP
#define IP_IPTABLES_TARGETS_DLLFLOWIDTARGET_HPP

#include <IP/IPHeader.hpp>
#include <IP/iptables/targets/TargetInterface.hpp>
#include <IP/iptables/IRuleControl.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace ip { namespace iptables { namespace targets {

	class DLLFlowIDTarget:
		public TargetInterface
	{
	public:

		DLLFlowIDTarget(const wns::pyconfig::View& _pyco);

		virtual
		~DLLFlowIDTarget();

        virtual TargetResult
        mangle(IPCommand*);

        virtual TargetResult
        mangle(IPCommand*, wns::service::tl::IUDPHeader*);

        virtual TargetResult
        mangle(IPCommand*, wns::service::tl::ITCPHeader*);

		void
		setRuleController(ip::iptables::IRuleControl* controller);

		void
		ruleErased(wns::service::dll::FlowID flowID);

	private:

		bool
		knowsTLFlowID(wns::service::tl::FlowID tlFlowID);

		wns::service::dll::FlowID
		getDLLFlowID(wns::service::tl::FlowID tlFlowID);

		typedef wns::container::Registry<wns::service::dll::FlowID, wns::service::tl::FlowID> dllFlowIDList;
		wns::logger::Logger log;
		ip::iptables::IRuleControl* postroutingChain;
		dllFlowIDList knowsFlowIDList;
	};

} // targets
} // iptables
} // ip
#endif // IP_IPTABLES_TARGETS_DLLFLOWIDTARGET_HPP
