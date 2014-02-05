============
Introduction
============

To follow the steps in this tutorial, it is assumed that the openWNS
and the openWNS Wrowser are installed as descibed in the chapter
:ref:`gettingStarted`.

++++++++++++++++++++++++
Functional Unit Networks
++++++++++++++++++++++++

Many functions and rich features lead to very complex protocol
functions and therefore to highly complicated and intertwined
dependencies inside a protocol layer. The hard part about protocol
design and implementation is the decomposition of an inherent complex
system into manageable units. The task is difficult because many
factors come into play: The granularity of such units, their
interfaces, and inter-dependencies, as well as their run-time
performance, reusability and their ability to allow a further
evolution of the communication system.

This tutorial introduces the Functional Unit Network (FUN) concept for
decomposing layers of protocol stacks using Functional Units FUs that
is used inside the openWNS to model a large number of different
protocol layers. The granularity and interfaces of the FUs are
presented. Each of these units has a cohesive responsibility in terms
of protocol functionality.

Instead of choosing a threaded approach, a FUN runs in a single
thread. Communication between FUs is performed by method calls. While
the method of a FU is executed, the FU is said to be *in
control*. When a FU calls a method of another FU, that FU assumes
control for the duration of the method execution. When the outermost
FU method returns, execution is suspended until resumed again as
consequence of an external event. External events comprise
transmission unit delivery and expiring timers.

In the following we will outline the most fundamental requirements for
FUs and describe interfaces that allow these requirements to be
met. Within this tutorial, we will show applications of the defined
interfaces and how the FUs can be used to compose a system based on
these interfaces. In cases where the interfaces are still too weak, we
will formulate new requirements and present extensions to the
interface that meet the newly identified requirements.

Data Handling
-------------

The most fundamental requirement for FUs is the ability to handle
data. In the following we will denote a basic data unit that is
transmitted between FUs as a *compound*. For now a compound can
be seen as a chunk of data of variable size.

.. _fig-fun-inout:

.. figure:: images/sdr_01.*

   Compound handling methods in different data flows.

FUs as part of a protocol stack may receive compounds for processing
before and after such a compound has been transmitted over the
air-interface. The first case is called *outgoing data flow*, while
the latter case is referred to as *incoming data flow* as depicted in
Figure :ref:`fig-fun-inout`. The interface for handling compounds has
to provide services for accepting data in both directions, incoming
and outgoing. The interface must further enable the FU to distinguish
between compounds of both flows. To support that, it is advisable to
choose two different methods:

.. code-block:: cpp

   void sendData(Compound)
   void onData(Compound)

``sendData`` is used for compounds in the outgoing flow while
``onData`` is used for compounds in the incoming flow as depicted in
Figure :ref:`fig-fun-inout`.

Functional Unit Networks
------------------------

The methods ``sendData`` and ``onData`` are called by other FUs to
propagate compounds through a Functional Unit Network (FUN). A FU
contains two sets of references to other FUs: The *connector set* and
the *deliverer set*. FUs call the ``sendData`` method of other FUs in
their connector set to transfer compounds in the outgoing data flow
and call ``onData`` of FUs in their deliverer set to pass on compounds
in the incoming data flow (see Figure :ref:`fig-fun-connections`).

.. _fig-fun-connections:

.. figure:: images/sdr_03.*

   FU connections for compound handling.

Some of the FUs can be connected to multiple units in both directions
to support multiplexing and de-multiplexing, realized by choosing
different strategies to select a unit for compound delivery in
outgoing and incoming data flows. A FUN is then constructed by
choosing FUs from a toolbox of FUs and connecting them by defining
their connector and deliverer sets.

Given a FUN that is supposed to process compounds from outside the
FUN, some of the units within the FUN serve as source units. Compounds
are injected into the FUN using the ``sendData`` method of such source
units. In other words, every FU whose ``sendData``
method is called from outside is called source unit.

It is possible to further identify a set of units as a sink for
outgoing flows: Compounds delivered to these units are leaving the FUN
for delivery to lower layers. Another set of units can be identified
as a sink for incoming flows: Compounds delivered to these units are
leaving the FUN for delivery to higher layers. Consequently, a FUN,
just like the protocol layer it represents, can be seen as a
bi-directional data processing network. Input to the network is
injected using either the ``onData`` or ``sendData`` method of any of
the FUs. The output of the network is measurable at the sink units.
FU *A* is said to be *above* FU *B* in a FUN if there exists a path
through the FUN in the outgoing direction from a source unit to a sink
unit in which *A* occurs before *B*. *Below* is defined accordingly.

