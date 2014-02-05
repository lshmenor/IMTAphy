/******************************************************************************
 * UDP ClientListenerBinding                                                  *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 ******************************************************************************/

#ifndef CONSTANZE_UDPCLIENTLISTENERBINDING_HPP
#define CONSTANZE_UDPCLIENTLISTENERBINDING_HPP

#include <CONSTANZE/Binding.hpp>
#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/Listener.hpp>

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/Service.hpp>
#include <WNS/service/tl/ConnectionHandler.hpp>
#include <WNS/service/tl/DataHandler.hpp>
#include <WNS/service/tl/Connection.hpp>
#include <WNS/service/tl/PortPool.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/module/Base.hpp>


namespace constanze
{
    class UdpClientListenerBinding:
        public Binding,
        public wns::service::tl::ConnectionHandler,
        public wns::service::tl::DataHandler
    {
    public:
        UdpClientListenerBinding(int _listenPort);
        virtual ~UdpClientListenerBinding() {};

        void
        registerUdpService(wns::service::tl::Service* _udpService);

        // from Binding
        virtual void
        registerComponent(wns::node::component::Component* _component);

        virtual void
        sendData(const wns::osi::PDUPtr& _data);

        virtual void
        initBinding(constanze::StartTrigger* _trgger);

        virtual void
        releaseBinding(constanze::StopTrigger* _trigger);

        virtual void
        registerListener(constanze::Listener* _listener);

        // from ConnectionHandler

        virtual void
        onConnectionEstablished(wns::service::nl::Address _sourceIpAddress, wns::service::tl::Connection* _newConnection);

        virtual void
        onConnectionClosed(wns::service::tl::Connection* _connection);

        virtual void
        onConnectionClosedByPeer(wns::service::tl::Connection* _connection);

        virtual void
        onConnectionLost(wns::service::tl::Connection* _connection);

        // from DataHandler
        virtual void
        onData(const wns::osi::PDUPtr& _data);

    protected:
        wns::service::nl::Address ownAddress;

        wns::service::tl::Port listenPort;

        wns::service::tl::Service* udpService;

        wns::service::tl::ConnectionHandler* connectionHandler;

        constanze::Listener* listener;

        wns::node::component::Component* component;



        /**
		 * @brief The Logger instance.
		 */
        wns::logger::Logger log;

        virtual std::string printAddress() const;

    };
} //constanze

#endif
