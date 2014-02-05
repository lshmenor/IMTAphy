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

#ifndef WNS_LDK_CRC_CRC_HPP
#define WNS_LDK_CRC_CRC_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/distribution/Uniform.hpp>

namespace wns { namespace ldk { namespace crc {

    class CRCCommand :
        public Command
    {
    public:
        CRCCommand()
        {
            local.checkOK = false;
        }

        /*
         * I - Information Frame
         * RR - Receiver Ready (ACK)
         */
        struct
        {
            bool checkOK;
        } local;
        struct
        {
        }
        peer;
        struct
        {
        }
        magic;

    };


    /**
     * @brief CRC implementation of the FU interface.
     *
     */
    class CRC :
        public fu::Plain<CRC, CRCCommand>,
        virtual public SuspendableInterface,
        public SuspendSupport
    {
    public:
        // FUNConfigCreator interface realisation
        CRC(fun::FUN* fuNet, const wns::pyconfig::View& config);
        ~CRC();

        virtual void onFUNCreated();

        // SDU and PCI size calculation
        void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

        bool
        isMarking() const;

    private:
        virtual void
        doSendData(const CompoundPtr& compound);

        virtual void
        doOnData(const CompoundPtr& compound);

        virtual bool
        doIsAccepting(const CompoundPtr& compound) const;

        virtual void
        doWakeup()
        {
            getReceptor()->wakeup();
        } // wakeup

        virtual bool
        onSuspend() const
        {
            return true;
        } // onSuspend

        /**
         * @brief Pointer to the used Uniform Distribution.
         *
         */
        wns::distribution::StandardUniform dis;

        /**
         * @brief Length of the CRC checksum.
         *
         */
        int checkSumLength;

        std::string  PERProviderName;

        /**
         * @brief behaviour of the CRC, defaults to DROPPING, which means that it
         * does not deliver bad Compounds to the next FUs in the 'incoming' chain. If
         * the behaviour is set to MARKING, the compounds are marked bad in the
         * local part of the CRCCommand and delivered.
         *
         */
        typedef enum { DROPPING, MARKING } Behaviour;
        Behaviour behaviour;

        struct Friends
        {
            FunctionalUnit* PERProvider;
        } friends;

        logger::Logger logger;

        // Probe variables
        wns::probe::bus::ContextCollectorPtr lossRatio;
    };

} // crc
} // ldk
} // wns

#endif // NOT defined WNS_LDK_CRC_CRC_HPP


