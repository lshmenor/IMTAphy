##############################################
Experiment 3 : Cellular scenario (not ready!!)
##############################################

.. Gezeigt wird der Einfluss verschiedener DSA-strategien auf die Varianz
   der Interferenz, die Rahmenbelegung und die Cell Spectral
   Efficiency.  Auswirkung der Cluster Order und Sectorisierung auf das
   SINR und die cell spectral efficiency.  Prozess der
   Interferenzschätzung soll erlaeutert werden. Scenario Viewer und
   Interferenz Cache (kks) werden vorgestellt.


In this experiment we will study the impact of scheduling strategies on the 
performance of the wireless communication system in a cellular scenario with a 
specific frequency reuse pattern. In a frequency reuse pattern of NxSxK, the 
network is divided into clusters of N cells (each cell in the cluster has a 
different frequency allocations), S sectors per cell, and K different frequency 
allocations per cell.

With fixed reuse patterns we can only evaluate the central cell with its
corresponding BSs and MSs. The stations in the surrounding cells only produce 
interference for the central cell and are not evaluated in the following. The 
cells or sectors operating on different frequency bands are assumed to not 
interfere the central cell and hence they are not simulated.

The evaluated scenario consists of 7 cells, each with 3 sectors
and 6 mobile stations per cell. Their positions remain unchanged during the 
simulations.


************
Preparation
************


:Add new Probe:
  In order to measure the precision of the SINR estimation, we add 
  a fourth Probe "wimac....deltaSINR" to the ``TutorialEvaluation``. By 
  uncommenting the corresponding line ``wimac.top.window.aggregated.deltaSINR``
  in the in the file ``default.py`` which is in the folder 
  ``myOpenWNS/modules/dll/WiMAC--main--1.0/PyConfig/wimac/evaluation/``:

  .. literalinclude:: ../../../../../.createManualsWorkingDir/wimac.tutorial.experiment2.evaluating.tutorialEvaluation
     :language: python

:Update campaign:
  Using ``./playground.py preparecampaign PATH`` with the same ``PATH``
  as in the preparation for experiment 1 (namely ``../myWiMACCampaign`` according
  to the chapter "Preparations" ), we can update the sandbox and apply the made 
  changes to the probing. When prompted by ``./playground.py``, please select 
  ``(U)pdate the sandbox``


:Create sub-campaign:
  Using ``./playground.py preparecampaign PATH`` with the same ``PATH``, we can 
  create a sub-campaign that uses the same sandbox, but a differed directory to 
  store the simulations. When prompted by ``./playground.py``, please select 
  ``(C)reate a sub-campaign`` and choose as name e.g. ``Experiment3``.


:Configuration files:
  Rerquired configuration files ``config.py`` and ``campaignConfig.py`` can be 
  found in ``myOpenWNS/tests/system/WiMAC-Tests--main--1.2/PyConfig/experiment3/``,
  this file needs to be copied into the simulations directory 
  (``myWiMACCampaign/experiment3``), e.g,:

   .. code-block:: bash

     $ cp ../../myOpenWNS/tests/system/WiMAC-Tests--main--1.2/PyConfig/experiment3/config.py .



*******************************************************
Experiment 3 - DSA Strategy 
*******************************************************


1. In this experiment we will study the impact of the DSA strategy on the 
   precision of the SINR estimation in a cellular scenario with a frequency 
   reuse pattern of (1x3x1). We will compare the DSA strategy ``random`` versus
   ``fixed`` decrease delta interference.

   a. Create the simulations (in the database and the scenarios) and execute them.

   #. Evaluate the impact of the  DSA strategy on the variance of the
      interference, the frame occupation, and error of the estimation of the SINR
      using the Wrowser. 
      Exemplarily you can evalute the downlink throughputs graphs of both users 
      for each scheduler configuration.
      
   #. Which strategy yields precise SINR estimations, more constant interference
      level per link or user, and how do the strategies differ in their impact 
      on the frame occupation?






Interference Cache
==================

The Scheduler of the sending station must have a rough estimation of
the interference level at the receiver to choose the appropriate
modulation and coding scheme. The measured SINR at the receiver
indicated by the PHY are stored in the *Interference Cache*
which are averaged by:

.. math::
  \hat{\mu}_{\gamma}[k] = \left\{ \begin{array}{c@{\quad}l}
      \gamma[0] & k=0 \\
      (1-\alpha_{avg})\hat{\mu}_{\gamma}[k-1]+\alpha_{avg}\gamma[k] & k>1
    \end{array} \right.

The measured values for SINR are directly stored at the
Interference Cache of the sender. The transmission of the measured
values to the sender is not simulated.

.. code-block:: cpp

  void storeCarrier( wns::node::Node* node, 
      const wns::Power& carrier, ValueOrigin origin);

  void storeInterference( wns::node::Node* node, 
      const wns::Power& interference, ValueOrigin origin);
  
  wns::Power 
  getAveragedCarrier( wns::node::Node* node ) const;
  
  wns::Power 
  getAveragedInterference( wns::node::Node* node ) const;
  
  wns::Power 
  getCarrierDeviation( wns::node::Node* node ) const;
  
  wns::Power 
  getInterferenceDeviation( wns::node::Node* node ) const;


The interference cache offers the interface shown in above. The
methods ``storeCarrier(...)`` and ``storeInterference(...)`` provide
write access to the interference cache while the other methods provide
read access. When adding measured values to the interference cache, it
is necessary to provide an origin indicator that indicates if the
access to the interference cache comes from a remote station or from a
local measurement. This gives the opportunity to average measured
values with different :math:`\alpha` values.
 