Commands
--------

Whenever a compound arrives at an FU, the FU gains control over the
compound. It then can realize different behaviors by handling the
compound accordingly. It may choose to mutate or drop the data unit,
buffer it, forward it to other FUs, or inject new compounds into the
FUN.

Many protocol functions (e.g., ARQ, CRC) require the FU realizing them
to enrich the compound, adding control information on outgoing
compounds and reinterpreting the added information on incoming
compounds. Those FUs provide a transparent connection to other FUs
above. An ARQ protocol for example adds sequence numbers as control
information to the compounds of the outgoing flow. It creates and
injects compounds as acknowledgments in order to reply to compounds of
the incoming flow. The ARQ instance in the peer FUN reinterprets the
added control information, delivers valid information frames to some
FU in the deliverer set and consumes dedicated compounds containing
acknowledgments. The control information added by FUs is called
*command*. The command can have different characteristics for
different purposes, like an information command or an acknowledgment
command for the ARQ.

The ARQ in our example is completely invisible to the FUs above. Even
underlying FUs do not need to have knowledge about the control
information added by ARQ implementations. The only FU that is required
to be able to handle the ARQ command is the peer unit of the ARQ.

Command Pool
============

Some FUNs add commands being important to other FUs either in the peer
FUN or within the same FUN. Connection identifiers may serve as
example for such information. FUs may require being able to retrieve
the destination address of a compound which is part of a higher level
(another FUs) routing command. This leads to the requirement of
having a possibility to access commands added by other FUs.

Note that FUs cannot simply reinterpret control information added by
other units to the compound's data. FUs have no information about the
layout of the FUN and therefore also have no information about the
layout of the combined control information within the compound. There
might be an arbitrary number of FUs in between the unit that added the
control information and the unit that intends to access
it. Additionally, the compound might have been heavily modified by
other FUs in between.

The solution is to attach a set of commands to each compound. Since a
FUN has a known number of connected FUs, there is a known set of
potential commands. The set containing all the commands of every FU
within a FUN is called *command pool*. Now, the union of a data unit
and a command pool is denoted a compound.

Upon arrival of a new incoming Service Data Unit (SDU) in the FUN, all
commands within the command pool of a compound are inactive. The data
unit of a compound is set depending on the circumstances the compound
is created in:

- The compound is created by the enclosing layer to fulfill a higher
  layer data transmission request: The data of the compound is set to
  the data unit delivered by the higher layer for transmission. Such a
  compound carrying data units of higher layers is referred to as
  *data compound*.

- The compound is created and injected from within the FUN (e.g., ARQ
  acknowledgments): The data part of the compound is initially empty.

Parts of the command pool get activated during the propagation of a
compound through the FUN. Each FU activates its command when it is in
control. At the same time FUs can mutate the data. A set of activated
commands ordered by their time of activation is named a *command
sequence*. A FUN is required to be free of cycles to assure that
commands are not activated more than once.

Figure :ref:`fig-fun-activation` shows two communicating
stations. Each station comprises a FUN with three FUs *A*, *B* and
*C*. Station 1 sends a compound to station 2. The boxes next to the
FUNs show the state of the commands within the command pool of the
compound transmitted. When the compound is delivered to FU *A*, all
commands within the command pool are inactive, which is for example
the case when a compound has just been delivered from a higher
layer. FU *A* activates its command and delivers the compound to FU
*B*. FU *B* itself activates its command and again delivers the
compound to an FU in its connector set.

In step four in this example, the command sequence is *[A, B, C]*. All
commands within the command pool are activated. Note that this is not
a must. Often compounds visit only a small set of FUs of a FUN.

In the incoming flow of station 2, the command pool is not further
mutated. Activated commands stay activated, inactivate commands stay
inactive. This again is the normal case, there may be some exceptions
as will be shown later.

.. _fig-fun-activation:

.. figure:: images/sdr_10.*

   Activation of commands in a command pool.

Coding of Commands
==================

Besides commands being accessible by other FUs, delaying the coding of
commands as part of the data has another advantage: Often information
in communication protocols is not transmitted explicitly as a stream
of bits, but implicitly through the choice of radio resource elements
like time, frequency, space or code. E.g., in a Time Division Multiple
Access (TDMA) system with fixed slot reservations for connections, it
would be useless to explicitly transmit connection
identifiers. Nevertheless the information is indirectly transmitted
through the choice of a specific slot. Such a slot must be chosen at
some point of time based on the connection identity. A command
provided by a connection aware FU may contain the connection
identifier. But the choice how to transmit the connection identity is
delayed, and the outcome may be different depending on the system.

