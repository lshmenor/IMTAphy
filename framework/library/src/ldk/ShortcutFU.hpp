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

#ifndef WNS_LDK_SHORTCUTFU_HPP
#define WNS_LDK_SHORTCUTFU_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/distribution/Distribution.hpp>

#include <boost/bind.hpp>

#include <deque>


namespace wns { namespace ldk {
        /**
         * @brief The ShortcutFU provides straight connection of
         * corresponding ShortcutFUs in peer entities.
         * The instances register themselves with their unique
         * addresses available in the module the FU is being placed.
         */
        template<typename ADDRESS, typename ELEMENT>
        //class ShortcutFU : public fu::Plain<ShortcutFU<ADDRESS, ELEMENT> >
        class ShortcutFU : public FunctionalUnit
        {
        public:
            ShortcutFU(fun::FUN* _fun, const wns::pyconfig::View& _pyco) :
                scheduler(wns::simulator::getEventScheduler()),
                logger(_pyco.getView("logger"))
            {
                wns::pyconfig::View distConfig = _pyco.getView("delay");
                std::string pluginName = distConfig.get<std::string>("__plugin__");
                wns::distribution::DistributionCreator* dc = 
                    wns::distribution::DistributionFactory::creator(pluginName);

                delay = dc->create(_pyco.get("delay"));
                offset = _pyco.get<simTimeType>("offset");

                scheduler->scheduleDelay(boost::bind(&ShortcutFU<ADDRESS, ELEMENT>::sendCompoundsInQueue, (ELEMENT)this), offset);
            }

            virtual
            ~ShortcutFU()
            {
                ContainerType& myReg = getElementRegistry();
                for(typename ContainerType::const_iterator it=myReg.begin(); it!=myReg.end(); ++it)
                {
                    if (it->second == this)
                    {
                        myReg.erase(it->first);
                    }
                }
            }

            virtual bool
            doIsAccepting(const wns::ldk::CompoundPtr&) const { return true; }

            virtual void
            doWakeup() {assure(false, "Configuration error: No FU below allowed!");}

            // CompoundHandlerInterface
            virtual void
            doSendData(const wns::ldk::CompoundPtr& _compound)
            {
                assure(_compound, "doSendData called with invalid compound!");

                compoundQueue.push_back(_compound);
            }

            virtual void
            doOnData(const wns::ldk::CompoundPtr& _compound)
            {
                assure(_compound, "doOnData called with invalid compound!");

                MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
                m << ": doOnData(), forwading to higher FU";
                MESSAGE_END();

                getDeliverer()->getAcceptor(_compound)->onData(_compound);
            }

            /**
             * @brief Only receivers will be considered when sending data. Tell me, are you a receiver?
             */
            virtual bool
            isReceiver() = 0;

            /**
             * @brief If you are a receiver, what is your address?
             */
            virtual ADDRESS
            getSourceAddress() = 0;

            /**
             * @brief To whom should I send this compound?
             */
            virtual ADDRESS
            getDestinationAddress(const wns::ldk::CompoundPtr&) = 0;

            /**
             * @brief Shall I send this compound to every receiver?
             */
            virtual bool
            isBroadcast(const wns::ldk::CompoundPtr&) = 0;

            virtual void
            onFUNCreated()
            {
                ContainerType& myReg = getElementRegistry();
                if (isReceiver())
                {
                    myReg.insert(getSourceAddress(), (ELEMENT)this);
                    MESSAGE_SINGLE(NORMAL, logger, "ShortcutFU "  << this->getFUN()->getName() << " registered with" << " address " << getSourceAddress());
                }
            }

            void
            sendCompoundsInQueue()
            {
                while (not compoundQueue.empty())
                {
                    const wns::ldk::CompoundPtr compound = compoundQueue.front();
                    compoundQueue.erase(compoundQueue.begin());

                    if (isBroadcast(compound))
                    {
                        typename ContainerType::const_iterator it;
                        for(it = getElementRegistry().begin();
                            it != getElementRegistry().end();
                           ++it)
                        {
                            it->second->doOnData(compound);
                        }
                        continue;
                    }

                    ADDRESS destAddr = getDestinationAddress(compound);
                    ELEMENT peerFU = getElementRegistry().find(destAddr);

                    peerFU->doOnData(compound);
                }

                scheduler->scheduleDelay(boost::bind(&ShortcutFU<ADDRESS, ELEMENT>::sendCompoundsInQueue, (ELEMENT)this), (*delay)());
            }

            void
            reset()
            {
                getElementRegistry().clear();
                compoundQueue.clear();
            }

            typedef wns::container::Registry<ADDRESS, ELEMENT, wns::container::registry::NoneOnErase> ContainerType;

        private:
            wns::events::scheduler::Interface* scheduler;

            /**
             * @brief  The distribution for transmission delay
             */
            wns::distribution::Distribution* delay;

            /**
             * @brief Offset in [ms] to empty the compoundQueues
             */
            simTimeType offset;

            /**
             * @brief FIFO queue for all compounds to be sent
             */
            std::deque<wns::ldk::CompoundPtr> compoundQueue;

        protected:
            wns::logger::Logger logger;

            ContainerType&
            getElementRegistry() { static ContainerType reg; return reg;}
        };
    }
}

#endif // WNS_LDK_SHORTCUTFU_HPP
