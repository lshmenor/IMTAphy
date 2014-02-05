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

#ifndef WNS_FSM_FSM_HPP
#define WNS_FSM_FSM_HPP

#include "FSMConfigCreator.hpp"

#include <WNS/StaticFactory.hpp>
#include <WNS/TypeInfo.hpp>
#include <WNS/Assure.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <string>

#include <iostream>

namespace wns { namespace fsm {
    /**
     * @brief Finite State Machines for C++
     *
     * A Finite State Machine (FSM) is typically described by the signals it
     * can receive, the variables it holds and possible states it might have
     * at runtime. This template defines an FSM with two parameters: SIGNALS
     * and VARIABLES
     *
     * SIGNALS should be an abstract interface defining the signals the FSM
     * can take. Each signal is represented by a method of the interface.
     *
     * VARIABLES should be a struct/class containing the variables of the
     * FSM.
     *
     * The different states of this FSM are represented by different classes
     * (or better objects of different classes).
     *
     * @sa @ref HowToWriteFiniteStateMachine
     */

    template <typename SIGNALS, typename VARIABLES>
    class FSM
    {
    public:
        /**
         * @brief Type of the Variables
         */
        typedef VARIABLES VariablesType;

        /**
         * @brief Export the state interface to let others derive from this :)
         */
        class StateInterface :
            virtual public SIGNALS
        {
        private:
            template <class> friend class wns::fsm::FSMConfigCreator;

            /**
             * @brief The basic FSM this state will operate on
             */
            typedef FSM<SIGNALS, VARIABLES> FSMType;

        public:
            /**
             * @brief Each state will automatically get the FSM it belongs to
             */
            explicit
                StateInterface(FSMType* _fsm, const std::string& _stateName) :
                fsm(_fsm),
                stateName(_stateName)
            {
            }

            /**
             * @brief initState is called when entering a new state
             */
            virtual void
            initState()
            {
            }

            /**
             * @brief exitState is called when exiting a state
             */
            virtual void
            exitState()
            {
            }

            /**
             * @brief Returns the name of the current state
             */
            std::string
            getStateName() const
            {
                return stateName;
            }

            /**
             * @brief Returns a reference to the VARIABLES of the FSM
             *
             * @note: This function is provided for convenience only.
             */
            VariablesType&
            vars()
            {
                return getFSM()->getVariables();
            }

        protected:
            /**
             * @brief Returns the FSM this state is part of
             */
            virtual FSMType*
            getFSM() const
            {
                return fsm;
            }

        private:
            /**
             * @brief This state is part of this FSM
             */
            FSMType* fsm;

            /**
             * @brief Name of current state
             */
            std::string stateName;
        };

        /**
         * @brief Needs to be initialized with an object of VARIABLES
         *
         * @note VARIABLES v will be COPIED(!!) into the FSM.
         */
        explicit
        FSM(const VariablesType& v) :
            currentState(NULL),
            variables(v),
            logger("WNS", TypeInfo::create(*this).toString()),
            stateCreated(false)
        {
        }

        /**
         * @brief Destructor
         *
         * @note If VARIABLES is a POINTER(!!) to a struct, it will not be
         * deleted.
         */
        virtual
        ~FSM()
        {
        }

    public:
        /**
         * @brief Creates the state the FSM shall change to
         */
        template <typename NEWSTATE>
        StateInterface*
        createState()
        {
            assure(!stateCreated, "A new state has been created already.");

            stateCreated = true;
            return new NEWSTATE(this);
        }

    private:
        /**
         * @brief StateFactory used for creation of states
         */
        typedef FSMConfigCreator<StateInterface> StateCreator;
        typedef wns::StaticFactory<StateCreator> StateFactory;

    public:
        /**
         * @brief Creates the state the FSM shall change to
         */
        StateInterface*
        createState(const std::string& stateName)
        {
            if (currentState && currentState->getStateName() == stateName)
                return currentState;

            assure(!stateCreated, "A new state has been created already.");

            stateCreated = true;
            return StateFactory::creator(stateName)
                ->create(this);
        }

        /**
         * @brief Returns a reference to the VARIABLES of the FSM
         *
         * This is to be used in the state objects in order to modify the
         * variables of the FSM.
         */
        VariablesType&
        getVariables()
        {
            return variables;
        }

        /**
         * @brief Returns the name of the current state
         *
         * May be used for testing purposes.
         */
        std::string
        getStateName() const
        {
            return getState()->getStateName();
        }

