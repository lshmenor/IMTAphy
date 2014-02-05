#########################################
Experiment 5: Window And Packet Probe FUs
#########################################

In this section we learn how to use the tool PyTree to browse the simualtion configuration and to visualize the Functional unit network (FUN)

***************
Starting PyTree
***************

The PyTree tool is installed under ``bin/pytree`` of your openWNS root directory. It can only be executed from the openWNS root directory or a subdirectory since it needs access to the installed openWNS Python files. To start inspecting the configuration file of Experiment 5 first change into the directory where the file is located:

.. code-block:: bash

   $ cd myOpenWNS/tests/system/glue/PyConfig/funTutorial

and then run PyTree:

.. code-block:: bash

   $ myOpenWNS/bin/pytree config5.py

The figure below shows the upper left part of the opened up PyTree window. Click on the triangle next to ``config5.py`` to expand the configuration tree. 

.. figure:: images/experiment5_pytree.*

    PyTree

PyTree shows all Python objects and their attributes included in the configuration. The most important ones are available through a shortcut under ``FAVORITES`` just underneath the root entry. Here three entries are present:

measurementSources
    All measurement sources (Probes) are listed here. Click on a probe to find out how measured     values are sorted 
    and which statistics are collected.

nodes
    All nodes in the simulation scenario are listed here. This configuration has three virtual nodes    supporting IP 
    (DHCP, DNS, ARP) and two communicating stations (``mainconfig.Station``).

simulator
    This is the root configuration object passed to the simulator. It includes global configurations    like simulation 
    time ``maxSimTime`` and the output directory ``outputDir``.

As a first task, try browsing through the configuration: Find out what the simulation time is. What is the output directory? What is the initial seed of the random number generator? How can you reach the list of nodes through the ``simulator`` entry in ``FAVORITES``?

We now want to take a closer look at the nodes. Expand the first ``mainconfig.Station`` under ``components`` and also below you can see the four layers of the node:

load (constanze.node.ConstanzeComponent)
    This is the traffic source and sink.

ip (ip.Component.IPv4Component)
    IP is not really required in this tutorial (no routing needed) but it is used since it simplifies node  addressing.

dll (Tutorial.Experiment5)
    This is our individual data link layer ``Glue`` constructed from a custom FUN. 

phy (copper.Copper.Transceiver)     
    The physical layer is a half-duplex wired channel. It can detect collisions and report them to the  DLL. It can also 
    use a random number distribution to draw the bit error rate for each   transmission.

Now further expand the ``dll`` entry and click on ``fun``. A window like the one below will pop up.

.. figure:: images/experiment5_pytree_fun.*

    PyTree FUN



.. _figure-funtutorial-experiment5-fun:

.. figure:: images/experiment5.*
   :align: center

   FUN setup

.. literalinclude:: ../../../../../../.createManualsWorkingDir/glue.fun.tutorial.experiment5
   :language: python

.. _figure-funtutorial-experiment5-results-throughput-clients:

.. figure:: images/experiment5_throughput_glue_top_glue_bottom_clients.*
   :align: center
   :width: 480px

   Aggregated throughput of the clients vs. load at top and bottom of the DLL
