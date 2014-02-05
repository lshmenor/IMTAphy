##########################################
Experiment 3: Flexible Scenario Generation
##########################################

In the third experiment, we will benefit from the power of Python as a
configuration language. The main goal of this experiment is to setup
the scenario shown in the following figure: One AP is situated in the
center, all STAs surround the AP on a circle with a variable radius
r. The number of STAs n can be changed, but the positioning of the
STAs on the circle shall always be equidistant.

.. figure:: images/experiment3.*
   :align: center

   Scenario of experiment 3

To support a flexible scenario setup, we will use two important features of Python:

#. To generate several STAs that only differ in the position, we will
   have one STA-config class, derived from the standard transceiver
   configuration, that requires only the position information.

#. Using this config class, we will generate n STAs using a for-loop
   and the cos/sin functions from Python to calculate the x/y
   coordinates of the STAs.

***********
Preparation
***********

Using ``./playground.py preparecampaign PATH`` with the same ``PATH``
as in the preparation for experiment 1 (namely ``../myFirstCampaign`` if you did follow the chapter "Preparations" before Experiment 1), we can create a sub-campaign
that uses the same sandbox, but a differed directory to store the
simulations. When prompted by ``./playground.py``, please select
``(C)reate a sub-campaign`` and choose as name e.g. ``Experiment3``.

As the scenario has similarities to the one created in the first two
experiments, we can re-use the ``config.py`` and the
``campaignConfiguration.py`` by copying them to the new directory
``Experiment3``. The following description will use those two files.

***************************
Specialising the STA-config
***************************

The ``config.py`` file from the first experiments required several
lines of code to generate and configure the WiFiMAC-part of the STA:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment1.config.NodeCreation.STA.node
   :language: python

As all n STAs will have the same configuration, we now would like to
replace those lines with the following code:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment3.config.NodeCreation.STAs.loop.node
   :language: python

Therefore, a class ``MySTATransceiver`` is needed, which is derived
from the basic STA-Transceiver class
``wifimac.support.Transceiver.Station``. This is done in the following way (before the creation of the STAs):

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment3.config.nodeConfiguration.STA
   :language: python

This class inherits the default configuration; this default values can
the be changed. Additionally, the creator function
``__init__`` can be changed to the meet requirements of the
scenario. In detail, the new specialised class works in the following way:

* A new ``__init__`` function requires only the STA position as parameter.

* Upon being called (which happens every time an instance of this
  class is created), the ``__init__`` function first calls the
  ``__init__`` function of its superclass, which requires known
  parameters (start frequency, position, frequencies for scanning,
  scanDuration).

* Then, the parameters are changed similar as in experiment 1:

  #. The transmission power is set to 20 dBm

  #. This time, the rate adaptation strategy is set to
     ``ConstantLow``, i.e. BPSK 1/2 for every transmitted frame.

  #. Finally, the threshold to precede every frame transaction with an
     RTS/CTS is set to a high value, i.e. RTS/CTS is switched off.

With this new class, STAs can be created using the code above. Of
course, the traffic generation has to be set for every station,
similar to experiment 1.

************************
Automatic STA generation
************************

Like any other programming language (but unlike static configuration
files), Python provides a ``for`` - loop construct which we will use
to generate as many STAs as required. Based on the STA number, we will
calculate the x/y position so that the STAs are placed on the circle
with equidistant separation. Thus, the loop looks like

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment3.config.NodeCreation.STAs.loop
   :language: python


Of course, the remaining creation of the STA (including the traffic
and the addition to ``WNS.nodes`` must be inside this loop.

.. note::

  The resulting ``config.py`` with the described changes is also
  available in the directory
  ``myOpenWNS/tests/system/wifimac-tests/PyConfig/experiment3``

********************
Experiments - Part 1
********************

#. Adapt the ``campaignConfiguration.py``, copied from the first
   experiment (**without** the binary-search based calculation of the
   saturation point) so that it contains the two new parameters
   ``radius`` and ``numStations``, the old parameter ``distance`` is
   no longer needed.

#. Simulate the scenario with the following parameters:

   * 2 STAs

   * a radius of 25m

   * 100% uplink traffic

   * packet size of 1480 Bytes

   * an offered traffic between 1 and 4 Mb/s.

  Where is approximately the saturation point?

#. Increase the number of STAs to 20. Where is the expected saturation
   point? Validate the expectation with simulation results.

*****************************************
Efficient Search for the Saturation Point
*****************************************

As in experiment 2, we can again use the binary search to find the
saturation point without manual trial-and-error simulations. To do
this, we have to adapt the function ``getTotalThroughput``, as now the
offered traffic is given per STA, but the measured throughput (e.g. by
the probe ``ip.endToEnd.window.incoming.bitThroughput_Moments``) in
the RANG is the combined throughput of all STAs. Hence, for a
retrieval of f(input) = output from the database, we have to

* Query, next to the input and the output value, the number of STAs in
  the simulation and

* Divide the total traffic by this number.

This is done by the following ``getTotalThroughputPerSTA`` function,
which can be found in the ``campaignConfiguration.py`` prepared in the
directory
``myOpenWNS/tests/system/wifimac-tests/PyConfig/experiment3``:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.tutorial.experiment3.campaignConfiguration.GetTotalThroughputPerSTA
   :language: python

As we can see, the two queries (for incoming and aggregated traffic)
result a 4-column list, where the 3rd column contains the number of
stations.

Besides this change (and the new simulation parameters), the file is
the same as in experiment 2.

********************
Experiments - Part 2
********************

#. Find the saturation point of the circle scenario for
   5, 10 and 50 STAs

#. What happens to the saturation point if the circle radius is
   enlarged from 25m to a value close to the maximum value, as
   calculated in experiment 2?
