##############################################
Experiment 6: Dual Transceiver Mesh Networking
##############################################

Second generation mesh networks support dual transceivers, so that the
last link to and from the STAs can be separated from the remaining
mesh backbone. With the flexible configuration provided by the
WiFiMAC, the setup of dual-transceiver MPs and APs is not any problem:
Instead of adding one transceiver to the configuration, we can add two
(or even more than two, if needed).

It is then a matter of configuration to assign useful frequencies
to different transceivers. In dual-transceiver mode, a single
frequency is required for the mesh network (usually from the 5.5GHz
band) and at most 3 non-overlapping channels are available at the
2.4GHz ISM-band.

In the following, we will change the string-topology scenario from
experiment 5 to a dual-transceiver mesh. To assign a BSS frequency
during the creation, we have to adapt the specialised configuration
class so that the frequency can be given a parameter during the
creation. Furthermore, the STA configuration needs to know the
possible BSS frequencies so that it can scan in the beginning for
beacons and associate to the strongest one.

****************************************
Creation of dual-transceiver MPs and APs
****************************************

Therefore, we first create a new configuration class
``MyBSSTransceiver``:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment6.config.nodeConfig.AP
   :language: python

The configuration class ``MyMeshTransceiver`` remains as before:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment6.config.nodeConfig.Mesh
   :language: python

Whereas the ``MySTATransceiver`` is changed to have
``scanFrequencies`` as parameter:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment6.config.nodeConfig.STA
   :language: python

The problem how to assign the BSS frequencies in an optimal way to the
MPs and APs is discussed in the literature; here, we will use a very
simple approach: We have an array of three non-overlapping
frequencies ``bssFrequencies = [2400, 2440, 2480]``. When creating the
AP and MPs, we use a counter ``bssCount`` and assign the frequencies
in a round-robin fashion. Hence, the creation e.g. of the MPs is done
as follows:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment6.config.NodeCreation.MP
   :language: python

The only further change to the ``config.py`` from experiment 5 is that
the virtual pathselection server now has ``1+2*numHops`` nodes,
because each transceiver counts towards this number.

***********
Experiments
***********

As no new parameters have been created, the same
``campaignConfiguration.py`` from experiment 5 can be used.

#. What is the new saturation throughput of the string, using the same
   simulation parameters as in experiment 5?

#. The spectral efficiency of the system can be defined as saturation
   throughput divided by bandwidth [b/s/Hz]. Compare the spectral
   efficiency of the dual-transceiver mesh, using 4 channels, with the
   single-channel mesh from experiment 1.