        /**
         * @brief Replaces the current state of the FSM
         */
        void
        replaceState(SIGNALS* newState)
        {
            assureType(newState, StateInterface*);
            StateInterface* si = dynamic_cast<StateInterface*>(newState);

            if (si == currentState)
                throw wns::Exception("replaceState(...) called with current state.");

            if (currentState)
            {
                MESSAGE_BEGIN(NORMAL, logger, m, "State replacement: ");
                m << wns::TypeInfo::create(*currentState)
                  << " -> "
                  << wns::TypeInfo::create(*si);
                MESSAGE_END();

                delete currentState;
            }

            stateCreated = false;

            currentState = si;
            currentState->initState();
        }

    protected:
        /**
         * @brief Returns the current state
         */
        StateInterface*
        getState() const
        {
            assure(currentState, "FSM not initialized. Please set state berfore sending signals.");
            return currentState;
        }

        /**
         * @brief Changes the state of the FSM (if necessary)
         */
        void
        changeState(SIGNALS* newState)
        {
            assureType(newState, StateInterface*);
            StateInterface* si = dynamic_cast<StateInterface*>(newState);

            if (si == currentState)
                return;

            if (currentState)
            {
                currentState->exitState();

                MESSAGE_BEGIN(NORMAL, logger, m, "State transition: ");
                m << wns::TypeInfo::create(*currentState)
                  << " -> "
                  << wns::TypeInfo::create(*si);
                MESSAGE_END();

                delete currentState;
            }

            stateCreated = false;

            currentState = si;
            currentState->initState();
        }

    private:
        /**
         * @brief Current State of the FSM
         */
        StateInterface* currentState;

        /**
         * @brief Variables of the FSM
         */
        VariablesType variables;

        /**
         * @brief Logger
         */
        logger::Logger logger;

        /**
         * @brief Helper variable
         */
        bool stateCreated;

    }; // FSM

} // fsm
} // wns

#endif // WNS_FSM_FSM_HPP

