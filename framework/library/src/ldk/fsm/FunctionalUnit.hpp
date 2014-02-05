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

#ifndef WNS_LDK_FSM_FUNCTIONALUNIT_HPP
#define WNS_LDK_FSM_FUNCTIONALUNIT_HPP

#include <WNS/ldk/fsm/CompoundHandlerSignalInterface.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>

#include <WNS/fsm/FSM.hpp>

#include <WNS/Exception.hpp>

namespace wns { namespace ldk { namespace fsm {

    /**
     * @brief FSM based FU
     *
     *
     * How to write a FSM based FU:
     *
     * 1.) Decide which variables your states must share
     *
     * 2.) Put these variables in a struct (we assume Variables for now):
     * @code
     * struct Variables
     * {
     *     int a;
     *     int b;
     * }
     * @endcode
     *
     * 3.) Define the interface of your new FU:
     * @code
     * #include <WNS/ldk/fsm/FunctionalUnit.hpp>
     * typedef ldk::fsm::FunctionalUnit<Variables> MyFUInterface;
     * @endcode
     *
     * 4.) Create an implementation of this interface
     * @code
     * class MyFU :
     *     public MyFUInterface
     * ...
     * @endcode
     *
     * 5.) Implement the states:
     * @code
     * class WaitingForACK :
     *     public MyFUInterface::UnhandledSignals
     * {
     * public :
     *     WaitingForACK(MyFUInterface::BaseFSM* t) :
     *         MyFUInterface::UnhandledSignals(t)
     *     {}
     * ...
     * @endcode
     *
     * Note that the state is derived from
     * FUInterface::UnhandledSignals. This allows this state object to be
     * used with different implementations of MyFUInterface.
     * Note also that the constructor of the state takes a pointer to
     * MyFUInterface::BaseFSM by convention.
     */
    template <typename VARIABLES>
    class FunctionalUnit :
        virtual public ldk::FunctionalUnit,
        public wns::fsm::FSM<wns::ldk::fsm::CompoundHandlerSignalInterface, VARIABLES>
    {
    public:
        typedef wns::fsm::FSM<wns::ldk::fsm::CompoundHandlerSignalInterface, VARIABLES> BaseFSM;
        class StateInterface :
            public BaseFSM::StateInterface
        {
        public:
            /**
             * @brief Each state will automatically get the FSM it belongs to
             */
            explicit
            StateInterface(BaseFSM* _fu, const std::string& stateName) :
                BaseFSM::StateInterface(_fu, stateName),
                fu(dynamic_cast<FunctionalUnit*>(_fu))
            {
                assureType(_fu, FunctionalUnit*);
            } // StateInterface

            /**
             * @brief Returns the FU this state is part of
             */
            virtual FunctionalUnit*
            getFU() const
            {
                return fu;
            } // getFU

            /**
             * @brief initState is called when entering a new state
             */
            virtual void
            initState()
            {
            } // initState

            /**
             * @brief exitState is called when exiting a state
             */
            virtual void
            exitState()
            {
            } // exitState

        private:
            /**
             * @brief This state is part of this FU
             */
            FunctionalUnit* fu;
        };

        /**
         * @brief Provides default implementation for unhandled signals
         *
         * If a signal is not implemented, the default action is to
         * throw an error.
         */
        class UnhandledSignals :
            public StateInterface
        {
        protected:
            UnhandledSignals(BaseFSM* t, const std::string& stateName) :
                StateInterface(t, stateName)
            {
            } // UnhandledSignals

        private:
            virtual void
            initState()
            {
                doInitState();
            } // initState

            virtual void
            doInitState()
            {
            } // doInitState

            virtual void
            exitState()
            {
                assure(!this->getFU()->inAction, "Trying to change state although current state is still in use.");

                doExitState();
            } // exitState

            virtual void
            doExitState()
            {
            } // doExitState

            virtual typename BaseFSM::StateInterface*
            doSendData(const CompoundPtr&)
            {
                Exception e;
                e << "Can't handle signal (DATAReq) in state: "
                  << wns::TypeInfo::create(*this);
                throw e;

                return this;
            } // doSendData

            virtual typename BaseFSM::StateInterface*
            doOnData(const CompoundPtr&)
            {
                Exception e;
                e << "Can't handle invalid signal (DATAInd) in state: "
                  << wns::TypeInfo::create(*this);
                throw e;

                return this;
            } // doOnData


            virtual typename BaseFSM::StateInterface*
            doWakeup()
            {
                Exception e;
                e << "Can't handle signal (wakeup) in state: "
                  << wns::TypeInfo::create(*this);
                throw e;

                return this;
            } // doWakeup


            virtual void
            doIsAccepting(const CompoundPtr&, bool&) const
            {
                Exception e;
                e << "Can't handle signal (isAccepting) in state: "
                  << wns::TypeInfo::create(*this);
                throw e;
            } // doIsAccepting
        };

        explicit
        FunctionalUnit(const VARIABLES& v) :
            BaseFSM(v),
            inAction(0),
            sendDataCompound(),
            furtherSendDataCompounds(0),
            wakeupFU(0),
            inWakeup(false),
            onDataCompounds()
        {
        } // FunctionalUnit

        void
        queueSendData(const CompoundPtr& compound)
        {
            sendDataCompound = compound;
        } // queueSendData

        void
        queueWakeup()
        {
            ++wakeupFU;
        } // queueWakeup

        void
        queueOnData(const CompoundPtr& compound)
        {
            onDataCompounds.push_back(compound);
        } // queueOnData

        bool
        isAccepting(const CompoundPtr& compound)
        {
            if (sendDataCompound)
            {
                ++furtherSendDataCompounds;
                return false;
            }

            return getConnector()->hasAcceptor(compound);
        } // isAccepting

        virtual void
        doSendData(const CompoundPtr& compound)
        {
            ++inAction;
            CompoundHandlerSignalInterface* stateInterface = this->getState()->doSendData(compound);
            --inAction;
            this->changeState(stateInterface);

                if (linkHandler())
                doWakeup();

            return;
        } // doSendData

        virtual void
        doOnData(const CompoundPtr& compound)
        {
            ++inAction;
            CompoundHandlerSignalInterface* stateInterface = this->getState()->doOnData(compound);
            --inAction;
            this->changeState(stateInterface);

            if (linkHandler())
            doWakeup();

            return;
        } // doOnData

    private:
        virtual void
        doWakeup()
        {
            do
            {
                ++inAction;
                CompoundHandlerSignalInterface* stateInterface = this->getState()->doWakeup();
                --inAction;
                this->changeState(stateInterface);
            }
            while (linkHandler());

            return;
        } // doWakeup

        virtual bool
        doIsAccepting(const CompoundPtr& compound) const
        {
            bool accepting = false;
            this->getState()->doIsAccepting(compound, accepting);
            return accepting;
        } // doIsAccepting

        typedef std::list<CompoundPtr> CompoundContainer;

    protected:
        bool
        linkHandler()
        {
            if (sendDataCompound)
            {
                CompoundPtr sendDataCompoundHelp = sendDataCompound;
                sendDataCompound = CompoundPtr();

                getConnector()->getAcceptor(sendDataCompoundHelp)->sendData(sendDataCompoundHelp);
            }

            if (furtherSendDataCompounds)
            {
                furtherSendDataCompounds = 0;
                return true;
            }

            while (!onDataCompounds.empty())
            {
                CompoundPtr onDataCompoundHelp = onDataCompounds.front();
                onDataCompounds.pop_front();

                getDeliverer()->getAcceptor(onDataCompoundHelp)->onData(onDataCompoundHelp);
            }

            if (wakeupFU && !inWakeup)
            {
                inWakeup = true;
                getReceptor()->wakeup();
                inWakeup = false;
                wakeupFU = 0;
            }

            return false;
        } // linkHandler

    protected:
        int inAction;

    private:
        CompoundPtr sendDataCompound;
        int furtherSendDataCompounds;
        int wakeupFU;
        bool inWakeup;
        CompoundContainer onDataCompounds;

    }; // FunctionalUnit

} // fsm
} // ldk
} // wns

#endif // NOT defined WNS_LDK_FSM_FUNCTIONALUNIT_HPP


