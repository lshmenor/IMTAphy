###########################################
Experiment 1: Saturation Throughput
###########################################

In the first experiment, we setup a very simple simulation campaign,
run the simulations and evaluate the results while learning the basic
parts of the WiMAC configuration.

In the following, it is assumed that ``~/myFirstCampaign`` is the root
directory of the simulation campaign, created as described in the
WiFiMAC tutorial section ``preparation``, and
``~/myFirstCampaign/experiment1`` is the directory where the
simulations are stored.

In the beginning, this directory contains only the following files:

.. code-block:: bash

   $ ls
   campaignConfiguration.py   simcontrol.py

``simcontrol.py`` is used to manage the simulation, i.e. to create the scenarios,
execute the simulations (either locally or in a distributed grid, if available)
and presenting information about the current status. ``campaignConfiguration.py``
contains the parameters which shall be simulated.



*****************************************
Experiment 1, part 1 - Offered Traffic
*****************************************

The first scenario is kept as simple as possible: One Base Station
(BS) and one Mobile Station (MS) transmit data in uplink and downlink,
the MS is positioned in distance `d` to the BS.

1. In this simulation, we would like to measure the saturation
   throughput in downlink and uplink. Accordingly, a set of
   simulations is created with increasing offered traffic and the
   carried throughput versus offered traffic is plotted. Each
   simulation campaign, independent of its complexity, follows four
   basic steps:

   a. Creation of the simulation configuration file, :ref:`experiment1FirstConfig` - 
      this one is the same for all simulations.

   #. Creation of the parameter file, :ref:`experiment1FirstCampaignConfig` - this one
      contains the parameters that differentiate the simulations from each other,
      i.e. the different offered traffic in our case.

   #. Execution of the simulations using :ref:`experiment1FirstSimcontrol`.

   #. Viewing of results using the :ref:`experiment1FirstWrowser`.


.. _experiment1FirstConfig:


config.py
---------

To complete the campaign, a configuration file ``config.py`` is required that
configures the scenario, stations and the evaluation. For the first experiment, a 
config.py can be found in 
``myOpenWNS/tests/system/wimac-tests/PyConfig/experiment1/``, this 
file needs to be copied into the simulations directory 
(``myFirstCampaign/experiment1``):

.. code-block:: bash

   $ cd ~/myFirstCampaign/experiment1
   $ cp ~/myOpenWNS/tests/system/wimac-tests/PyConfig/experiment1/config.py .

Take a look at the first lines of the configuration file ``config.py`` and you 
can see how to adjust the parameters of this scenario:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment1.config.simulationParameter
   :language: python

The most important statement is the first one:

.. code-block:: python

   from openwns.wrowser.simdb.SimConfig import params

Here, a parameter class ``params`` is imported. It is required for the
automatic generation of scenarios in the campaign: the object
``params`` contains member variables for every parameter that will be
changed in the campaign. In this case, it is the offered traffic ``params.offeredTraffic``.

Besides these parameters, this section also sets the packet size, the
packet scheduling strategy, simulation settling time and five other
parameters.

.. _experiment1FirstCampaignConfig:



campaignConfiguration.py
------------------------

To set different values for the parameters of the simulations, a second file besides the
``config.py`` is necessary: the ``campaignConfiguration.py``.
For the first experiment, a prepared ``campaignConfiguration.py`` can be found in
``~/myOpenWNS/tests/system/wimac-tests/PyConfig/experiment1/``, this
file needs to be copied into the simulations directory, overwriting the existing one:

.. code-block:: bash

   $ cp ~/myOpenWNS/tests/system/wimac-tests/PyConfig/experiment1/campaignConfiguration.py .

Two sections in this files are especially interesting for the simulation: First,
the parameter class ``Set`` is defined that contains all simulation parameters that
are used in ``config.py``:

.. literalinclude:: ../../../../.createManualsWorkingDir/wimac.tutorial.experiment1.campaignConfiguration.Set
   :language: python

Next, an instance with the same name as in the ``config.py`` is created:

.. literalinclude:: ../../../../.createManualsWorkingDir/wimac.tutorial.experiment1.campaignConfiguration.params
   :language: python

.. The class ``Set`` contains the function ``setDefaults()``. Calling this functions,
   default values are defined for all parameters.

Then, the parameters in ``params`` can be populated with different values. Each
time the ``write()`` member function (inherited from the class ``Parameters``) is
called, the current values are fixed and represent one simulation:

.. literalinclude:: ../../../../.createManualsWorkingDir/wimac.tutorial.experiment1.campaignConfiguration.offeredTraffic
   :language: python

With this setup, 5 simulations are created, differentiated by the
offered downlink traffic between 0.01 and 10.01 Mbps. This concludes the file 
``campaignConfiguration.py``.