/**
 * @page HowToWriteFiniteStateMachine How to write a Finite State Machine
 *
 * A simple example should clarify how to implement a finite state machine based
 * on the wns::fsm::FSM template. A finite state machine for a light control,
 * which switches the light at night on if a movement was detected and switches
 * it off again after a certain timer has elapsed should be designed. The
 * following sections will show the necessary steps to implement the ligt
 * control based on wns::fsm::FSM. The following sub-sections represent the
 * necessary steps:
 *
 * <ul>
 * <li>@ref FSM_Modelling</li>
 * <li>@ref FSM_SignalInterface</li>
 * <li>@ref FSM_Variables</li>
 * <li>@ref FSM_Interface</li>
 * <li>@ref FSM_Implementation</li>
 * <li>@ref FSM_States</li>
 * </ul>
 *
 * @section FSM_Modelling Modelling the FSM Light Control
 *
 * @b Recipe: Find a model which describes the problem based on a finite state
 * machine
 *
 * First of all we need to find a model for the finite state machine. We know
 * that the Lamp Control should turn on the lamp if a movement was detected and
 * if it's currently night. Thinking about this we can derive that we need at
 * least three states for the FSM to represent the different behaviours: Day,
 * Night, LampOn, since the FSM must react different to input if it's day or
 * night and different again, if the light is currently turned on (e.g., at
 * day the light must not be turned on if a movement was detected).
 *
 * At run time the finite state machine will be in one of these three
 * states. Being in these states it needs to react to certain signals (input)
 * with some action. The action may also include a state change (e.g., the
 * finite state machine is in state "Night" and the signal "light" arrives, the
 * state would change to "Day"). The signals needed here are: dark (to switch from
 * Day to Night), light (to switch from Nigth to Day), movementDetcted (to switch the
 * Lamp on) and timerElapsed (to switch the lamp off after some time). With
 * these states and signals, the model of our finite state machine "Lamp Control"
 * looks like this:
 *
 * @dot
 * digraph finite_state_machine {
 *   node [shape = circle, fontname=Helvetica, fontsize=12 ]; Day Night LampOn;
 *   rankdir=LR;
 *   Day -> Night [ label = "dark", fontname=Helvetica, fontsize=9 ]
 *   Day -> Day [ label = "movementDetcted", fontname=Helvetica, fontsize=9 ]
 *   Day -> Day [ label = "light", fontname=Helvetica, fontsize=9 ]
 *   Night -> Night [ label = "dark", fontname=Helvetica, fontsize=9 ]
 *   Night -> Day [ label = "light", fontname=Helvetica, fontsize=9 ]
 *   Night -> LampOn [ label = "movementDetcted", fontname=Helvetica, fontsize=9 ]
 *   LampOn -> Night [ label = "timerElapsed", fontname=Helvetica, fontsize=9 ]
 *   LampOn -> Day [ label = "light", fontname=Helvetica, fontsize=9 ]
 *   LampOn -> LampOn [ label = "movementDetcted", fontname=Helvetica, fontsize=9 ]
 *   LampOn -> LampOn [ label = "dark", fontname=Helvetica, fontsize=9 ]
 * }
 * @enddot
 *
 * Summary of the model:
 * <ol>
 *   <li> It has three states:</li>
 *   <ul>
 *     <li> Day </li>
 *     <li> Night </li>
 *     <li> LampOn </li>
 *   </ul>
 *   <li> It has four signals:</li>
 *   <ul>
 *     <li> light </li>
 *     <li> dark </li>
 *     <li> movementDetected </li>
 *     <li> timerElapsed </li>
 *   </ul>
 * </ol>
 *
 * After this model has been identified it needs to be transfered to C++. The
 * framework provided by wns::fsm::FSM will help us to get the model implemented
 * fast and efficiently. Most of the following steps contain a sentence
 * describing in recipe style what needs to be done.
 *
 *
 * @section FSM_SignalInterface An interface (abstract class) to define the signals
 *
 * @b Recipe: Write a class having all signals of the finite state machine as pure
 * virtual functions.
 *
 * The signals have to be modeled as pure virtual methods of a class. Make sure
 * the class is really an interface (does not contain any variables, is
 * stateless). The States classes will be derived from this
 * interface later. The interface for our signals looks like this:
 *
 * @include "FSM::LightControlSignals.example"
 *
 * @note The copy constructor has been disallowed in order to prevent copying of
 * states. The default constructor is protected: since an interface can't be
 * instantiated the constructor can only be called by a derived class.
 *
 * @section FSM_Variables States share Variables
 *
 * @b Recipe: Write a class containing all the variables of the FSM shared by
 * the different states
 *
 * The finite state machine is also characterized by a set of variables which it
 * holds. All these variables have to be implemented in a struct or class which
 * will be used by the wns::fsm::FSM template later. For simplicity our example
 * assumes the class definig the variables is called "Lamp" and contains only
 * one variable called "on":
 *
 * @include "FSM::Lamp.example"
 *
 * @section FSM_Interface An interface for the FSM "Light Control"
 *
 * @b Recipe: Make a handy typedef to the interface of your FSM
 *
 * The template wns::fsm::FSM instantiated with Variables and Signal will not
 * implement the finite state machine itself. It will provide an interface for
 * the final finite state machine. To have a handy alias create a typedef to
 * this interface with the signal interface from above ("LightControlSignals")
 * and the variables that characterize the finite state machine ("Lamp"):
 *
 * @include "FSM::LightControlInterface.example"
 *
 * @section FSM_Implementation Implementation of the finite state machine
 *
 * @b Recipe: Derive the final implementation from the interface (aliased by the
 * typedef)
 *
 * We're going to use the previously introduced typedef defining our FSM to
 * derive a real implementation of this FSM. All methods of the signal
 * interface ("LightControlSignals") need to be defined here:
 *
 * @include "FSM::LightControl.example"
 *
 * The implementation of the methods forward the method call simply to the
 * current state object:
 *
 * @include "FSM::LightControlMethods.example"
 *
 * @note The constructor must always take a const reference to the type exported
 * as "VariablesType" by the FSM interface ("LightControlInterface", the
 * typedef)
 * @note The constructor must set the FSM to a valid initial state (here Night).
 *
 * @section FSM_States Implementing the behaviour in the different states
 *
 * @b Recipe: Implement a class for each state
 *
 * Each state is represented by a different class. These classes will be
 * instantiated and deleted by the FSM. Here the definition of the states (Day,
 * Night and LampOn) are presented:
 *
 * @include "FSM::Day.example"
 * @include "FSM::Night.example"
 * @include "FSM::LampOn.example"
 *
 * @note All methods from the signal interface must be redefined.
 * @note The state must be derived from an type exported by the interface of the
 * FSM ("Light Control Interface", the typedef) named "StateInterface"
 * @note The constructor must take a pointer to the interface of the FSM
 *
 * The implementation of the methods of these classes looks like this:
 * @include "FSM::DayMethods.example"
 * @include "FSM::NightMethods.example"
 * @include "FSM::LampOnMethods.example"
 *
 * @note: The name of a state is defined by the string used to register the
 * state at the static factory and has to be unique throughout the WNS.
 * This name has to be used also in the constructor of the state.
 *
 */

