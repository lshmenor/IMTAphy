########################################
Experiment 4: Shadowing and Interference
########################################

The fourth experiment increases the realism of the scenario by
introducing shadowing, i.e. attenuation of the radio signal by
objects. The scenario to be evaluated is shown in the following figure.

.. figure:: images/experiment4.*
   :align: center

   Scenario of experiment 4

The wall in the middle of the scenario shall have an attenuation of
100dB, so that it completely shields from radio waves. Depending on
the length of the wall, different interference situations occur, under
the assumption that traffic is 100% downlink from the APs to the STAs:

* If the wall length is zero, all transmissions interfere with each
  other, but all nodes also can detect the other's
  transmissions. Hence, frame errors due to low Signal to Noise plus
  Interference Ratio (SINR) occur only if the same backoff value is
  selected by the APs, which has a relatively low probability.

* If the wall length is greater than 5m, but less than 15m, the APs
  cannot detect each other's transmissions, but both will create
  interference at the STAs. Therefore, the transmissions chance for
  low SINR and thus high packet error rates is high. This is a
  typical case of the hidden node scenario.

* If the wall is longer than 15m, but less than 25m, it will block the
  interference from the APs. Nevertheless, the transmission of an ACK
  by a STAs will interfere with a potentially simultaneous
  transmission to the other STA, and thus lower the SINR and create
  frame errors.

* Finally, if the wall is longer than 25m, the two links are totally
  separated from each other.

For the simulation of the those cases, please generate a new
sub-campaign as done in experiment 3.

***************
Creating a Wall
***************

Walls are created in two steps:

#. A list of objects is created and enriched with the shadowing objects with a given size and attenuation:

   .. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment4.config.scenario.createWallObj
      :language: python

#. Instead of using ``rise.scenario.Shadowing.No()``, a different class of shadowing is used:

   .. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment4.config.radioChannel
      :language: python

That's it.

***********
Experiments
***********

For the experiments, the ``config.py`` of experiment 3 can be used and
changed according to the following steps:

#. In the scenario, the two APs shall have different beacon start
   times to avoid unrealistic results from beacon collisions at the
   STAs. Beside the position, the remaining configuration shall be
   identical to the configuration in experiment 1. Therefore, similar
   to the third experiment, a specialised class ``MyAPTransceiver``
   shall be used to create the AP configuration. The creation of the
   AP is therefore reduced to

   .. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment4.config.NodeCreation.AP
      :language: python

   Write the necessary class ``MyAPTransceiver`` by copying the class ``MySTATranceiver`` and editing it respectively.

#. The creation of one AP and one STA can be done in a for loop, where
   only the y-coordinate changes:

   .. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment4.config.NodeCreation.ForLoop
      :language: python

#. The virtual path selection server requires as input the number of
   nodes; here, it is four.

#. Next to ``simTime``, ``offeredTraffic``, ``packetSize`` and ``ulRatio``, the parameter ``wallLength`` shall be used in the campaign.

To evaluate the saturation point with the binary search, a similar
``campaignConfiguration.py`` as in experiment 2 can be used, only with
the difference that the cumulative traffic has to be divided by 2 to
account for the two STAs.

.. note::

   An example ``campaignConfiguration.py`` can be found in
   ``myOpenWNS/tests/system/wifimac-tests/PyConfig/experiment4``. Additionally,
   an example ``config.py`` is also located there.

#. Evaluate the saturation throughput for a wall length of 0.0, 10.0
   and 20.0 meters.

#. Add a new parameter of type ``String`` to evaluate the difference
   of the two rate adaptation strategies ``Opportunistic`` and
   ``ConstantLow``.