.. _experiment1FirstSimcontrol:



simcontrol.py
-------------

We are now ready to create the simulations and let them run.  As mentioned earlier,
simulation execution is controlled by the script ``simcontrol.py``. With the command

.. code-block:: bash

   $ ./simcontrol.py --create-database

the ``campaignConfiguration.py`` is executed and the parameter values
are written to the database. Tip: You do not have to type the full
option. As long as it is unique you can just type the beginning. In
this case ``--create-d`` will also work to create the database.

Then, the command

.. code-block:: bash

  $ ./simcontrol.py --create-scenarios

reads the database and creates a sub-directory for every scenario.
This can be validated by calling

.. code-block:: bash

   $ ./simcontrol.py -i

      id   state        [...]       offeredTraffic
        1  NotQueued                      10000.0
        2  NotQueued                    2510000.0
        3  NotQueued                    5010000.0
        4  NotQueued                    7510000.0
        5  NotQueued                   10010000.0

Before running all simulations, a single one can be tested (e.g. for typos in 
``config.py``) by changing into one of the new created directories and running

.. code-block:: bash

   $ ./openwns-dbg

If everything works right, the logging output of the simulation is printed 
(consisting of the simulation time, the module, and the output), until the
simulation time reaches 1.1 seconds and the simulation ends with

.. code-block:: bash

   wns::simulator::Application: shutdown complete

Have a look at the output of the traffic source and sink
``CONST``. How many packts were generated, how many were successfully
received?

After this test, the simulations can be run one-by-one using the
``simcontrol.py`` script:

.. code-block:: bash

   $ ./simcontrol.py --execute-locally
   Executing scenario with id: 1
   Executing scenario with id: 2
   Executing scenario with id: 3
   Executing scenario with id: 4
   Executing scenario with id: 5


This starts the serial execution of all defined scenarios. In a "production"
environment, a grid engine could be used to queue all simulations and run them
in parallel; the script is configured to work together with the SunGridEngine_
e.g. with the command instead:


.. code-block:: bash

   $ ./simcontrol.py --queue-scenarios

.. rubric:: Footnotes

.. _SunGridEngine: http://gridengine.sunsource.net/

The installation and configuration of this grid is out of the scope of
this tutorial.

After some time, all 6 simulations should be finished, which can be
controlled again with

.. code-block:: bash

   $ ./simcontrol.py -i

    id    state  [...]  simTime prog   sgeId host   offeredTraffic
    1   Finished         1.10s  100.00%                   10000.0
    2   Finished         1.10s  100.00%                 2510000.0
    3   Finished         1.10s  100.00%                 5010000.0
    4   Finished         1.10s  100.00%                 7510000.0
    5   Finished         1.10s  100.00%                10010000.0

Each simulation directory now contains a directory ``output``, where all probe
output is stored in text files. Additionally, the output is stored in the
database, which can be accessed much more user-friendly than viewing text files.

.. _experiment1FirstWrowser:



Wrowser
-------

The Wrowser ("Wireless network simulator Result brOWSER") is the openWNS graphical
user interface to browse, i.e. plot, simulation results in a fast and convenient
way. We assume that the Wrowser is installed according to :ref:`gettingStartedWrowser`.
Thus, the Wrowser is started by calling

.. code-block:: bash

   $ wrowser

In the menu ``File`` are the different options to read the generated simulation data,
we select ``Open Campaign Database`` and then under the appropriate user the
campaign with the chosen name, see :ref:`figure-wimac-experiment1-wrowser-selectCampaign`.

.. _figure-wimac-experiment1-wrowser-selectCampaign:

.. figure:: images/experiment1-wrowser-selectCampaign.*
   :align: center

   Campaign selection

After reading the simulation parameters from the database, the
Wrowser will display all possible parameter combinations, see
:ref:`figure-wifimac-experiment1-wrowser-parameter`.

.. _figure-wimac-experiment1-wrowser-parameter:

.. figure:: images/experiment1-wrowser-parameter.*
   :align: center

   Parameter selection

We do not want to deselect any of the simulations, but draw the graph of the offered
traffic versus the obtained throughput measured by the top MAC-Layer. Thus, the
Wrowser needs to get, for all six simulations, the throughput, combine it with
the offered traffic parameter and draw this to a graph. This type of combination
plot is named "Parameter Plot" in the Wrowser. We select Figure -> New -> Parameter.
In the new window, the simulation parameter has to be set to ``offeredTraffic``,
this will be displayed on the x-axis. For the y-axis, we select ``wimac.top.window.incoming.bitThroughput_BS_Id1_PDF.dat``
and select ``Draw`` in the bottom to see a figure as in :ref:`figure-wimac-experiment1-wrowser-throughput`.

