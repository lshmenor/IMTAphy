##########################
Experiment 1: A Simple FUN
##########################

Functional units in the WNS are the components of a functional unit
network (FUN). Several functional units arranged and connected
together become the FUN. Usually a functional unit needs to send
functional unit specific information like sequence numbers to its
counterpart it is communicating with. This is performed by embedding
information in the command of the functional unit. Every FUN holds a
command proxy, which is the common access point of a command pool,
which is literally the PCI of the Layer. Every functional unit that is
derived from the CommandTypeSpecifier may ask the command proxy to
activate its command entry in the command pool and access the
functional unit specific data in the command.

The figure shows a simple functional unit network that consists of the
upper and lower convergence. The trivial case connects the upper
converegence with the lower convergence. All compounds are directly
directed to the upper or the lower convergence respectively.

.. _figure-funtutorial-experiment1-fun:

.. figure:: images/experiment1.*
   :align: center

The python configuration of the functional unit is shown below. The
``Experiment1`` class derives from the ``Component2Copper`` class
which is DLL component that connects to a copper component. The
``Experiment1`` class directly connects the upper converegence with
the lower convergence.

.. literalinclude:: ../../../../../../.createManualsWorkingDir/glue.fun.tutorial.experiment1
   :language: python

-----
Tasks
-----

 1. Create a new campaign.
 2. Copy ``campaignConfiguration.py``, ``Tutorial.py``, ``Evaluation.py`` and
    ``config1.py`` as ``config.py`` to the campaign's simulation directory.
 3. Create the database.
 4. Create the scenarios.
 5. Execute the simulations.
 6. Have a look at the results with the wrowser, as soon as the simulations have been finished.

-------
Results
-------

The results show that the goodput of the clients depends on the
offered traffic and the number of clients in the scenario. With a
single client, the saturation throughput is at 40% of the offered
traffic. With 25 clients in the scenario, the saturation throughput is
at 20% of the offered traffic.

.. _figure-funtutorial-experiment1-results-throughput-clients:

.. figure:: images/experiment1_throughput_clients.*
   :align: center
   :width: 480px

   Aggregated throughput of the clients vs. load

.. _figure-funtutorial-experiment1-results-throughput-router:

.. figure:: images/experiment1_throughput_router.*
   :align: center
   :width: 480px

   Aggregated throughput of the router vs. load
