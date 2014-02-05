###################################################
Experiment 2 : Cell Spectral Efficiency Calibration
###################################################

.. _WINNERPLUS: http://projects.celtic-initiative.org/winner+/WINNER+%20Evaluation%20Group.html

In this experiment you learn how to setup the openWNS LTE module to obtain results for the baseline LTE-R8 configuration as used for IMT-Advanced calibration by the 3GPP and the WINNERPLUS_ evaluation group. The motivation behind this base line scenario is to agree on default values for LTE functionalities not fully described by the standard. This mainly applies to the many degrees of freedom in the scheduler. Furthermore, we will now be using the database scripting API to extract user throughput simulation results for uplink and downlink direction and compare these with calibration results obtained by other partners of the WINNERPLUS_ evaluation group. The results we compare against are shown below

.. figure:: images/LTER8RefUL.*
   :align: center

.. figure:: images/LTER8RefDL.*
   :align: center

These figures show uplink and downlink user throughput distributions for the indoor hotspot (InH) scenario for three different partners of the the WINNERPLUS_ evaluation group. The openWNS complies with all five IMT-Advanced scenarios, but for simplicity we will only process the InH scenario in this tutorial.



-------------------------------
Calibration Scenario
-------------------------------
3GPP and WINNERPLUS_ partners agreed on LTE-R8 simulator configurations that go beyond the specfications given in the ITU-R IMT-A Evaluation guidelines. The baseline calibration setup re-uses the cell spectral efficiency setup as specified by the ITU-R and deploys an LTE Release 8 system operating in FDD mode. The bandwidth for each Uplink and Downlink is 20MHz for InH and 10MHz for the other scenarios. A discussion of other parameters will be discussed in this section.

*******************************
Scenario Setup
*******************************

Scenario viewer should already be known. Describe ``channelModelCreator`` and random seed setup for multiple drops.

******************
Downlink Scheduler
******************
Downlink scheduling is Round Robin for all UEs and during each subframe the full bandwidth should be allocated to on UE.

.. note::
   In openWNS use the ``Exhaustive Round Robin`` scheduling strategy with any DSA strategy.

****************
Uplink Scheduler
****************

Uplink scheduling is FDMA. Each user is assigned an equal sharw of resource blocks every subframe.

.. note::
   In openWNS use ``DSA Driven Round Robin`` scheduling strategy with ``Fixed`` DSA strategy.


*******************************
Uplink Power Control
*******************************

The more power a station emits the more interference it will cause to other cells. At the same time more transmission power increases the received signal power in the own cell and therefore the SINR. The LTE module implements the open-loop fractional pathloss compensation power control behaviour as specified by [3GPP36.213]_
The formula used by user terminals to calculate its transmission power in uplink direction is

.. math:: P_{TX} = P_0 + \alpha PL

where both the base level :math:`P_0` and the fractional path-loss compensation factor :math:`\alpha` are broadcasted by the eNB. User terminals measure the path-loss :math:`PL` to the serving cell utilizing the downlink reference signals and a proper time-averaging function to determine the transmission power. 

The dynamic offset mechanisms as specified in [3GPP36.213]_ are not implemented.

.. note::
   For the calibration use :math:`P_0 = -106dBm` and :math:`\alpha = 1.0`.

.. [3GPP36.213] 3GPP Technical Specification 36.213, 'Evolved Universal Tertestial Radio Access (E-UTRA); Physical Layer Procedures (Release 8)', www.3gpp.org



*******************************
Small scale fading
*******************************

Describe ``FTFading`` setup.

-----
Tasks
-----

************************
Task 1: Verify the Scheduling Result
************************
Use CouchDB channel trace to verify resource assignement

************************
Task 2: Run Calibration Campaign
************************
#. Setup a campaign based on ``myOpenWNS/tests/system/lte-tests/PyConfig/config.py``. 
#. Create the ``campaignConfiguration.py`` including two parameters:

   * 40 random seeds 
   * :math:`\alpha=1.0` and :math:`\alpha=0.8`

#. Adapt ``config.py`` to include your settings
#. Queue the simulations.
#. View results with wrowser. Use the aggregation functionality to generate average curves for the random drops.
#. Compare with reference results from others

*******************************
Task 3: Script the Calibration Figures
*******************************