.. _figure-wimac-experiment1-wrowser-throughput:

.. figure:: images/experiment1-wrowser-throughput.*
   :align: center

   Throughput plot

In the new figure, we should see that the uplink traffic has reached the saturation
point at 7.3 Mbps. Now check the downlink.

Analytical verification of the results is straight forward: There are
384 OFDM subcarriers in a symbol at 5MHz bandwidth. 18 symbols are
available for user data in every 5ms frame. Modulation is QAM64
(``~/myOpenWNS/modules/wimac/PyConfig/wimac/LLMapping.py``), coding
rate is 0.917
(``~/myOpenWNS/framework/rise/PyConfig/rise/CoderSpecification.py``).

Another interesting figure is the relation of offered traffic versus
the packet delay, evaluated as a probability function. This can be
done by choosing a ``PDF/CDF/CCDF`` graph in the ``Figure`` menu and
plotting the probe ``wimac.top.packet.incoming.delay_BS_Id1_PDF``

The Packet Delay is only meaningful in underload, because the infinite
delay of lost or dropped packets in overload is not considered in this
metric. From the previous figure we can derive the saturation
throughput. Therefore we deselect the last two simulations with a
``offeredTraffic`` beyond 7.3Mbps. Try also the ``Filter Expression``
box to deselect traffics beyong the saturation point.  In the
``Configure Graph`` menu which is called using the button next to the
''Draw'' button, we select the ``Line markers`` as ``None``.

:ref:`figure-wimac-experiment1-wrowser-configureGraph_b`.

.. _figure-wimac-experiment1-wrowser-configureGraph_b:

.. figure:: images/experiment1-wrowser-configureGraph_b.*
   :align: center

   configure graph


The probability for a higher delay increases as the offered traffic
increases, as visible in the following figure.

:ref:`figure-wimac-experiment1-wrowser-uplink-delay`.

.. _figure-wimac-experiment1-wrowser-uplink-delay:

.. figure:: images/experiment1-wrowser-uplink-delay.*
   :align: center

   Delay plot

Now compare uplink and downlink delays for 5.01 Mbps. How is the delay
distributed? Why do the results differ? Tip: This is a Time Division
Duplex (TDD) configuration.

*******
Details
*******

So far, we have just used the prepared ``config.py``, without the
knowledge how it generates the simulation scenario. In the following
sections, we will go step by step through the different parts of the
``config.py`` and learn what is necessary to setup the scenario and
the WiMAC module.



Simulation Parameters
---------------------

The file ``config.py`` for this scenario begins with the simulation
parameters to allow for a better overview and an easy change of
parameters.

.. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment1.config.simulationParameter
   :language: python

The statement ``from openwns.wrowser.simdb.SimConfig import params`` is required for the
automatic generation of scenarios in the campaign: the object
``params`` contains member variables for every parameter that will be
changed in the campaign. In this case, they are

#. the carrier bandwidth (bandwidth),
#. the distance between the BS and the MS (distance) and
#. the offered traffic (offeredTraffic).

Besides these parameters, this section also sets the packet size, the
packet scheduling strategy, simulation's settling time and five other
parameters.


Import Statements
-----------------

Then the Python code that generates the scenario starts. First,
several modules are imported:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment1.config.imports
   :language: python

Namely, we import

* The random number generator

* The simulator core ``openwns``, which includes different types of
  libraries, for example classes to define dB, dBm and an interval.

* The scenario package to define the radio environment.

* The IP Backbone Helpers for DNS, DHCP and ARP (all done virtually)

* From wimac:

  - The nodecreator package that allows the generation of stations
  - The WiMAC Helpers to setup traffic, the scheduler and the channel model
  - The evaluation structure for the WiMAC



WNS Core Configuration
----------------------

The next section creates one instance of the openWNS

.. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment1.config.WNS
   :language: python

The output strategy ``DELETE`` assures that old simulation output is
deleted prior to the simulation. The write interval of the
status-report and the probes is set.



Scenario
--------

The scenario building basically consists of the following definitions

* the positions of stations
* the transceivers
* the protocol stacks of the MAC

After passing the default configuration Config.parametersPhy to the wimac module, the
creation of the scenario instance is done by using the scenario and wimac module;

.. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment1.config.scenario
   :language: python

