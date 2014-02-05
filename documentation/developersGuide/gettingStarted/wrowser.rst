.. _gettingStartedWrowser:

----------------------
Installing the Wrowser
----------------------

The Wrowser (an acronym for Wireless network simulator Result brOWSER)
supports with a graphical interface the collection of results,
extraction of measurements and creation of parameter
plots. Furthermore, the Wrowser helps to create simulation campaigns
with large parameter spaces. The Wrowser is a project separated from
the openWNS and thus has to be installed independently.

.. important::
   The following steps are only required if you want to
   use the Wrowse to view simulation results. To run a single
   simulation and to evaluate the results using the text-output files,
   the Wrowser is not needed.


Prerequisites
-------------

Similar to the openWNS the Wrowser relies third party software. Each
of the listed libraries and programs below are freely
available.

 * python-qt4_ A comprehensive set of Python bindings for the Qt cross-platform GUI/XML/SQL C++ framework from Qt Software
 * python-qt4-dev Tools Development tools for python-qt4_
 * python-matplotlib_ A python 2D plotting library which produces publication quality figures in a variety of hardcopy formats and interactive environments across platforms.
 * python-tk_ Python's de-facto standard GUI (Graphical User Interface) package.
 * python-scipy_ Python library for mathematics, science, and engineering
 * pyqt4-dev-tools Various support tools for PyQt4 developers

.. _python-qt4: http://wiki.python.org/moin/PyQt
.. _python-matplotlib: http://matplotlib.sourceforge.net/
.. _python-tk: http://wiki.python.org/moin/TkInter
.. _python-scipy: http://www.scipy.org

Use the following command to install them in Ubuntu Linux:

.. code-block:: bash

    $ sudo apt-get install python-qt4 python-qt4-dev python-matplotlib python-tk python-scipy pyqt4-dev-tools

Installation
------------

Similar to the openWNS installation, the Wrowser can be obtained and
updated using Bazaar. The following command will checkout the current
version of the Wrowser into the directory 'wrowser'. Choose whatever
you want as name. Note that for the last step the root password is required.

.. code-block:: bash

   $ bzr branch lp:openwns-wrowser wrowser
   Branched 27 revision(s).
   $ cd wrowser
   $ sudo python setup.py install

You should now be able to start the Wrowser using

.. code-block:: bash

   $ wrowser

It will open an empty window, see :ref:`figure-gettingstarted-wrowser`.

.. _figure-gettingstarted-wrowser:

.. figure:: images/wrowser.*
   :align: center

   The initial, empty Wrowser window

Checking the playground.py wrowser-plugin 
--------------------------------------------

Go into your openWNS - directory created during the installation
of the openWNS and call playground.py. If everything went right, you
should see the new command ``preparecampaign``:

.. code-block:: bash

   $ ./playground.py
   [...]
   preparecampaign :  Prepare a simulation campaign.
   [...]

There will be an error message since the database is not set up yet. You can still use Wrowser in directory mode to view data from a single output directory. Proceed to the next step to install and setup the database.
