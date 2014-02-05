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

#ifndef TCP_TCPHEADER_HPP
#define TCP_TCPHEADER_HPP

#include <WNS/service/tl/TCPHeader.hpp>
#include <WNS/ldk/Forwarding.hpp>
#include <WNS/ldk/fu/Plain.hpp>

namespace tcp {

    class TCPCommand:
        public wns::ldk::Command,
        public wns::service::tl::ITCPHeader
    {
     public:
         TCPCommand()
             {
                 peer.urg = false;
                 peer.ack = false;
                 peer.psh = false;
                 peer.rst = false;
                 peer.syn = false;
                 peer.fin = false;
             }

         struct {} local;
         struct {
             wns::service::tl::FlowID flowID;
             bool urg;
             bool ack;
             bool psh;
             bool rst;
             bool syn;
             bool fin;
         } peer;
         struct {} magic;

        const wns::service::tl::FlowID& getFlowID() const { return peer.flowID; }

        wns::service::tl::FlowID& getFlowID() { return peer.flowID; }

        const bool& getUrgentFlag() const { return peer.urg; }

        bool& getUrgentFlag() { return peer.urg; }

        const bool& getAckFlag() const { return peer.ack; }

        bool& getAckFlag() { return peer.ack; }

        const bool& getPushFlag() const { return peer.psh; }

        bool& getPushFlag() { return peer.psh; }

        const bool& getResetFlag() const { return peer.rst; }

        bool& getResetFlag() { return peer.rst; }

        const bool& getSynFlag() const { return peer.syn; }

        bool& getSynFlag() { return peer.syn; }

        const bool& getFinFlag() const { return peer.fin; }

        bool& getFinFlag() { return peer.fin; }
     };

    /**
     * @brief Dummy Functional Unit that only provides the TCP Header
     *
     */
    class TCPHeader :
        public wns::ldk::Forwarding<TCPHeader>,
        public wns::ldk::fu::Plain<TCPHeader, TCPCommand>
    {
    public:
        TCPHeader(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

        virtual ~TCPHeader() {}

        wns::ldk::CommandPool*
        createReply(const wns::ldk::CommandPool* original) const;

        virtual void
        calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

    private:
        /**
		 * @brief Allow for changing the TCP-header size from the default
		 *        20*8b (UDP header: 8*8bit)
         */
        Bit headerSize;



    };
} // tcp


#endif // TCP_TCPHEADER_HPP
