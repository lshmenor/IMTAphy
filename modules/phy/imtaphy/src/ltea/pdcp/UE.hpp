/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

// based on code from openWNS with the following license:

/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#ifndef LTEA_PDCP_UE_HPP
#define LTEA_PDCP_UE_HPP

#include <DLL/UpperConvergence.hpp>
#include <DLL/Layer2.hpp>

#include <WNS/ldk/FunctionalUnit.hpp>


namespace ltea { namespace pdcp {

class PDCPUE:
    virtual public wns::ldk::FunctionalUnit,
    virtual public dll::UTUpperConvergence
{
public:

  PDCPUE(wns::ldk::fun::FUN* fun,
		     const wns::pyconfig::View& config);

  void
  sendData(const wns::service::dll::UnicastAddress& _peer,
	   const wns::osi::PDUPtr& pdu,
	   wns::service::dll::protocolNumber protocol,
	   wns::service::dll::FlowID bearerID = 0);

  void
  processIncoming(const wns::ldk::CompoundPtr& compound);


  void
  onFUNCreated();

  virtual void
  registerHandler(wns::service::dll::protocolNumber protocol,
		  wns::service::dll::Handler* _dh);

  void
  registerFlowHandler(wns::service::dll::FlowHandler* flowHandler);

  void
  establishFlow(wns::service::tl::FlowID flowID, wns::service::qos::QoSClass qosClass);

/*
  void
  releaseFlow(wns::service::tl::FlowID flowID);

  void
  onFlowBuilt(wns::service::tl::FlowID _flowID,
	      wns::service::dll::FlowID _dllFlowID,
	      bool newFlow);
*/
          
private:

  wns::ldk::fun::FUN* fun;

  wns::ldk::CommandReaderInterface* pdcpReader;
  
  dll::ILayer2* layer2;
  
//  lte::controlplane::flowmanagement::IFlowManagerUE* flowManager;
  
  wns::service::dll::FlowHandler* tlFlowHandler;
};

}}

#endif 