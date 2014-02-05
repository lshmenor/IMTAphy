.. _gettingStartedPrerequisites:

-------------
Prerequisites
-------------

openWNS relies on a number of third party software. Each of the listed
libraries and programs below are freely available. openWNS is built
entirely around free software. Some of the software is optional.


Third party libraries
---------------------

 * CppUnit_ (>=1.10) The basic unit testing framework
 * Python_ (>=2.5) Used by almost everything starting from the build framework to configuration
 
   - Numpy Python package used for channel models implemented in Python
 * Boost_ Mainly used to have the TR1 implementations of the upcoming C++ standard available
 
   - Besides basic Boost files Boost libraries Signals, Filesystem, Date-Time and Program-Options are required

.. _CppUnit: http://cppunit.sourceforge.net/
.. _Python: http://www.python.org
.. _Boost: http://www.boost.org/

Build framework
---------------

 * Bazaar_ The Revision Control System
 * SCons_ (>=0.96) A make replacement
 * GCC_ (>=3.4) Compiler Suite, due to usage of special language features regarding templates the versions below 3.4 do not work

.. _Bazaar: http://bazaar-vcs.org/
.. _SCons: http://www.scons.org
.. _GCC: http://gcc.gnu.org/

Optional for build framework
----------------------------

 * Doxygen_ Documentation is generated with this tool
 * Graphviz_ Used by doxygen to create nice UML diagrams
 * Icecream_ Allows for distributed compiling

.. _Doxygen: http://www.doxygen.org/
.. _Graphviz: http://www.graphviz.org/
.. _Icecream: http://wiki.kde.org/icecream

Ubuntu Linux
------------

Ubuntu Linux 10.04 is currently the default development and testing platform for openWNS. We try to update to 12.04 ASAP. All required packages can ge easily downloaded using the command:

.. code-block:: bash

    $ sudo apt-get install build-essential scons libboost-dev libboost-program-options-dev libboost-date-time-dev libboost-filesystem-dev libboost-signals-dev bzr libcppunit-dev python2.6-dev python-numpy

use the following line on Ubuntu 12.04 to get all required packages:

.. code-block:: bash

    $ sudo apt-get install build-essential scons libboost-dev libboost-program-options-dev libboost-date-time-dev libboost-filesystem-dev libboost-signals-dev bzr libcppunit-dev python-dev python-numpy

if you are using version 8.04 (Hardy) of Ubuntu Linux you have to type:

.. code-block:: bash

    $ sudo apt-get install build-essential scons libboost-dev libboost-program-options-dev libboost-date-time-dev libboost-filesystem-dev libboost-signals-dev bzr libcppunit-dev python2.5-dev python-numpy

You may enounter problems with Bazaar not beeing able to access the repository. Follow the instructions here to install the newest, not officially supported, version of Bazaar for Ubuntu Linux 8.04 (Hardy): https://launchpad.net/~bzr/+archive/ppa. 

