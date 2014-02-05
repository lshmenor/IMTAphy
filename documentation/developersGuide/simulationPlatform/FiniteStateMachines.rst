.. highlight:: c++

.. index:: finite state machine, FSM, state machine

How to write a Finite State Machine
===================================

A simple example should clarify how to implement a finite state machine based
on the wns::fsm::FSM template. A finite state machine for a light control,
which switches the light at night on if a movement was detected and switches
it off again after a certain timer has elapsed should be designed. The
following sections will show the necessary steps to implement the ligt
control based on wns::fsm::FSM. The following sub-sections represent the
necessary steps:


Modelling the FSM Light Control
-------------------------------

**Recipe**: Find a model which describes the problem based on a finite state
machine

First of all we need to find a model for the finite state machine. We know
that the Lamp Control should turn on the lamp if a movement was detected and
if it's currently night. Thinking about this we can derive that we need at
least three states for the FSM to represent the different behaviours: Day,
Night, LampOn, since the FSM must react different to input if it's day or
night and different again, if the light is currently turned on (e.g., at
day the light must not be turned on if a movement was detected).

At run time the finite state machine will be in one of these three
states. Being in these states it needs to react to certain signals (input)
with some action. The action may also include a state change (e.g., the
finite state machine is in state "Night" and the signal "light" arrives, the
state would change to "Day"). The signals needed here are: dark (to switch from
Day to Night), light (to switch from Nigth to Day), movementDetcted (to switch the
Lamp on) and timerElapsed (to switch the lamp off after some time). With
these states and signals, the model of our finite state machine "Lamp Control"
looks like this:

.. graphviz::
   digraph finite_state_machine {
   size="8,10"
   node [shape = circle, fontname=Helvetica, fontsize=12 ]; Day Night LampOn;
   rankdir=LR;
   Day -> Night [ label = "dark", fontname=Helvetica, fontsize=9 ]
   Day -> Day [ label = "movementDetcted", fontname=Helvetica, fontsize=9 ]
   Day -> Day [ label = "light", fontname=Helvetica, fontsize=9 ]
   Night -> Night [ label = "dark", fontname=Helvetica, fontsize=9 ]
   Night -> Day [ label = "light", fontname=Helvetica, fontsize=9 ]
   Night -> LampOn [ label = "movementDetcted", fontname=Helvetica, fontsize=9 ]
   LampOn -> Night [ label = "timerElapsed", fontname=Helvetica, fontsize=9 ]
   LampOn -> Day [ label = "light", fontname=Helvetica, fontsize=9 ]
   LampOn -> LampOn [ label = "movementDetcted", fontname=Helvetica, fontsize=9 ]
   LampOn -> LampOn [ label = "dark", fontname=Helvetica, fontsize=9 ]
   }

Summary of the model:

* It has three states:
  #. Day
  #. Night
  #. LampOn
* It has four signals:
  #. light
  #. dark
  #. movementDetected
  #. timerElapsed

After this model has been identified it needs to be transfered to C++. The
framework provided by ``wns::fsm::FSM`` will help us to get the model implemented
fast and efficiently. Most of the following steps contain a sentence
describing in recipe style what needs to be done.


An interface (abstract class) to define the signals
---------------------------------------------------

**Recipe**: Write a class having all signals of the finite state machine as pure
virtual functions.

The signals have to be modeled as pure virtual methods of a class. Make sure
the class is really an interface (does not contain any variables, is
stateless). The States classes will be derived from this
interface later. The interface for our signals looks like this:

.. literalinclude:: ../../../.createManualsWorkingDir/FSM::LightControlSignals.example

.. note::

   The copy constructor has been disallowed in order to prevent copying of
   states. The default constructor is protected: since an interface can't be
   instantiated the constructor can only be called by a derived class.

States share Variables
----------------------

**Recipe**: Write a class containing all the variables of the FSM shared by
the different states

The finite state machine is also characterized by a set of variables which it
holds. All these variables have to be implemented in a struct or class which
will be used by the wns::fsm::FSM template later. For simplicity our example
assumes the class definig the variables is called "Lamp" and contains only
one variable called "on":

.. literalinclude:: ../../../.createManualsWorkingDir/FSM::Lamp.example

An interface for the FSM "Light Control"
----------------------------------------

**Recipe**: Make a handy typedef to the interface of your FSM

The template wns::fsm::FSM instantiated with Variables and Signal will not
implement the finite state machine itself. It will provide an interface for
the final finite state machine. To have a handy alias create a typedef to
this interface with the signal interface from above ("LightControlSignals")
and the variables that characterize the finite state machine ("Lamp"):

.. literalinclude:: ../../../.createManualsWorkingDir/FSM::LightControlInterface.example

Implementation of the finite state machine
------------------------------------------

**Recipe**: Derive the final implementation from the interface (aliased by the
typedef)

We're going to use the previously introduced typedef defining our FSM to
derive a real implementation of this FSM. All methods of the signal
interface ("LightControlSignals") need to be defined here:

.. literalinclude:: ../../../.createManualsWorkingDir/FSM::LightControl.example

The implementation of the methods forward the method call simply to the
current state object:

.. literalinclude:: ../../../.createManualsWorkingDir/FSM::LightControlMethods.example

.. note::

   The constructor must always take a const reference to the type exported
   as "VariablesType" by the FSM interface ("LightControlInterface", the
   typedef)

.. note::

   The constructor must set the FSM to a valid initial state (here Night).

Implementing the behaviour in the different states
--------------------------------------------------

**Recipe**: Implement a class for each state

Each state is represented by a different class. These classes will be
instantiated and deleted by the FSM. Here the definition of the states (Day,
Night and LampOn) are presented:

.. literalinclude:: ../../../.createManualsWorkingDir/FSM::Day.example
.. literalinclude:: ../../../.createManualsWorkingDir/FSM::Night.example
.. literalinclude:: ../../../.createManualsWorkingDir/FSM::LampOn.example

.. note::

   All methods from the signal interface must be redefined.

.. note::

   The state must be derived from an type exported by the interface of the
   FSM ("Light Control Interface", the typedef) named "StateInterface"

.. note::

   The constructor must take a pointer to the interface of the FSM

The implementation of the methods of these classes looks like this:

.. literalinclude:: ../../../.createManualsWorkingDir/FSM::DayMethods.example
.. literalinclude:: ../../../.createManualsWorkingDir/FSM::NightMethods.example
.. literalinclude:: ../../../.createManualsWorkingDir/FSM::LampOnMethods.example

.. note::

   The name of a state is defined by the string used to register the
   state at the static factory and has to be unique throughout the WNS.
   This name has to be used also in the constructor of the state.