We will now use the sripting API of openWNS wrowser to retrieve the uplink and downlink user throughput distributions and plot these along with the results of other partners. We will also be using Pylab_ as a free Python substitute for MATLAB. Go to ``myOpenWNS/tests/system/lte-tests/PyConfig/`` and take a look at ``compareToReference``. This script reproduces the figures given at the very beginning of this LTE tutorial. Now extend the script to fetch your result (for :math:`\alpha=1`) from Task 1 and plot them along with the results of the other calibration results. First look at the section on the wrowser API below.

.. note:: Take care that you do not pass all scenarios when aggregating the PDF. Otherwise you will end up with an averaged curve not only for all random seeds but also for all values of :math:`\alpha`.

.. _PyLab: http://www.scipy.org/PyLab


Using the openWNS Wrowser API
-----------------------------

The API offers methods to open campaign databases and access the parameters and results contained within. The API does not offer more functionality than openWNS wrowser does, but it gives you the power to write your own scripts to analyze the data, combine it with other data or simply automate recurring every day tasks. The methods offered by the API are listed in the table below.

+---------------------------------------------------------+-----------------+-----------------------------------+ 
| Method                                                  | Return Value    | Purpose                           |
+=========================================================+=================+===================================+ 
| getCampaigns()                                          | [Campaign]      | Retrieves all of your             |
|                                                         |                 | campaigns                         |
+---------------------------------------------------------+-----------------+-----------------------------------+
| getCampaignByTitle(name)                                | Campaign        | Retrieve a campaign by            |
|                                                         |                 | its title                         |
+---------------------------------------------------------+-----------------+-----------------------------------+
| getScenariosForCampaign(campaign)                       | [Scenario]      | For a given scenario retrieve     |
|                                                         |                 | all scenarios (includes the       |      
|                                                         |                 | parameters                        |
+---------------------------------------------------------+-----------------+-----------------------------------+      
| getParametersOfCampaign(campaign)                       | [string]        | The list of campaign parameters   |
+---------------------------------------------------------+-----------------+-----------------------------------+
| getParameterSet(campaign, scenario)                     | ParameterSet    | Parameter settings for            |
|                                                         |                 | one scenario                      |
+---------------------------------------------------------+-----------------+-----------------------------------+
| getResultsInfo(campaign)                                | ResultsInfo     | Which Probes are available        |
+---------------------------------------------------------+-----------------+-----------------------------------+
| getPDFs(name, campaign, forScenarios=None, agg=None)    | [(xvec,yvec)]   | Get the PDF for probe ``name``    |
|                                                         |                 | of ``campaign`` for all scenarios |
|                                                         |                 | passed via the parameter          |
|                                                         |                 | ``forScenarios``.                 |
|                                                         |                 | ``agg`` can be any SQL aggregation|
|                                                         |                 | function such as "AVG" or "SUM"   |
+---------------------------------------------------------+-----------------+-----------------------------------+
| getCDFs(name, campaign, forScenarios=None, agg=None)    | [(x,y)]         | Get a CDF                         |
+---------------------------------------------------------+-----------------+-----------------------------------+
| getCCDFs(name, campaign, forScenarios=None, agg=None)   | [(x,y)]         | Get a CCDF                        |
+---------------------------------------------------------+-----------------+-----------------------------------+
| query(sql, campaign=None)                               | SQL cursor      | Execute a custom query (expert)   |
+---------------------------------------------------------+-----------------+-----------------------------------+

To play around with the API, fire up a python shell and simply type in the commands you want to try out. Below is a sample session that uses the most important methods of the API.

.. code-block:: python

   bash # python
   Python 2.6.5 (r265:79063, Apr 16 2010, 13:09:56) 
   [GCC 4.4.3] on linux2
   Type "help", "copyright", "credits" or "license" for more information.
   >>> import openwns.wrowser.simdb.api as api
   >>> c = api.getCampaignByTitle("lteR8RC1Calib")
   >>> c
   <openwns.wrowser.simdb.api.campaigns.Campaign instance at 0xb759344c>
   >>> dir(c)
   ['__doc__', '__init__', '__module__', 'authorized', 'campaignID', 'dbSize', 'description', 'title']
   >>> c.dbSize
   '8336 kB'
   >>> api.getParametersOfCampaign(campaign)
   ['randomNmbr', 'uepercell']
   >>> for s in api.getScenariosForCampaign(campaign):
         print s.parameterSet.params
    
   {'randomNmbr': 0, 'uepercell': 10}
   {'randomNmbr': 1, 'uepercell': 10}
   {'randomNmbr': 2, 'uepercell': 10}
   {'randomNmbr': 3, 'uepercell': 10}
   {'randomNmbr': 4, 'uepercell': 10}
   {'randomNmbr': 5, 'uepercell': 10}
   {'randomNmbr': 6, 'uepercell': 10}
   {'randomNmbr': 7, 'uepercell': 10}
   {'randomNmbr': 8, 'uepercell': 10}
   {'randomNmbr': 9, 'uepercell': 10}
   {'randomNmbr': 10, 'uepercell': 10}
   {'randomNmbr': 11, 'uepercell': 10}
   {'randomNmbr': 12, 'uepercell': 10}
   {'randomNmbr': 13, 'uepercell': 10}
   {'randomNmbr': 14, 'uepercell': 10}
   {'randomNmbr': 15, 'uepercell': 10}
   {'randomNmbr': 16, 'uepercell': 10}
   {'randomNmbr': 17, 'uepercell': 10}
   {'randomNmbr': 18, 'uepercell': 10}
   {'randomNmbr': 19, 'uepercell': 10}

   >>> api.getResultsInfo(campaign).pdfProbes
   ['lte.effSINR_Downlink_DecodeFailure_PDF', 'lte.effSINR_Downlink_DecodeSuccess_PDF', 
    'lte.effSINR_Downlink_PDF', 'lte.effSINR_Uplink_DecodeFailure_PDF', 'lte.effSINR_Uplink_DecodeSuccess_PDF',
    'lte.effSINR_Uplink_PDF', 'lte.IoT_DL_CenterCell_PDF', 'lte.IoT_UL_CenterCell_PDF',
    'lte.PhyMode_DL_CenterCell_PDF', 'lte.PhyMode_UL_CenterCell_PDF',
    'lte.schedulerTXSegmentOverhead_DL_CenterCell_PDF', 'lte.schedulerTXSegmentOverhead_UL_CenterCell_PDF',
    'lte.SINR_DL_CenterCell_PDF', 'lte.SINRest_DL_CenterCell_PDF', 'lte.SINRestError_DL_CenterCell_PDF',
    'lte.SINRestError_UL_CenterCell_PDF', 'lte.SINRest_UL_CenterCell_PDF', 'lte.SINR_UL_CenterCell_PDF',
    'lte.top.packet.incoming.delay_BS_PDF', 'lte.top.packet.incoming.delay_UE_PDF',
    'lte.top.packet.outgoing.delay_BS_PDF', 'lte.top.packet.outgoing.delay_UE_PDF',
    'lte.top.total.window.aggregated.bitThroughput_BS_PDF', 'lte.top.total.window.aggregated.bitThroughput_UE_PDF',
    'lte.top.total.window.incoming.bitThroughput_BS_PDF', 'lte.top.total.window.incoming.bitThroughput_UE_PDF',
    'lte.top.total.window.outgoing.bitThroughput_BS_PDF', 'lte.top.total.window.outgoing.bitThroughput_UE_PDF',
    'lte.TxPower_DL_CenterCell_PDF', 'lte.TxPower_UL_CenterCell_PDF']
   >>> api.getCDFs("winprost.SINR_UL_CenterCell_PDF", campaign)
   [([-20.0, -19.5, . . ., 28.5, 29.0, 29.5, 30.0], [0.0, 0.0, . . ., 1.0, 1.0, 1.0, 1.0]),
    ([-20.0, -19.5, . . ., 28.5, 29.0, 29.5, 30.0], [0.0, 0.0, . . ., 1.0, 1.0, 1.0, 1.0]),]
    # One xvec,yvec pair per scenario
    >>> api.getCDFs("winprost.SINR_UL_CenterCell_PDF", campaign, agg="AVG")
    [([-20.0, -19.5, . . ., 28.5, 29.0, 29.5, 30.0], [0.0, 0.0, . . ., 1.0, 1.0, 1.0, 1.0])]
    # Excatly on xvec, yvec pair containing the average yvalue per bin
    >>> from pylab import *
    >>> curve = api.getCDFs("winprost.SINR_UL_CenterCell_PDF", campaign, agg="AVG")
    >>> plot(curve[0][0], curve[0][1])
    [<matplotlib.lines.Line2D object at 0x936760c>]
    >>> show()

