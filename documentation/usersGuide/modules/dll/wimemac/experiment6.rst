####################################
Experiment 6: Interference Awareness
####################################

This experiment will show one method to handle interference between transmissions.

.. note::

    The experiments 5 and 6 of this tutorial are not executable with the main branch of the WiMeMAC module. The ``campaignConfiguration.py`` of this experiment is specificly configured for the Simulatoin Grid Engine at the ComNets Research Group.

*****************
Signal Modulation
*****************

------------
Interference
------------

.. figure:: images/experiment6-obstacle.png
   :align: center

Obstacles, e.g. walls, can attenuate the signal between two stations. If the signal is attenuated strong enough, the two stations aren't aware of each others existence; they may use the same MASs in the superframe and both connections can be established if they are separated. But although the signal of the stations may be attenuated, it may still be strong enough to cause interference during the data transmission of the other station. This leads to transmission errors, frames will have to be sent again and the throughput will decrease.
To gain system capacity in this case, the WiMeMAC module offers an additional functionality which can be used through the parameter ``interferenceAwareness`` in its config file. With this parameter set to ``True``, every station uses additional information to create its reservation: Before a reservation pattern is generated, the reservation owner monitors the channel and notes the interference that is already caused by other stations for every MAS. When creating the MAS pattern for its reservation, it chooses the MASs with the least interference first. For more information on the Interference-Awareness feature, see [#f1]_ . The number of reserved MASs does not change directly through this procedure, but since the MASs with much interference are avoided, the frame error rate (FER) is lower and less frames have to be sent again. The reservation owner has the possibility then to choose a better modulation coding scheme (MCS).

------------------------
Modulation Coding Scheme
------------------------

The modulation coding scheme (MCS) is the way the information is coded to a signal. The scheme can be changed by amplitude- or phase-modulation as well as by different data coding. The MCS affects the data rate and the error susceptibility. Every station can switch between several MCSs to adapt the transmission to the current channel situation. If there is not much interference, the transmitting station will choose a MCS that may be susceptible to noise or interference but offers a higher data rate. In case of a higher FER due to interference, frames have to be retransmitted. To prevent that, the station changes to a different MCS that makes the signal less error-prone in exchange for a lower data rate. Although the data rate is lowered, the throughput increases because of the more robust MCS. By using the ``interferenceAwareness`` function, frame errors are avoided and stations are able to choose a faster MCS.


************
The Scenario
************

This scenario is bigger than every other scenario of this tutorial so far. It was part of a diploma thesis [#f1]_ and utilizes most of the capabilities of the WiMeMAC module. It consists of 50 stations that are arranged in an alignment of several walls. This is the scenario setup:

.. figure:: images/experiment6-scenario.png
   :align: center

In this setup, we have 5 seperated rooms, arranged as a cross. In each room are 10 stations; 5 reservation owners and their corresponding reservation targets. The rooms have a size of 6x6 meters. Every station keeps at least 1 meter distance to each wall.

.. note::

    The walls in this experiment differ from the wall we had in experiment 4. That wall had an exaggerated strong signal attenuation to show the hidden node problem. The walls we have here are more realistic, have a weaker attenuation and don't absorb the signal completely.

All the stations in one room can communicate freely with each other and are members of the same beacon group. The signals from stations in other rooms are attenuated because of the walls; they are so alleviated that the stations in the center room are not aware of the existence of the stations in other rooms, but they are still strong enough to cause interference. We expect all the transmissions in this experiment to be slightly defective and we will compare the saturation points of the connections with and without the interferenceAwareness feature.

------------
Config Files
------------

Ok, here we go: Create a ``config.py`` with 50 stations, all the walls and arrange them as shown in the picture above.
.............................just kidding, simply create a new subcampaign and copy the fully prepared config files to your subcampaign directory. The config files are stored in 

.. code-block:: bash

    $ cp ../../myOpenWNS/tests/system/wimemac-addOn-Tests--main--1.0/PyConfig/experiment6/config.py .
    $ cp ../../myOpenWNS/tests/system/wimemac-addOn-Tests--main--1.0/PyConfig/experiment6/campaignConfiguration.py .

Now take a look at the ``config.py``. In this file, each room is referred to as a WPAN. Scroll to the parenthesis where normally the stations are placed into the scenario environment. As you can see, this time the placement is a little more complicated: The position of each node is randomly chosen; it is only subject to some restrictions: Each WPAN contains 5 reservation owners and 5 targets, each node keeps at least 1 meter distance to all walls and there mustn't be two stations at the same position.

.. note::

    Altough the station placement is randomly chosen, the experiment results are still reproducable. The ``config.py`` uses a special random number generator that requires a number as an input value. It generates an arbitrary amount of random numbers from the input value, and if you start the experiment again with the same input value, the same sequence of numbers will be generated.

-------------
Binary Search
-------------

To determine the saturation point of a connection, we have simply set up a row of simulations and looked in the wrowser where not all of the stations could handle the offered load. Now that we have 25 connections, this procedure is very time consuming, so we use a more efficient way to determine the saturation point: The binary search.
The binary search creates dynamically scenarios in the campaign with load-values depending on the results of the previous scenario. It is used to estimate the maximum throughput of a connection with few scenarios. With this method, the first generated scenario will have a very low value for ``throughputPerStation``, like 8 Mb/s. If the whole offered load could be transferred, the saturation point for this connection was not reached in this scenario; binary search generates another scenario and doubles the offered traffic. As long as the saturation point is not reached, the throughput will always be doubled. If the data throughput is below the offered load, the next scenario's traffic will have the arithmetic mean value between the last below-saturation-scenario and the last executed scenario. If the last executed scenario was a below-saturation-scenario, the mean value will be calculated with this scenario and the last over-saturation-scenario.
To make this procedure more understandable, let's look at the example in the picture:

.. figure:: images/experiment6-binarysearch.png
   :align: center

The binary search uses 7 scenarios to estimate the saturation point. After the 7th scenario, the alteration of the value for ``throughputPerStation`` would be very small and is below the abortion-threshold. So the binary search ends after this scenario and declares the 7th measurement point as the saturation point. The value for the first scenario and the abortion-threshold are configurable.

-----------------------
Starting The Experiment
-----------------------

Since this experiment is different from your other experiments (automatic generation of further scenarios, etc.), don't start with the 'create-database create-scenarios' routine. This time, switch to your campaign folder and type

.. code-block:: bash

   $ ./simcontrol.py --create-database --interval=2000

This will already start the campaign due to the configuration in the ``campaignConfiguration.py``. It consists of 2 separate simulations: The scenario will be executed once with the interference awareness feature and once without. The suffix ``interval=2000`` causes that after 2000 seconds the 2 scenarios will be ran again, this time with different values for ``throughputPerStation``, according to the next step in the binary search. When the saturation point for all the connections is determined, there will no further scenarios be generated. 

.. note::

    Even if all the saturation points are determined, the program will continue to check the results of the last cycle; to end the program, press ``Ctrl + C``.

Since it might take 6 or 7 cycles with the binary search to determine the saturation point, and one cycle takes 2000 seconds, this campaign will take a few hours to finish. After the saturation points have been determined, we can display the results with the wrowser. 


***********
The Results
***********

Start the wrowser and select your experiment. This time, we will display two separate diagrams: The first will show the throughput of all 25 transmissions without the interference awareness feature, the second one shows the same transmissions with this feature. 

At first, uncheck the checkbox next to ``4IA-Random-MAS`` since the graphs that belong to this option will be displayed in the second diagram. Then, select ``Figure-> New-> Parameter``, choose ``offeredLoadpLink`` as the simulation parameter and then select ``traffic.endToEnd.window.incoming.bitThroughput`` for all stations with an odd index. The results should look like this:

.. figure:: images/experiment6-Wrowser_resultsNoIA.png
   :align: center

.. note::

    In this scenario, the odd indexes represent the receiving node of each connection. Since we measure the incoming throughput, it would make no sense to include the even indexes who have only outgoing data.

The most conspicuous aspect of these results is that some stations reach a lower throughput despite a higher offered load. The reason for this is that the ``useRelinquishRequest`` parameter in this configuration is set to false. Stations that start to transmit earlier reserve more MASs if the offered load increases, so other stations have less free MASs to set up their own reservations. This effect starts at a throughput of 8.5 Mb/s per link. Since some stations are suppressed at higher values for ``offeredLoadpLink``, this is the saturation point for this scenario. The maximum throughput for the whole system (25 connections) is 25*8.5 Mb/s = 212.5 Mb/s.

Now let's see how the scenario went with interference aware scheduling. Check the checkbox next to ``2Random-MAS`` and uncheck ``4IA-Random-MAS``. Draw the results .

.. figure:: images/experiment6-Wrowser_resultsWithIA.png
   :align: center

At first appearance, these results look similar to the previous. But if you look closer, it is noticable that no station is suppressed before 12.5 Mbit throughput per link. That means, the complete system throughput is 312.5 Mbit. As you can see, the ``interferenceAwareness`` feature increased the maximum throughput by 100 Mb/s.
The results depend on the wall attenuation. With a varying wall attenuation, different parts of the WPANS overlap and it is expected that the capacity gain using interference awareness is maximized for a certain attenuation.


**********
Conclusion
**********

Most of the functions and additional features of the module have been explained as well as the basic knowledge of the WiMedia standard. The intention of this tutorial is to provide an easy introduction to be able to work with the module after a short time. This experiment is the last one of this tutorial and concludes the introduction to the WiMeMAC module.

.. rubric:: Footnotes

.. [#f1] "Interference-Aware Sceduling for QoS Support in UWB Networks" (Jens Frerichs, 2010)