This leads to the idea of having a mechanism solely responsible for
the coding of commands.

Flow Control
------------

In practice every FU has only a limited capacity to store compounds
and often FUs do not need to store compounds at all to accomplish
their task (e.g., forward error correction units). The physical layer
on the other hand introduces a bottleneck, limiting the amount of
information transmitted and thus the rate at which compounds must be
handled.

Without any flow control mechanism within an FUN, compounds could
leave the FUN with much higher rates than the physical layer could
possibly handle. This would result in dropping of compounds in the
physical layer. Buffering between the layers is not an adequate choice
either, since the delay between processing the compound in the FUN and
data transmission would increase. The increase of delay has several
drawbacks. First, timeout mechanisms do not work as
expected. Retransmission timers can lead to retransmission of
compounds although the last transmission of these compounds has not
even been started. Such compounds get added to the buffer several
times, leading again to increasing delays.

Another drawback of increasing delays between compound processing and
compound transmission is that feedback from the \ac{PHY} looses
accuracy; gathered information, e.g. channel state information, is
probably outdated, when the consequences of the decisions based on the
gathered information finally manifests.

Thus, the need for an intra layer flow control arises. FUs must have
the ability to prevent other units from delivering compounds to them,
when they decide not to accept additional compounds.

The Intra Node Flow Control Protocol
====================================

To implement flow control in the outgoing data flow of FUNs, it is
sufficient to supplement the compound handling interface with the
following two methods:

.. code-block:: cpp

   bool isAccepting(compound)
   void wakeup()

Before an FU is allowed to deliver a compound to another FU using
``sendData``, it has to ask for permission using the ``isAccepting``
method. If the response is negative, it may not send a compound to the
questioned unit.

It is essential that FUs ask for permission for a concrete compound,
since the answer may depend on the content of the compound. A FU may
be willing to accept compounds of some type, refusing to accept
others. E.g., a concatenation unit could still be able to use a small
compound for concatenation, not having capacities left for the
concatenation of a larger one.

.. _fig-fun-flow-yes:

.. figure:: images/FlowControl_01.*

   An ``isAccepting`` call with positive response and data delivery.

.. _fig-fun-flow-no:

.. figure:: images/FlowControl_02.*

   An ``isAccepting`` call with negative response and no data
   delivery.

Figures :ref:`fig-fun-flow-yes` and :ref:`fig-fun-flow-no` illustrate
the flow control protocol showing the method calls between two
FUs. The FU "upper" wishes to deliver a compound to the FU "lower" in
the outgoing flow using ``DATAreq``. Figure :ref:`fig-fun-flow-yes`
shows the method calls for a successful compound delivery between two
FUs. Figure :ref:`fig-fun-flow-no` shows the method calls for an
unsuccessful compound delivery attempt.

When an FU cannot deliver further compounds, it cannot proceed and
thus ceases operation until it gains control, again.

The method used for informing other FUs that they might succeed in
sending a compound is ``wakeup``. The set of FUs that have to be
notified when an FU is willing to accept new compounds is called
*receptor set*. The receptor set of a FU *A* contains exactly those
FUs that have FU *A* in their connector set.

.. _fig-fun-flow-wakeup:

.. figure:: images/FlowControl_04.*

   Recursive propagation of state changes using the ``wakeup`` method
   call.

.. _fig-fun-flow-alltogether:

.. figure:: images/FlowControl_03.*

   Interplay of ``wakeup`` and ``isAccepting`` calls.

Figure :ref:`fig-fun-flow-wakeup` shows an example of a possible
``wakeup`` method call sequence. The ``wakeup`` method of FU "lower"
has been called and it has no compounds ready for delivery. Thus, it
delegates the ``wakeup`` call to some (possibly all) FUs in its
connector set: FU *upper1* and FU *upper2*. In this example, both
upper FUs have no compounds to deliver and do not further delegate the
``wakeup`` call to upper FUs. In :ref:`fig-fun-flow-alltogether`, a
call sequence is depicted where the FU woken up delivers compounds
until the lower FU stops allowing compound delivery using intra layer
flow control.

Besides the rules above, there are some rules which must be followed
by every FU to conform to the flow control protocol: Two consecutive
calls to ``isAccepting`` with the same compound and no ``sendData``
calls in between have to yield the same result.

