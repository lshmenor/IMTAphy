===========
Preparation
===========

In the following, it is assumed that the openWNS source code is
situated in the directory ``myOpenWNS``.

Make sure that the following points are met within your openWNS
installation:

* Running ``./playground.py`` from ``myOpenWNS`` should give no error or
  warning message, but the standard help output.

* The installation of both the debug as well as the opt version of
  openWNS, from the source code to the ``myOpenWNS/sandbox`` directory,
  works without problems. Installation is started with the commands

  .. code-block:: bash

     $ ./playground.py install
     $ ./playground.py install  --flavour=opt

* All system and unit tests should run without errors. Tests are
  started with the command

  .. code-block:: bash

     $ ./playground.py runtests

* The command to create simulation campaigns, ``preparecampaign``, is
  available when running ``./playground.py``.

* The database to store simulation results is installed according to
  :ref:`gettingStartedPostgreSQL`.

If all these steps succeed, you can create a new simulation campaign
which is used for all the following experiments. A simulation campaign
consists of

#. A sub-directory ``sandbox`` that contains all necessary libraries
   and the openwns executable. After creation of the campaign, the
   directory will be changed to read-only mode in order to avoid
   changes to its content.

#. One or more directories to store the simulation configuration(s)
   and the results in. When the simulation campaign is initially
   created, this directory is named ``simulations``.

#. Optinally, a set of tables in the simulation database to store
   * The parameters used to create the simulations and
   * The results of each simulation
   By combining the simulation parameters with the results, it becomes
   possible to data-mine the effects of different parameters to the
   measurements. We will use in the experiments this methodology,
   supported by the help of the openWNS Wrowser, which allows
   aggregation, slicing and dicing of simulation results together with
   the plotting of presentation-quality graphs.

To create the simulation campaign, use the ``preparecampaign``-command
in the following way:

.. code-block:: bash

   $ ./playground.py preparecampaign ../myFirstCampaign
   Preparing simulation campaign. Please wait...

.. note::

  The ``preparecampaign`` command will only be available if you have
  properly installed Wrowser and its plugins for playground.py. Please
  refer to :ref:`gettingStartedWrowser`.

This will create the directory ``../myFirstCampaign``, including the
neccessary sub-directories. After some moments, the script requests
the following user input:

#. ``Please enter the name of the directory the simulations shall be
   stored in:``

   For each experiment, we suggest to use a different simulation
   directory (or "sub-campaign"). In this way, all experiments use the
   same version of the openWNS, but have different configuration
   files. Hence, the first experiment should get the name
   ``experiment1``.

#. ``Please enter a name for the campaign:``

   This name will be used to identify and select the campaign in the
   openWNS Wrowser.

#. ``Please enter a short description of the campaign:``

   This description will also be shown in the openWNS Wrowser.

Then, the installation procedure will start. For the other
experiments, it is suggested to create sub-campaigns. To do this, call
the ``preparecampaign`` command again with a directory of an existing
campaign:

.. code-block:: bash

   $ ./playground.py preparecampaign ../myFirstCampaign

the script will ask (after the database-question):

.. code-block:: bash

   Found simulation campaign in directory /home/wns/myFirstCampaign.
   Shall I try to (U)pdate the sandbox or do you want to (C)reate a new sub campaign?
       Type 'e' to exit (u/c/e) [e]:

Use ``c`` to create the sub-campaign and select a useful name for the
directory.
