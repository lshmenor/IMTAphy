########################
Experiment 3: The CRC FU
########################

*****************
Unit Dependencies
*****************

Ideally a FUN would consist of FUs without any inter unit
dependencies. But that is not an option for building real world
protocol stacks. Knowing what kinds of unit dependencies exist, what
they imply and when to accept them is essential for the design of FUs
and FUNs. We distinguish between two different kinds of unit
dependencies: *Direct coupling* and *deferred coupling*. Direct
coupling is a dependency on the interface of a FU; deferred coupling
is the dependency on the interface of the command of another FU. When
FU *A* depends on the interface or the interface of the command of FU
*B* we say that *B* is a *friend* of *A*.

Direct coupling
---------------

Direct dependencies can arise for example for

- *horizontal collaboration*; FUs responsible for realizing control
  plane functionality have to configure their friends in the user data
  plane to modify the behavior of the user data plane
  accordingly. Inter node flow control may serve as example: A flow
  control FU realizes the flow control signaling, controlling gate FUs
  in the user data plane.

- *vertical collaboration*; layered protocol functions that must work
  close together but need to take different places in the protocol
  stack.

  E.g., multiplexing FUs that need assistance of their friends below
  them to decide where to deliver compounds.

To avoid tight coupling, those dependencies should rely on the most
general interface possible. The goal is to make FUs depend on families
of units sharing a common interface, rather than to depend on a single
type of FU. This allows friends to be exchanged without modifying the
dependent FU.

Since the exact layout of a FUN is unknown to the FUs, the FUN
provides services for the FUs to find their friends by name and
desired interface. Making the names of the friends a configuration
option to dependent FUs results in a high degree of
flexibility. Friends can be retrieved once after (re-)configuration of
the FUN.

Deferred coupling
-----------------

To retrieve a command from a command pool, the retrieving FU does not
need to rely directly on an interface of the command's
provider. Instead, it relies on the command's provider to be present
in the FUN and on the type and structure of the command the provider
specified.

Placement of FUs in a FUN is crucial for deferred coupling to be
valid. In the outgoing data flow, all friends of a FU in a FUN have to
be placed above. Otherwise, the friend's commands can not be retrieved
while in control, since they are not yet activated.

***
CRC
***

* **Module**

  * ``wns.ldk.CRC``

* **Usage** 

  * Constructor: ``CRC(perProvider)``
  * ``perProvider`` : Name of the friend FU that provides the PER 
  * Drops or marks random compounds depending on their PER

* **Parameter**

  * ``CRCsize`` (default ``16``)

    * Size of the checksum in bits.

  * ``isDropping`` (default ``True``)

    * Select between *dropping* or *marking* behavior

* **Dependencies**

  * A Packet Error Ratio (PER) provider to determine the probability of compound loss.


As an abstract modeling of the calculation of the checksum, the Cyclic
Redundancy Check (CRC) unit performs a random experiment based on the
PER of the compound it inspects. For that, it has to rely on the help
of a friend, the ``perProvider``. The PER provider must provide a
command conforming to the PER provider interface. Using this PER, the
CRC unit can determine the error rate of the compound it is currently inspecting
and thus decide whether the compound is defective or not.

Depending on the configuration, defective compounds get dropped or
simply marked defective.

****
Task
****
The figure below shows the functional unit that also comprises a CRC
functional unit. Even though the lower convergence drops collided
compounds on the physical layer, a minimal bit error rate remains. The
CRC's task is to check whether the remaining packet error rate results
in an erroneous packet.

.. _figure-funtutorial-experiment3-fun:

.. figure:: images/experiment3.*
   :align: center

   FUN setup


1. Implement the missing CRC functional unit that evaluates whether
   the compound is erroneous or not in the ``src`` directory of the
   glue module. Use the CRC template from the functional unit
   tutorial. Use the packet error rate provider to get the packet
   error probability.
2. Include the CRC functional unit in the ``libfiles.py`` of the Glue.
3. Rebuild/Update your campaign.
4. Copy ``config3.py`` file as ``config.py`` into your campaign directory.
5. Run the simulations.



.. literalinclude:: ../../../../../../.createManualsWorkingDir/glue.fun.tutorial.experiment3
   :language: python




.. _figure-funtutorial-experiment3-results-throughput-clients:

.. figure:: images/experiment3_throughput_clients.*
   :align: center
   :width: 480px

   Aggregated throughput of the clients vs. load