The following rules provide a way how to accomplish this stability.

1. An FU may only base its decision whether to accept a compound or
   not on its internal state, on the content of the compound and on
   the outcome of ``isAccepting`` calls to FUs in its connector set.

2. An FU may not mutate the compound during a call to its
   ``isAccepting`` method.

3. An FU may not change state during a call to its ``isAccepting``
   method.

4. An FU may not mutate the compound between the ``isAccepting`` call
   to an FU of its connector set and the delivery of that unit to the
   questioned unit.

   Since an FU may base its decision whether to accept a compound on
   the content of the compound, it is illegal for the questioner to
   mutate the compound, potentially invalidating the promise of the
   questioned unit to accept the compound.

5. If an FU delegates the ``isAccepting`` call to an FU in its
   connector set, it has to deliver the compound to exactly this FU.
   This leads to arbitrary long chains of promises to accept a
   compound.

Note that rule 4 has a very strong impact on the implementation of FUs
that have no internal capacity since they may not mutate the compound.
A weaker version of rule 4 would allow the modification of the
compound given the knowledge that no FU in the chain of promises bases
its decision on the changes made to the compound. But this condition
is very difficult to guarantee. The FU ``Synchronizer`` helps dealing
with this problem.

It is important to note that the order in which an FU awakens units in
its receptor set significantly changes the behavior of propagation of
compounds. Units being called first, have a higher chance of being
able to deliver compounds. A fair strategy wakes units up using a
round robin algorithm, starting with another unit every time. For
three units *A*, *B*, *C* in the receptor set, a fair wakeup sequences
is: *ABC*, *BCA*, *CAB*, *ABC*, etc. If the units in the receptor set
have clear priorities, a single wakeup sequence with the units ordered
by descending priorities suffices. The wakeup strategy is part of the
receptor aspect of each FU.

Inter Node Flow Control
=======================

As stated above, in contrast to the Physical Layer (PHY), higher
layers usually are not a bottleneck, but in case of a bottleneck in
higher layers (e.g., streaming applications that accept data with a
lower bit rate than the physical layer provides), protocols have to
provide inter node flow control mechanisms between the communicating
nodes.

In fact, inter node flow control again is based on the flow control in
outgoing data flows, but this time between FUs of the peer
node. Protocol functions must be provided to informing the peer node
producing data to slow down, which results in intra node flow control
of the producing node to limit the amount of generated data. Thus,
inter node flow control is peer controlled intra node flow control.

Five Aspects of a Functional Unit
---------------------------------

To summarize the discussion above, we distinguish five aspects of an
FU:

1. *Compound Handler*

   Implement the handling of compounds of an FU including intra FUN
   flow control. The methods provided are

   1. ``void onData(compound)``

   2. ``void sendData(compound)``

   3. ``void wakeup()``

   4. ``bool isAccepting(compound)``

   Handling of compounds includes mutation, dropping, injection and
   forwarding. Activation and initialization of commands is considered
   as mutation.

2. *Command Type Specifier*

   Specify the type of command provided by the FU. The command type is
   used to create and maintain command pools.

3. *Connector*

   Hold the set of outgoing target FUs: Compounds in the outgoing data
   flow get delivered to FUs from this set. Define a strategy to
   select the appropriate FU for a given compound.

4. *Receptor*

   Hold the set of FUs in which the FU itself is in the connector
   set. Define a strategy to wake up FUs.

5. *Deliverer*

   Hold the set of incoming target FUs: Compounds in the incoming data
   flow get delivered to FUs from this set. Define a strategy to
   select the appropriate FU for a given compound.

Configurability
---------------

The high degree of configurability of protocol stacks using FUNs is
achieved by allowing configuration at several levels. The levels of
configurability in order of increasing abstraction are:

- *Parametrization level:*

   The lowest level of configuration includes the parametrization of
   concrete FUs: What is the window size of the *SelectiveRepeat* ARQ
   unit? What is the Maximum Transfer Unit (MTU) of the Segmentation
   And Reassembly (SAR) unit?

- *Concretion level:*

   The next higher level focuses on the selection of concrete FUs to
   fill the respective places in a FUN. Concrete implementations have
   to be chosen for intended protocol functions.

- *Layout level:*

   The highest level of configuration comprises the placement of
   protocol functions in a stack: the scaffolding of a protocol stack,
   including the interconnections of FUs and their intended
   functions. The order in which certain processing is applied to
   compounds as well as the overall set of supported messages is
   determined at this level of abstraction.
