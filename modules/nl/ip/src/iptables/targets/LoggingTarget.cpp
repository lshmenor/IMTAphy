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

#include <IP/iptables/targets/LoggingTarget.hpp>

using namespace ip::iptables::targets;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	LoggingTarget,
	TargetInterface,
	"loggingTarget",
	wns::PyConfigViewCreator);

LoggingTarget::LoggingTarget(const wns::pyconfig::View& _pyco):
	log(_pyco.get("logger"))
{
}

LoggingTarget::~LoggingTarget()
{
}

TargetResult
LoggingTarget::mangle(ip::IPCommand* ipHeader)
{
	MESSAGE_BEGIN(NORMAL, log, m, "");
	m << *ipHeader;
	MESSAGE_END();

	return CONT;
}

TargetResult
LoggingTarget::mangle(ip::IPCommand* ipHeader, wns::service::tl::ITCPHeader* tcpHeader)
{
	MESSAGE_BEGIN(NORMAL, log, m, "");
	m << *ipHeader;
    m << " TCP : " << tcpHeader->getFlowID();
    if (tcpHeader->getUrgentFlag())
    {
        m << " URG";
    }

    if (tcpHeader->getAckFlag())
    {
        m << " ACK";
    }
    if (tcpHeader->getPushFlag())
    {
        m << " PSH";
    }
    if (tcpHeader->getSynFlag())
    {
        m << " SYN";
    }
    if (tcpHeader->getFinFlag())
    {
        m << " FIN";
    }

	MESSAGE_END();

	return CONT;
}

TargetResult
LoggingTarget::mangle(ip::IPCommand* ipHeader, wns::service::tl::IUDPHeader* udpHeader)
{
	MESSAGE_BEGIN(NORMAL, log, m, "");
	m << *ipHeader;
    m << " UDP : " << udpHeader->getFlowID();
	MESSAGE_END();

	return CONT;
}
