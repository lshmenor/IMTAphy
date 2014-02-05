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

#ifndef WNS_SERVICE_DLL_HANDLER_HPP
#define WNS_SERVICE_DLL_HANDLER_HPP

#include <WNS/service/dll/ProtocolNumber.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/service/tl/FlowID.hpp>
#include <WNS/service/dll/FlowID.hpp>

namespace wns { namespace service { namespace dll {
      /**
       * @brief DLL DataHandler interface.
       *
       * DLL components need a callback to deliver incoming data. Users of
       * @ref wns::service::dll::DataTransmission can register such a callback
       * by calling @ref
       * wns::service::dll::DataTransmission::registerDataHandler and passing
       * an implementation of @ref wns::service::dll::DataHandler
       */
      class Handler
      {
      public:
	/**
	 * @brief Destructor
	 */
	virtual ~Handler() {};

	/**
	 * @brief Receive data from lower DLL layer.
	 *
	 * @param[in] _pdu Data that is delivered to the DataHandler.
	 */
	virtual void
	onData(const wns::osi::PDUPtr& _pdu, wns::service::dll::FlowID _dllFlowID = wns::service::dll::NoFlowID) = 0;
      };

      /**
       * @brief TL FlowHandler interface.
       *
       * TL components need a callback to notify the establishment of a flow
       * to higher layers. Users of
       * @ref wns::service::dll::FlowEstablishmentAndRelease can register such
       * a callback by calling
       * @ref wns::service::dll::FlowEstablishmentAndRelease::registerFlowHandler
       * and passing an implementation of @ref wns::service::dll::FlowHandler
       */

      class FlowHandler
      {
      public:
	/**
	 * @brief Destructor
	 */
	virtual ~FlowHandler() {}

	/**
	 * @brief Receive notificiation about flow establishment from
	 * lower DLL layer.
	 *
	 * @param[in] source/destination ports and IP adresses belonging
	 * to this flow.
	 */
	virtual void
	onFlowEstablished(wns::service::tl::FlowID /*flowID*/, wns::service::dll::FlowID /*_dllFlowID*/)=0;

	virtual	void
	onFlowChanged(wns::service::tl::FlowID /*flowID*/, wns::service::dll::FlowID /*_dllFlowID*/)=0;
      };


      class IRuleControl
      {
      public:
	/**
	 * @brief Destructor
	 */
	virtual ~IRuleControl() {}

	/**
	 * @brief Receive notificiation about changed Flows from
	 * lower DLL layer to use for IPTables
	 */
	virtual void
	onFlowRemoved(wns::service::dll::FlowID /*flowID*/)=0;
      };


      /**
       * @brief DLL Notification interface.
       *
       * wns::service::dll::Notification allows users to register the Handler.
       */
      class Notification :
    virtual public wns::service::Service
      {
      public:
	/*
	 * @brief Registration of DataHandlers of the upper layer.
	 *
	 * @param[in] _protocol Protocol Number this _handler is responsible for
	 *
	 * @param[in] _handler An implementation of the Handler interface.
	 * Will receive PDUs of the given protocol.
	 */
	virtual void
	registerHandler(protocolNumber protocol, Handler* handler) = 0;

	/*
	 * @brief Registration of FlowHandlers of higher layers.
	 *
	 * @param[in] _flowHandler An implementation of the FlowHandler interface.
	 * Will receive notification about established flow.
	 */
	virtual void
	registerFlowHandler(FlowHandler* _flowHandler) = 0;

	/*
	 * @brief Registration of IRuleControl of IP.
	 *
	 */
	virtual void
	registerIRuleControl(IRuleControl* _iRuleControl) = 0;
      };
    } // dll
  } // service
} // wns

#endif // WNS_SERVICE_DLL_HANDLER_HPP
