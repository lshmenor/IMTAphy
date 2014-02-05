#############################
Experiment 5: Mesh Networking
#############################

The fifth experiment adds a new class of nodes to the known AP and STA
node: Mesh Points (MPs). In contrast to APs and STAs, MPs can neither
be source nor sink of a traffic flow. In fact, MPs do not have an IP
layer, but use a layer 2 path selection protocol to find the optimal
path in the network between an AP and a STA or between two
STAs. Therefore, the mesh network is completely transparent for the IP
layer and all layers above.

In the next steps, the scenario in the following figure shall be
simulated: A STA wants to communicate with an AP that is out of its
communication range. Hence, it uses the intermediate MPs which provide
service for association and transparent data forwarding between the AP
and the STA. The distance between the nodes in the line can be varied;
furthermore, the number of hops (and thus the number of MPs) can be
changed.

.. figure:: images/experiment5.*
   :align: center

   Scenario of experiment 5

We recommend to create a new sub-campaign for experiment 5 and to copy
the ``config.py`` and the ``campaignConfiguration.py`` from experiment
4 into the new simulation directory.

********************
Creating mesh points
********************

The transceiver configuration of a MP is the same as the
configuration for an AP; hence, the same specialised configuration
class from experiment 4 can be used; we choose to rename it to ``MyMeshTransceiver``:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment5.nodeConfig.AP
   :language: python

Then, the MPs can be created using a ``for``-loop; the only difference
to the creation of an AP is the usage of the ``createMP`` function of
the node creator:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment5.config.NodeCreation.MP
   :language: python

Please note that it is ensured that beacons are transmitted by the MPs
at different points in time so that collisions of beacons with other
beacons are impossible.

********************************
Remaining scenario configuration
********************************

The rest of the scenario configuration resembles the setting of the
experiment 1: Only one AP and one STA has to be created; this time,
the specialised configuration classes should be used for convenience.

Other changes to the configuration file are:

* One new parameter is required: ``numHops``

* The sizeX of the scenario has to be enlarged, depending on the
  distance and the number of hops.

* The number of nodes, given as parameter to the virtual pathselection
  server, has to be adapted to the number of MPs.

* The evaluation of the wifimac requires as a parameter the number of
  hops.

***********
Experiments
***********

For the experiments, the ``campaignConfiguration`` of experiment 2 can
be used; only the new parameter ``numHops`` has to be added. In the
following, the nodes shall be separated by a distance so that the
neighbours can communicate with each other, but the neighbor's neighbors
cannot. Hence, the 2-hop neighborhood shall be hidden nodes.

#. Determine the saturation throughput of the string with at least 2
   hops if only unidirectional traffic (e.g. only downlink) is used.

#. Mesh networks often have the problem that the "boundary" nodes have
   much less neighbors as the central nodes; Hence, they see the
   channel more often as idle and thus "overflow" the central
   nodes. To simulate this effect, determine the saturation throughput
   of the string with bidirectional traffic, e.g. 50% uplink and 50%
   downlink traffic. What percentage of the unidirectional traffic can
   be reached? How can the difference be explained?

.. note::

   An example ``config.py`` and ``campaignConfiguration.py`` can be found in
   ``myOpenWNS/tests/system/wifimac-tests/PyConfig/experiment5``.
