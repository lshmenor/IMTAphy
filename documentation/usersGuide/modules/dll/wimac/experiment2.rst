#################################
Experiment 2 : Packet Scheduling
#################################

In this experiment we extend the previous deployment by one additional MS. In 
the single cell scenario with one BS and two MS we will study the impact of 
different scheduling strategies on the throughput and fairness.

In Wireless Metropolitan Area Network (WMAN) systems a central scheduler allocates
resources (such as time, frequency, or transmit power) to MSs within a cell and 
forwards its scheduling decisions within a MAP-message every frame. Additionally
data packets are selected by the scheduler which are being transmitted on the 
allocated resources. 

The scheduler framework within the openWNS allows to orthogonalize the 
scheduling of the different dimensions and thereby to independently choose
different strategies for each resource allocation (such as dynamic subchannel assignment (DSA),
adaptive modulation and coding (AMC), adaptive power control (APC)) and the packet 
scheduling. 

In this experiment we study the impact of different strategies for the 
packet scheduling on the throughput and fairness as well as briefly examine the
effect of different strategies for DSA on the frame occupation. During DSA 
transmission opportunities are defined in time and frequency and are dedicated 
to MSs.



************
Preparation
************


:Add new Probe:
  In order to differentiate and to gather data rate from different links we add 
  a third Probe "aggregated.bitThroughput" to the ``TutorialEvaluation``. By 
  uncommenting the corresponding line ``wimac.top.window.aggregated.bitThroughput``
  in the file ``default.py`` which is in the folder 
  ``myOpenWNS/modules/dll/wimac/PyConfig/wimac/evaluation/``:

  .. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment2.evaluating.tutorialEvaluation
     :language: python

:Update campaign:
  Using ``./playground.py preparecampaign PATH`` with the same ``PATH``
  as in the preparation for experiment 1 (namely ``~/myFirstCampaign`` according
  to the chapter "Preparations" ), we can update the sandbox and apply the made 
  changes to the probing. When prompted by ``./playground.py``, please select 
  ``(U)pdate the sandbox``


:Create sub-campaign:
  Using ``./playground.py preparecampaign PATH`` again with the same ``PATH``, we can 
  create a sub-campaign that uses the same sandbox, but a differed directory to 
  store the simulations. When prompted by ``./playground.py``, please select 
  ``(C)reate a sub-campaign`` and choose as name e.g. ``experiment2``.


:Configuration files:
  Required configuration files ``config.py`` and ``campaignConfiguration.py`` 
  can be found in ``~/myOpenWNS/tests/system/wimac/PyConfig/experiment2/``,
  this file needs to be copied into the simulations directory 
  (``~/myFirstCampaign/experiment2``), e.g.:

   .. code-block:: bash
       $ cd ~/myFirstCampaign/experiment2
       $ cp ~/myOpenWNS/tests/system/wimac/PyConfig/experiment2/config.py .
       $ cp ~/myOpenWNS/tests/system/wimac/PyConfig/experiment2/campaignConfiguration.py .

:Reduce bandwidth:
  These files use a reduced system bandwidth of 1.25 MHz (although the standard 
  considers a minimal bandwidth of 5MHz) in order to speed up the simulations 
  campaign by reducing the required traffic load.


:Second MS:
  The second MS is also placed by the ``LinearPlacer`` by the following line in 
  the ``config.py``:

  .. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment2.config.scenario
     :language: python

  creating two MS on a line at 100m and 3000m from the BS.


:Change probing type:
  As we do not need the distribution of of our metrics anymore, we secondly change
  our Probing type from ``PDF`` to ``Moments`` in the following line in the ``config.py`` :

   .. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment2.config.probing
      :language: python


*******************
Static Factory
*******************
In previous tutorials we modified values of parameters in the PyConfig but the 
protocol stack of the simulated communication systems and the C++ program were 
left unchanged. Now we will change the protocol stack and the C++ program by means 
of the ``StaticFactory``. 

Within a given context, a number of different strategies to accomplish a task may
exist. Depending on the situation (here configuration), one of those strategies 
should be chosen. To make strategy selection configurable, the strategies are 
encapsulated in classes. These classes share an abstract strategy interface 
accessed by the context. The strategy can then be replaced without modification 
of the context. 
An example for the use of the strategy pattern is the ``Dropping buffer``.
The ``Dropping buffer`` can be configured to use one of two dropping strategies: ``Front``
and ``Tail``. Internally, those names are used to create instances of dropping strategy
classes. Those strategy classes encapsulate the selection of candidates to drop.

All these instances are created by the ``StaticFactory`` which provides an easy to 
use interface for creating instances. Strategies register at a ``StaticFactory``
in a decentralized manner by utilizing static, namespace-level objects. Before
the main function starts execution, all strategies have already been registered 
at the ``StaticFactory``. If shared objects containing strategies are dynamically
loaded, all strategies defined within the shared object get registered during 
shared object initialization. There is no central point of strategy registration.

