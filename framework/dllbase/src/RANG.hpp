/******************************************************************************
 * DLLBase (DLL Base classes to create FUN-based DLL)                         *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2006                                                         *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef DLL_RANG_HPP
#define DLL_RANG_HPP

#include <WNS/logger/Logger.hpp>

#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/service/dll/FlowID.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/container/Registry.hpp>

#include <WNS/osi/PDU.hpp>

namespace dll {
    /**
     * @brief RANG Radio Access Network Gateway.
     *
     * This is the gateway to the core network for all APs.  The RANG is
     * connected to the Network Layer (IP) of the server. The APs don't
     * have an IP component, they terminate with the Layer2. So, the
     * communication between the server and an AP goes via the RANG. The
     * RANG has the funtion of a router, i.e. it forwards outgoing packets
     * to the correct AP's DLL. From the Viewpoint of the server's NL, it
     * behaves like a DLL itself
     */
    class RANG :
        virtual public wns::service::dll::UnicastDataTransmission,
        virtual public wns::service::dll::Notification,
        virtual public wns::service::dll::Handler,
        public wns::node::component::Component
    {
        typedef wns::container::Registry<wns::service::dll::UnicastAddress, wns::service::dll::UnicastDataTransmission*> AccessPointLookup;
    public:
        RANG(wns::node::Interface*, const wns::pyconfig::View&);
        virtual ~RANG(){};

        virtual void
        registerFlowHandler(wns::service::dll::FlowHandler*){};

        virtual void
        registerIRuleControl(wns::service::dll::IRuleControl*){};

        /** @name wns::service::dll::DataTransmission service */
        //@{
        virtual void
        sendData(
            const wns::service::dll::UnicastAddress& _peer,
            const wns::osi::PDUPtr& _data,
            wns::service::dll::protocolNumber protocol,
            wns::service::dll::FlowID _dllFlowID = wns::service::dll::NoFlowID);
        //@}

        virtual wns::service::dll::UnicastAddress
        getMACAddress() const
            {
            return wns::service::dll::UnicastAddress();
            }


        /** @name wns::service::dll::Notification service */
        //@{
        virtual void
        registerHandler(wns::service::dll::protocolNumber protocol,
                wns::service::dll::Handler* _dh);
        //@}

        /** @name wns::service::dll::Handler Interface */
        //@{
        /** @brief standard onData method */
        virtual void onData(const wns::osi::PDUPtr& _data,
                    wns::service::dll::FlowID _dllFlowID = wns::service::dll::NoFlowID);

        /** @brief Modified Handler Interface for APs.
         *
         * Modified Handler Interface for APs. It gets two parameters in
         * addition to the PDUPtr to keep the RANG's routing table up to
         * date. */
        bool knowsAddress(wns::service::dll::UnicastAddress _sourceMACAddress);

        wns::service::dll::UnicastDataTransmission*
        getAccessPointFor(wns::service::dll::UnicastAddress _sourceMACAddress);

        void updateAPLookUp(wns::service::dll::UnicastAddress _sourceMACAddress,
                wns::service::dll::UnicastDataTransmission* _ap);

        void removeAddress(wns::service::dll::UnicastAddress _sourceMACAddress,
                           wns::service::dll::UnicastDataTransmission* _ap);

        virtual
        void onData(const wns::osi::PDUPtr& _data,
                  wns::service::dll::UnicastAddress _sourceMACAddress,
                  wns::service::dll::UnicastDataTransmission* _ap,
                  wns::service::dll::FlowID _dllFlowID = wns::service::dll::NoFlowID);
        //@}

        /** @name wns::node::component::Component Interface */
        //@{
        virtual void onNodeCreated();
        virtual void onWorldCreated();
        virtual void onShutdown();
        //@}

    protected:
        virtual void
        doStartup();
        AccessPointLookup accessPointLookup;

    private:
        /**
         * @brief Needed for demultiplexing of upper layer protocols.
         */
        typedef wns::container::Registry<wns::service::dll::protocolNumber, wns::service::dll::Handler*> DataHandlerRegistry;

        /**
         * @brief Registry for datahandlers. Each datahandler is select
         * by the protocol number.
         */
        DataHandlerRegistry dataHandlerRegistry;

        wns::pyconfig::View config;

        bool learnAPfromIncomingData;

        wns::logger::Logger logger;
    };
}

#endif // NOT defined DLL_RANG_HPP
