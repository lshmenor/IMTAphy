/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <TCP/FlowHandler.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/StaticFactory.hpp>

using namespace tcp;

FlowHandler::FlowHandler()
{
}

void
FlowHandler::onFlowEstablished(wns::service::tl::FlowID flowID, int _dllFlowID)
{
	tlService->openConnectionContinue(flowID, _dllFlowID);
}

void
FlowHandler::onFlowChanged(wns::service::tl::FlowID flowID, int _dllFlowID)
{
	tlService->dllFlowChanged(flowID, _dllFlowID);
}

void
FlowHandler::setTLService(Service* _tlService)
{
	tlService = _tlService;
}
