.. openWNS documentation master file, created by sphinx-quickstart on Thu Oct  9 09:42:32 2008.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to WinProSt Developers Guide!
=====================================

.. ifconfig:: builder=='html'

   Welcome to the new WinProSt documentation. 
   The documentation you see here is also available downloaded as PDF_.

   .. _PDF: winprost-DevelopersGuide.pdf


   **Table of Contents**

.. toctree::
   :maxdepth: 2

   about/AboutThisBook
   todolist

Scheduling Strategies for QoS
=============================

QoS Class Definitions
---------------------
In Winprost you can set a different scheduling strategy for each QoS class.
The QoS classes are defined in ``PyConfig/winprost/qos.py``. 

.. literalinclude:: ../../.createManualsWorkingDir/winprost.qos.example

Scheduling Strategies
---------------------

QoS scheduling is supported via the ``StaticPriority`` scheduling strategy of
the libwns. For each QoS class a scheduling ``SubStrategy`` is statically assigned.
The available substrategies are (cmp. ``openwns/Scheduler.py``).

 * RoundRobin
 * ExhaustiveRoundRobin
 * ProportionalFair (ensures fair datarates for all users, essential for relaying)
 * EqualTimeRoundRobin
 * First Come First Serve (FCFS)
 * Earliest Deadline First (EDF)
 * HARQRetransmission (specialized for HARQ Retransmission handling. Directly communicates with the HARQ colleague)

Scheduler Configuration
-----------------------
The ``StaticPriority`` scheduling strategy keeps a mapping of QoS classes (priorities) to
scheduling substrategies. This mapping must be provided during construction of the ``StaticPriority`` scheduling strategy. Here is a mapping definition. For the PCCH(priority 0)
a round robin strategy is used and an exhaustive round robin for all other priorities.

::

  self.subStrategiesTXDL =(
  openwns.Scheduler.RoundRobin(), # for priority 0
  openwns.Scheduler.ExhaustiveRoundRobin(), # for priority 1
  openwns.Scheduler.ExhaustiveRoundRobin(), # for priority 2
  openwns.Scheduler.ExhaustiveRoundRobin(), # for priority 3
  openwns.Scheduler.ExhaustiveRoundRobin(), # for priority 4
  openwns.Scheduler.ExhaustiveRoundRobin(), # for priority 5
  openwns.Scheduler.ExhaustiveRoundRobin() # for priority 6
  )

Once you have defined such a list you can use it upon construction of the ``StaticPriority``
strategy (you will need several other settings, too).

::

    openwns.Scheduler.StaticPriority(parentLogger=sf.logger, txMode = True,
        subStrategies = self.subStrategiesTXDL,
        dsastrategy=self.dsastrategyDL,
        dsafbstrategy=self.dsafbstrategyDL,
        apcstrategy=self.apcstrategy)


In WinProSt, the ``ResourceScheduler`` must then be told to use the ``StaticPriority`` strategy as its strategy. You can use the ``setStrategy`` method for this. The ``ResourceSceduler`` is created in the mode dependent part of each station. You can find the definition of the mode dependent FUN in ``winprost/TaskFUNs.py``.

Enabling H-ARQ for QoS Classes
------------------------------

You can enable H-ARQ for each QoS class separately. Use the keyword argument ``useHARQ`` to
control the H-ARQ setting. For example:

::

    self.subStrategiesTXDL =(
    openwns.Scheduler.RoundRobin(), # for priority 0
    openwns.Scheduler.ExhaustiveRoundRobin(), # for priority 1
    openwns.Scheduler.ExhaustiveRoundRobin(), # for priority 2
    openwns.Scheduler.ExhaustiveRoundRobin(useHARQ = True), # for priority 3
    openwns.Scheduler.ExhaustiveRoundRobin(), # for priority 4
    openwns.Scheduler.ExhaustiveRoundRobin(), # for priority 5
    openwns.Scheduler.ExhaustiveRoundRobin() # for priority 6
    )

H-ARQ is turned off by default.

Unsorted Notes
==============

* Each relay node has a ModeTaskFUN for the UT and the BS phas
* Each of these funs has its own TimingScheduler which takes care of the proper timing of the BS and UT phases

.. literalinclude:: ../../.createManualsWorkingDir/winprost.TaskFUNs.py.RNBS.example

* The queue size of the scheduler queues is set per CID in the RegistryProxy. It is set to 250000 bits in ResourceScheduler.py of winprost.

Overhead Calculation
====================

* UDP Header : 20 Byte = 160bit
* IP Header : 20 Byte = 160bit

* ROHC takes place in the PDCP. Also PDCP header needs to be added.

* This is what should arrive in the segmenting queue.

* The Segmenting Queue emulates the RLC and should add Fixed and Extension headers appropriately

* Then the MAC Overhead must be added (In case of CID multiplexing the header depends on the number of RLC PDUs)

* HARQ probably adds the channel coding and CRC overhead



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

