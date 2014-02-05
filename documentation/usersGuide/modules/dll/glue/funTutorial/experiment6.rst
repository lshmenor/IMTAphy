######################################
Experiment 6: The Stop-And-Wait ARQ FU
######################################

******************
Compound Injection
******************

Three forms of compound injection have been identified: Injection of
received compounds from the incoming data flow into the outgoing data
flow, replies to received compounds and spontaneous injections.

Relaying
--------

Relaying concepts are a promising approach to cope with the high
expectations in next generation wireless systems. They enable flexible
deployment concepts with high Radio Access Point (RAP) densities,
providing high data rates at reduced costs.

A relaying FU of a relay station receives compounds in the incoming
data flow and injects these compounds back as part of the outgoing
data flow. As presented, compounds contain a fixed, non-extendable
command pool. The activation of commands within the command pool
introduces a problem: Having a single point of activation implies that
compounds may not cross the borders between the two networks from
incoming to outgoing data flows. No FU may forward a compound using
``sendData``, when received via ``onData``. This is a direct
consequence from the activation of commands. Otherwise, it would be
possible for the compound to be delivered to a FU that already
activated its command.

.. _fig-fun-relay:

.. figure:: images/sdr_09.*

   Activation of commands in a command pool when performing relaying.

To implement relaying, the relaying FU has to inject a copy of the
received compound. It has only those commands activated and copied,
which occur in the command sequence before the relaying unit. The
remaining commands will stay inactive. See Figure :ref:`fig-fun-relay`
for an overview over the activation status of commands within a
compound before and after being processed by a relaying FU.

Replies
-------

A common requirement of FUs is to have a reversed transmission channel
to support bi-directional communication. E.g., an ARQ FU needs to be
able to send acknowledgment compounds to the sender of data
compounds. It therefore creates a new compound and activates its
command, filling it with the acknowledgment control information. Since
the ARQ FU has no knowledge about addressing and routing of compounds,
FUNs must provide a mechanism to get the according commands filled by
the respective FUs.

Replies are created with the assistance of FUs in the command
sequence. A FU queries its predecessor in the command sequence to
create a reply to a given compound using the create reply interface.
The create reply interface extends the command provider interface:

.. code-block:: cpp

   CommandPool createReply(CommandPool)

The ``createReply`` method may get called recursively until the
source unit in the command sequence is reached, or until some FU in
the path has the knowledge to decide that the information added to the
compound suffices for successful compound delivery.

Spontaneous Injections
----------------------

In contrast to creating replies, FUs performing spontaneous injections
have no original compound to inspect and possibly no predecessors in
the command sequence to delegate command pool initialization to.

The proposed solution to implement spontaneous injections of compounds
that need a pre-activated command pool is to configure the FU to use a
compound creation strategy. One possible creation strategy could use
the prototype pattern.


.. _figure-funtutorial-experiment6-fun:

.. figure:: images/experiment6.*
   :align: center

   FUN setup

.. literalinclude:: ../../../../../../.createManualsWorkingDir/glue.fun.tutorial.experiment6
   :language: python

.. _figure-funtutorial-experiment6-results-throughput-clients:

.. figure:: images/experiment6_throughput_clients.*
   :align: center
   :width: 480px

   Aggregated throughput of the clients vs. load