The positions of the stations can be defined by using the
placers. (e.g.  ``HexagonalPlacer``, ``LinearPlacer``) which guarantee
a specific distribution of the positions. Here, the positions of BSs
comply a hexagonal grid with the chosen number of rings
``numberOfCircles`` around a center cell. All positions of MSs are on
a line according to the position list ``positionList``. Because we
only study a scenario with one BS and one MS we chose
``numberOfCircles`` equal to zero and ``numberOfNodes`` equal to
one. The different types of placers can be chosen from the python
classes in the path
``~/myOpenWNS/framework/scenarios/PyConfig/scenarios/placer/``.

Next, the antenna type of the BS is defined with a position offset
relative to the BS positions. Here we only have one antenna per BS
with a height of 5 meters.

The protocol stack of the stations is defined by the creator. The
creators ``WiMAXBSCreator`` and ``WiMAXUECreator`` are used, which can
be found in ``nodecreators.py`` in the folder
``myOpenWNS/modules/dll/wimac/PyConfig/wimac/support/``.

The scenario is built by the
``scenarios.builders.CreatorPlacerBuilder`` using the above mentioned
parameters and functions.



Radio Channel
-------------

The configuration of the radio channel propagation parameters contains
modules for three major effects:

* Pathloss,
* Shadowing and
* Fast fading

.. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment1.config.radioChannel
   :language: python

Line-of-Side Test (LoS_Test) radio channel is chosen. More details about how the
radio channel can be configured can be found in the file ``wimac.support.helper.py``.



Evaluation
----------

In the final lines, the evaluation is installed, creating probe output
for the WiMAC station types BSs and MSs with a certain window and
settling time:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment1.config.Probing
   :language: python



************************************* 
Experiment 1, part 2 - Bandwidth
************************************* 

2. Find the impact of increased ``bandwidth`` from 5MHz to 10 and 20
   MHz on the saturation throughput by editing the
   ``campaignConfiguration.py``. and ``config.py``.

   a. Change the static setting in ``config.py`` to a variable parameter that gets
      its value from the imported instance ``params``.

   #. Add the parameter ``bandwidth`` to the class ``Set`` in the ``campaignConfiguration.py``.
      As existing simulations do not have this parameter, but have used 5MHz, we
      add this as the default value by specifying::

            bandwidth = Int(default = 5)
            
      The default value is to be set also by adding ``params.bandwidth = 5`` 
      after the command ``params = Set()``, e.g.::
      
            params = Set()
            params.bandwidth = 5

   #. Modify the existing for-loop to generate different offered traffic between 
      0.001 and 35.001 Mbps. Add an inner for-loop to vary the bandwidth between
      5 and 20 MHz::

            for rate in [0, 10, 15, 30, 35]:
                for bandw in [5,10,20]:
                    params.offeredTraffic = (0.01 + rate) * 1e6
                    params.bandwidth = bandw
                    params.write()


   #. It is not necessary to delete existing scenarios,
      ``simcontrol.py`` will automatically identify the missing
      simulations and create them when told so.  Every time you modify
      the campaign parameters, first call ``simcontrol.py
      --create-database`` to add entries to the database and
      ``simcontrol.py --create-scenarios`` to create the simulation
      directories accordingly. Create the simulations (in the database
      and the scenarios). Use ``simcontrol.py -i`` to assure
      everything went right. Execute the simulations using
      ``./simcontrol.py --execute-locally --restrict-state=NotQueued``
      to assure only the new simulations are executed.

   #. Evaluate the impact of the bandwidth on the saturation point
      using the Wrowser.




***********************************
Experiment 1, part 3 - Distance
***********************************

3. Now, we want to vary another parameter ``distance`` to select a distance between 
   BS and MS of 0.2km to 12km (with an offered traffic of 10.01 Mbps and a 
   bandwidth of 5 MHz).

   #. Change the static setting in ``config.py`` to a variable parameter that 
      gets its value from the imported instance ``params``.

   #. Add the parameter ``distance`` to the class ``Set`` in the
      ``campaignConfiguration.py``. As existing simulations do not have this 
      parameter, but have used 100m, we add this as the default value by specifying::

            distance = Float(default = 100.0)

      The old default value has to be removed by editing 
      ``bandwidth = Int(default = 5)`` into::

            bandwidth = Int()

      The default value is to be set also by adding ``params.distance = 100`` 
      after the command ``params = Set()``, e.g.::

            params = Set()
            params.bandwidth = 5
            params.distance = 100

   #. Replace the existing for-loop by the following code to vary the distance 
      between 2100m and 12100m::

            for dist in xrange(0,7):
                params.offeredTraffic = 10 * 1e6
                params.bandwidth = 5
                params.distance = 100 + dist * 2000
                params.write()

   .. Create the simulations (in the database and the
      scenarios). Check them using the ``-i`` switch.

   #. Evaluate the impact of the ``distance`` on the saturation point
      using the Wrowser. Use the ``toggle`` button to select the right
      simulations.

