Every "StaticFactory" is parametrized by a creator, which itself is configured by
the strategy TYPE (the abstract strategy interface) and the concrete 
implementation KIND and enforces a strategy constructor signature. Every specific 
strategy registers at the ``StaticFactory`` using its abstract strategy interface
and the creator suiting the abstract strategy constructor signature. 

Let's see at the example of packet scheduling how the ``StaticFactory`` is used. 
The packet scheduling can be configured to use one of the strategies:
``Round Robin``, ``Proportional Fair``, ``Exhaustive Round Robin``, and 
``Fixed Resources``. Internally, those names are used to create instances of 
the classes of packet scheduling strategy which encapsulate the type of selection 
of packets to schedule.

According to the string in the parameter ``Config.scheduler`` being set in the 
``config.py`` the function 
``wimac.support.helper.setupScheduler(WNS, Config.scheduler)`` in the file 
``~/myOpenWNS/modules/dll/wimac/PyConfig/wimac/support/helper.py`` 
the specific type of scheduler in terms of DSA- and packet scheduling strategy 
is firstly selected and the schedulers are secondly instantiated in a loop for 
each BS:

    .. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment2.staticFactory.substrategy.ProportionalFair.helper.py
       :language: python

For instance for a chosen string ``PropFair`` the packet scheduling strategy 
``ProportionalFair`` and a DSA strategy ``LinearFFirst`` are selected . In this 
manner working combinations of the two strategy types are suggested and can be 
easily configured. By using ``setupSchedulerDetail()`` instead of 
``setupScheduler()`` the strategy for packet scheduling and DSA can be chosen 
independently.

A class is registered at the ``StaticFactory`` by a name which is used twice 
in the code. Once in the python file and once in the c++ code. 
The ``subStrategy`` (TYPE) (or strategy for packet scheduling) of 
``ProportionalFair`` (KIND) is registered once in the python file 
``/myOpenWNS/framework/library/PyConfig/openwns/Scheduler.py`` with 
the keyword ``__plugin__``:

  .. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment2.staticFactory.substrategy.ProportionalFair.openwns.Scheduler.py
     :language: python

and once at the beginning of the c++ file ``/myOpenWNS/framework/library/src/scheduler/strategy/staticpriority/ProportionalFair.cpp`` 
referring to the ``SubStrategyInterface``:

  .. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment2.staticFactory.substrategy.ProportionalFair.cpp
     :language: c++


*******************************************************
Experiment 2 - packet scheduling strategies (part 1) 
*******************************************************

1. We will again increase the offered traffic with varying scheduling types namely 
   ``Round Robin``, ``Proportional Fair``, ``Exhaustive Round Robin``, and 
   ``Fixed Resources``.

   a. Create the simulations (in the database and the scenarios) and execute them.

   #. Evaluate the impact of the strategy on the cell throughput as well as on 
      the individual throughput using the Wrowser. Exemplarily you can evaluate the
      downlink throughput graphs of both users for each scheduler configuration.
      
   #. Which strategy is fair? And in what terms (e.g. resources or data rate) ?



*******************************************************
Experiment 2 - dynamic subchannel assignment (part 2) 
*******************************************************

2. In the second part we like to examine the impact of the DSA strategy on the frame
   occupation. The scheduling strategies ``Fixed`` and ``Round Robin`` may yield similar 
   throughput results but they use different DSA strategies, namely ``Linear Frequency First`` 
   and ``Fixed``.
      
   a. Recording the frame occupation can be activated by uncommenting the
      following line at the end of the ``config.py``:
       
            ``wimac.evaluation.default.installJSONScheduleEvaluation(WNS, loggingStationIDs)``

      Change the line ``settlingTime = 0.1`` to ``settlingTime = 0.05`` to start probing earlier.
      Also look for the line ``WNS.maxSimTime = 0.9`` and change it to ``WNS.maxSimTime = 0.06`` to reduce the simulation time. 
      
   #. In order to evaluate the differences of the DSA- strategies we will study 
      the frame occupation in a middle load situation at 0.98 Mbps for the scheduling 
      ``Round Robin`` and ``Fixed``. You can get the corresponding simulation 
      ``SCENARIOID`` (and folder name) by the command ``./simcontrol.py -i``. The WiMAC 
      simulator is configured in a manner that the frame occupation can only be 
      probed in the debug (dbg) mode. Enter the folder:
        
        .. code-block:: bash

            $ cd SCENARIOID

        
   #. Run the single simulation in debug mode
        
        .. code-block:: bash

            $ ./openwns-dbg
   #. Do this for both, the ``Round Robin`` and ``Fixed`` simulation.
        
   #. Watch the resulting frame occupation of these two simulations by using the
      Wrowser according to the CouchDB_.
        
        .. _CouchDB: http://docs.openwns.org/UsersGuide/CouchDB.html
   
   #.  How does the resource assignment differ? 

