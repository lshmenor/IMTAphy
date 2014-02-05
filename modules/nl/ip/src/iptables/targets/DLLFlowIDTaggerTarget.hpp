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

#ifndef IP_IPTABLES_TARGETS_DLLFLOWIDTAGGERTARGET_HPP
#define IP_IPTABLES_TARGETS_DLLFLOWIDTAGGERTARGET_HPP

#include <IP/IPHeader.hpp>
#include <IP/iptables/targets/TargetInterface.hpp>
#include <IP/iptables/IRuleControl.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace ip { namespace iptables { namespace targets {

	class DLLFlowIDTaggerTarget:
		public TargetInterface
	{
	public:

		DLLFlowIDTaggerTarget(wns::service::dll::FlowID, wns::service::tl::FlowID);

		virtual
		~DLLFlowIDTaggerTarget();

        virtual TargetResult
        mangle(IPCommand*);

        virtual TargetResult
        mangle(IPCommand*, wns::service::tl::IUDPHeader*);

        virtual TargetResult
        mangle(IPCommand*, wns::service::tl::ITCPHeader*);

	private:
		wns::service::dll::FlowID myDllFlowID;
		wns::service::tl::FlowID myTlFlowID;
	};

} // targets
} // iptables
} // ip
#endif // IP_IPTABLES_TARGETS_DLLFLOWIDTAGGERTARGET_HPP
