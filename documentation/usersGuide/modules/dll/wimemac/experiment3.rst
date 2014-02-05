#####################################
Experiment 3: Reservation Negotiation
#####################################

The purpose of this experiment is to give an introduction to the communication between the stations.
Before you can start the next experiment, you need to know how a connection between two stations is established. Therefore the following section contains some basic knowledge of the WiMedia standard.

************************
Transmission Negotiation
************************

Frames and MASs
----------------

.. |mu| unicode:: U+003BC
   :rtrim:

Every station divides time into slots, called MASs (medium access slots). Every MAS has a length of 256 |mu| s. 256 MASs form one superframe. The slots in a superframe are numbered from 0 to 255. The WiMeMAC module uses the Distributed Reservation Protocol (DRP) to initiate and maintain transmissions: Before a station is allowed to transmit data, it has to reserve MASs. Afterwards, it broadcasts to every station in its neighbourhood (i.e. to every reachable station) in which MASs it is going to send. This is done to avoid collisions if two stations may want to send data at the same time.

.. figure:: images/experiment3-Superframe_blank.png
   :align: center

The first few MASs of each superframe are reserved to transmit beacons. Beacons are messages that inform all neighbours of a station (i.e. all other stations within the transmission range) about existing or upcoming MAS reservations. The time reserved for beacons is called `beacon period`. The remaining time of the superframe is called `data transfer period`. User data transmissions are only allowed within the data transfer period. Once the MASs are reserved, the station is able to send data in these MASs in every superframe. If another station wants to start a transmission now, it has to reserve other MASs so that the two transmissions do not collide with each other.

.. figure:: images/experiment3-Superframe_twoReservations.png
   :align: center

The more load per station is offered, the more MASs are reserved by that station. If a station offers a lot of load per second, this station might reserve the whole superframe to transmit its data. 

.. figure:: images/experiment3-Superframe_full.png
   :align: center

If the offered load increases further, not all of the data can be transferred since the maximum capacity of the system is reached. This is called `saturation` (compare to the results of experiment 2).


Reservation Patterns
--------------------

The MAS reservations in the pictures above have one thing in common: All the reservations consist of adjacent reserved MASs, forming a reservation block. In each reservation block a guard time of 12 |mu| s at the end of the block is needed where no data is transferred. The guard time ensures that different transmissions won't overlap.
To place the reservation into one block has advantages and disadvantages. The greatest advantage of a contiguous MAS reservation is to reduce the amount of unused transmission time: Transmitted data is always put together to frames. Since every reservation block can only contain an integer number of frames, there is a small unused offcut at the end of each block which is too small to contain another frame, so no data is transmitted within this offcut. If the reservation is split into many reservation blocks, channel time is wasted in the superframe caused by unusable transmission time and by the guard time. This leads to a lower system capacity since less data can be transferred.
But using one reservation block has also a disadvantage: Since the frames are provided at the sending station before as well as after the time of the reservation block (compare following figure), some of the data frames have to be buffered for almost one superframe, before they can be transmitted to the receiving station.

.. figure:: images/experiment3-Superframe_delay.png
   :align: center

That leads to an increased frame delay which is disadvantageus for transmissions with strong delay demands such as voice traffic.


*****************************
Manipulating The Packet Delay
*****************************

For this experiment we will create a new subcampaign within the campaign folder. First, switch to your OpenWNS directory. Type

..  code-block:: bash

    $ ./playground.py preparecampaign ../campaigns

This is the same command as for creating a new campaign. After some time the script will prompt you that the campaign folder already exists.

.. code-block:: bash

    Shall I try to (U)pdate the sandbox or do you want to (C)reate a new sub campaign? Type 'e' to exit (u/c/e) [e]:

Create a new subcampaign and choose ``experiment3`` as the folder name. After choosing a name and a description, the subcampaign is created. Since the simulations in this experiment are similar to the one before, copy the ``config.py`` and the ``campaignConfiguration.py`` from your previous campaign folder to your new subcampaign directory.

In this experiment the effect of the number of reservation blocks on the frame delay will be determined. First, open ``campaignConfiguration.py``; extend the parameter set `params` by one new parameter ``reservationBlocks`` (Integer). Then set the variable ``throughputPerStation`` to a fixed value (40E6) and include ``reservationBlocks`` in the for-loop like this:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wimemac.tutorial.experiment3.campaignConfiguration.initialization
   :language: python

Afterwards, you need to apply the new parameter to the ``config.py`` file. Open the file and look for this line

.. code-block:: python

        reservationBlocks = 1

and change the value to the corresponding variable of ``params``. Now the changes to the config files are already completed. You can also find a copy of the already changed files in

.. code-block:: bash

    $ cd ../../myOpenWNS/tests/system/wimemac-Tests--main--1.0/PyConfig/experiment3/config.py
    $ cd ../../myOpenWNS/tests/system/wimemac-Tests--main--1.0/PyConfig/experiment3/campaignConfiguration.py

Create the database, the scenarios and start the simulation campaign. When all of the 6 simulations are completed, open the Wrowser and select your new subcampaign. We want to display the frame delay, so do the same as in experiment 2: Select ``Figure-> New-> PDF/CDF/CCDF``, mark the ``traffic.endToEnd.packet.incoming.delay\_wns.node.Node.id_X\_PDF`` probe (with the corresponding ID as 'X') and switch the drop-down list under the probes to CCDF. Don't forget to set the Y-Axis to a logarithmic scale (lg) via the ``Configure...`` button. After clicking ``Draw``, zoom to the left part of the diagram like in experiment 2. If everything went right, the graph should look like this:

.. figure:: images/experiment3-Wrowser_delayZoomed.png
   :align: center

This figure shows that a higher number of reservation blocks leads to a smaller frame delay. The reason for not splitting the reservation into many small reservation blocks is the low system capacity. As mentioned before, in every reservation block channel time is wasted. If there are multiple parallel transmissions and each of them uses many reservation blocks, the superframe would look like this:

.. figure:: images/experiment3-Superframe_GuardTime.png
   :align: center

Since a significant amount of resources would be unused, there would be less time in each superframe for the transmission of user data. The optimal solution for this problem is a compromise between a one-block-reservation and the multiple split up. The compromise depends on the Quality of Service demands of the used application. By adjusting the number of reservation blocks, the maximum delay can be set; a minimum number of reservation blocks can be used that still fits the delay demands and waste as few cannel time as possible. For further experiments, we will set the parameter ``reservationBlocks`` to 2.

In the next experiment, you will learn about signal interference and the hidden-node-problem.
