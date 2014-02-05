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

#ifndef WNS_LDK_ARQ_PIGGYBACKER_HPP
#define WNS_LDK_ARQ_PIGGYBACKER_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/arq/ARQ.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>


namespace wns { namespace ldk { namespace arq {
    class PiggyBackerCommand :
        public Command
    {
    public:
        PiggyBackerCommand()
        {
            peer.piggyBacked = CompoundPtr();
        }
        ~PiggyBackerCommand()
        {
        }

        struct
        {
        }
        local;

        struct
        {
            CompoundPtr piggyBacked;
        } peer;
        struct
        {
        }
        magic;
    };


    class PiggyBacker :
        public wns::ldk::fu::Plain<PiggyBacker, PiggyBackerCommand>
    {
    public:
        PiggyBacker(fun::FUN* fuNet, const pyconfig::View& config);
        virtual void onFUNCreated();

        virtual void doSendData(const CompoundPtr& compound);
        virtual void doOnData(const CompoundPtr& compound);
        virtual void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const;

    private:
        virtual bool doIsAccepting(const CompoundPtr& compound) const;
        virtual void doWakeup();

        std::string arqName;
        Bit bitsIfPiggyBacked;
        Bit bitsIfNotPiggyBacked;
        bool addACKPDUSize;

        struct Friends
        {
            ARQ* arq;
        }
        friends;

        CompoundPtr i;
        CompoundPtr rr;

        bool inControl;

        logger::Logger logger;

        void tryToSend();
        ARQCommand* getARQPCI(const CompoundPtr& compound) const;
    };

}
}
}


#endif // NOT defined WNS_LDK_ARQ_PIGGYBACKER_HPP


