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

#include <IP/iptables/targets/DLLFlowIDTaggerTarget.hpp>

using namespace ip::iptables::targets;

DLLFlowIDTaggerTarget::DLLFlowIDTaggerTarget(wns::service::dll::FlowID dllFlowID, wns::service::tl::FlowID tlFlowID):
	myDllFlowID(dllFlowID),
	myTlFlowID(tlFlowID)
{
	myTlFlowID.swapSrcDest();
}

DLLFlowIDTaggerTarget::~DLLFlowIDTaggerTarget()
{
}

TargetResult
DLLFlowIDTaggerTarget::mangle(ip::IPCommand* /*ipHeader*/)
{
	return CONT;
}

TargetResult
DLLFlowIDTaggerTarget::mangle(ip::IPCommand* ipHeader, wns::service::tl::ITCPHeader* tcpHeader)
{
	if(myTlFlowID == tcpHeader->getFlowID())
	{
		ipHeader->local.dllFlowID = myDllFlowID;
	}
	return CONT;
}

TargetResult
DLLFlowIDTaggerTarget::mangle(ip::IPCommand* ipHeader, wns::service::tl::IUDPHeader* udpHeader)
{
	if(myTlFlowID == udpHeader->getFlowID())
	{
		ipHeader->local.dllFlowID = myDllFlowID;
	}
	return CONT;
}
