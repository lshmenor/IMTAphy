###############################
IMT-A Channel Model Calibration
###############################

In this chapter we will introduce how to use the Wrowser scenario viewer and how to setup an IMT-Advanced (IMT-A) scenario.

=====================================
How to use Wrowser to view a scenario
=====================================


.. note::

   Before starting this section, please refer to :ref:`gettingStartedWrowser` to install Wrowser and its plugins for playground.py.

Then use following command to start Wrowser.

.. code-block:: bash

   $ openwns-wrowser: wrowser

After Wrowser started, ``sandbox path`` needs to be set up for Wrowser first.
Go to the ``Extra`` menu and open ``Preferences``. Find the ``Sandbox`` tab and fill in your sandbox path ``/home/[USER]/myOpenWNS/sandbox``. ``[USER]`` must be replaced by your user name. Using ``~`` does not work in this path! Then click ``OK`` to save and close this dialog window. 

Then go to the ''File'' menu and click ''View Scenario''. In the open window find the config file. Here we use ``~/myOpenWNS/tests/system/ofdma-tests/configITUInH.py`` as an example. This is the IMT-A Indoor Hotspot (InH) evaluation scenario with two base stations.

As shown in the Figure :ref:`figure-scenarios-Wrowser-view-scenario1`:

.. _figure-scenarios-Wrowser-view-scenario1:

.. figure:: images/view-scenario1_InH.*
   :align: center

   Scenario viewer of an Indoor Hotspot scenario, picture 1

The main window shows the positions of all base stations and user terminals. There are two base stations (BS) in this scenario. The ``ofdma-tests`` scenarios do not have real user terminals (UTs). ``UE3`` in this figure is only a scan node used to record the SINR distribution. It is positioned in the lower left corner of the scenario at relative position -60m, -25m from the center. The center is located at 1000m, 1000m.

On the left part of this window click ``scan``.

After scan has finished, click ``Map Plotting`` tap at left bottom. Here you get a list of available plots after the scan. ``RxPwr`` is the received power, ``SINR`` the signal to noise and interference ratio. If you select a probe with ``BSID`` infix only, results measured from one BS are shown. Suffix ``max`` means the maximum recorded value at a position, ``mean`` the average value. The suffix ``trials`` is for debug purposes assuring enough measurements were collected at each position. 

First have a look at the mean and max received power of both and each BS. Try out the ``Draw contour plot`` option. A UT will usually associate to the BS it experiences best channel conditions to. Which available plot is best to visualise the relation between position and channel quality? Where is the boundary between the two cells?

Figure :ref:`figure-scenarios-Wrowser-view-scenario2` shows an example:

.. _figure-scenarios-Wrowser-view-scenario2:

.. figure:: images/view-scenario2_s.*
   :align: center

   Example: Max Receiver Power for In door Hotspot scenario

Now open the ``Map Cut Plotting`` tab. Insert the start and end point coordinates and click ``Plot``. The x-axis shows the distance from ``Point 1``. On the y-axis the value of the area plot along the way between the points is shown. Now go back and redraw the area plot. Then return to the ``Map Cut Plotting`` tab. Can you find a more convenient way to select the points than typing in the coordinates?

==================================================
ITU Urban Macro Scenario Channel Model Calibration
==================================================

To assure an error free implementation of the IMT-A channel model, multiple evaluation groups have published their SINR and pathloss results. They can then be compared against each other. The results from the WINNER+ evaluation group are available `here
<http://projects.celtic-initiative.org/winner+/WINNER+%20and%20ITU-R%20EG%20documents/Calibration%20for%20IMT-Advanced%20Evaluations.pdf>`_.

The results for the Urban Macro scenario can be found in figure 3 of the document. The ``ofdma-tests`` in openWNS have been created to assure the channel model is calibrated and remains that way. Enter the test directory ``~/myOpenWNS/tests/system/ofdma-tests/`` and run the Urban Macro test by typing ``./fast-openwns -f configITUUMa.py``. While waiting for the simulation to finish we take a look at the simulation setup:

.. literalinclude:: ../../../../.createManualsWorkingDir/ofdmascanner.uma

Above lines provide all that is needed to set up an Urban Macro scenario with one ring of interferers and a total of 21 sectors. BS transmission power is 49 dBm and center frequency is 2GHz. Increasing numberOfCircles to 2 would create 57 sectors. The number of nodes is set to zero, because a special kind of node is created later.

.. literalinclude:: ../../../../.createManualsWorkingDir/ofdmascanner.uma.setup

The first line of this code block initializes the openWNS random number generator. It does not affect random numbers generated in Python. For that, one has to include the line ``random.seed(2714)``. Next a simple UT is created and placed at position 1000m, 1000m. Finally mobility is added to the UT. It will scan a hexagon with radius 250m. The inner 25m will not be scanned. This corresponds to the geometry of the Urban Macro scenario. The resolution is 50 steps in x and y direction each. Since the ``creatorPlacerBuilder`` is not used for that additional node, the last line assures it is appended to the list of nodes.

.. literalinclude:: ../../../../.createManualsWorkingDir/ofdmascanner.uma.final

At the end the measuring probes are set up. The probability density function (PDF) of maximal measured SINR, maximal received power, and minimal pathloss is recorded. By taking the maximum or minimum respectively it is assured that measurements come from the BS the UT would be associated to at each position. The last lines set the simulation time to 1000 seconds and force old results to be deleted with each new simulation run.

The simulation should now be finished. Start ``wrowser``. From the ``File`` menu select ``Open Directory``. Select the ``output`` directory and click ``Set Root`` and then ``Scan``. Now ``Draw`` the maximum SINR and minimum pathloss. They correspond to the results found by the WINNER+ evaluation group.






