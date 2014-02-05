=============================================
Mastering your SDK (a guide to playground.py)
=============================================

WNS consists of a number of sub-projects. Each of this project is
configured in ``config/projects.py``. The projects have different
purposes. Some of them contribute as module to WNS, while others
specify system tests. The system tests can be run to ensure the proper
operation of WNS. All these projects are managed via Bazaar (see:
http://www.bazaar-vcs.org). Since Bazaar has no built-in
support to control multiple projects, ``playground.py`` has been
developed to fulfill this task. Besides the management of the
different project trees this script also performs WNS-wide tasks like
compiling/installing all modules, building documentation and running tests as
will be shown below.

``playground.py`` offers a number of *commands* and according *modifiers*. 
A modifier modifies the way a command is executed (thus its
name). All commands and modifiers are listed by executing:

.. code-block:: bash

   $ ./playground.py --help

Compilation/Installation
------------------------

One of the most important features of playground.py installs a version
of WNS. You have probably used it right after check out. To install a
version of WNS in @c WNS/sandbox issue the following command:

.. code-block:: bash

   $ ./playground.py install --flavour=dbg

The string ``dbg`` tells ``playground.py`` to build the debugging version of
WNS. It can be found in ``WNS/sandbox/bin/dbg`` and the modules are
located under ``WNS/sandbox/lib/dbg``. Installation flavours available
are:
- **dbg** (for debugging)
- **opt** (optimized, for simulations, messages and asserts are disabled)
- **profOpt** (like opt but with profiling support for gprof)
- **callgrind** (to be used for CPU cycle profiling with valgrind)

There are also some modifiers available for the ``install`` command.

- **--flavour** Specify which build flavour to use (dbg, opt, ...)
- **-j/--jobs** Specify the number of compile jobs to be executed in
  parallel. Useful on multi-processor systems and when compiling
  distributed.
- **--static** All Modules will be linked into WNS
- **--scons** Forwards options (like no-filter or no-inf) to scons
- **--sandboxDir** Specify the location of your sandbox

Staying up-to-date
------------------

To upgrade the complete WNS including all sub-projects issue:

.. code-block:: bash

   $ ./playground.py upgrade


For each (sub-)project ``bzr pull`` will be called. Any newly
available patch in the repository will be applied. If you had
changes on one of the projects, it can happen that conflicts occur. @c
playground.py will stop and you should resolve the conflicts
first.

To have an overview in advance where new patches are available for
your WNS issue the following command:

.. code-block:: bash

   $ ./playground.py missing

To see where conflicts can happen you need to find out which projects
have local changes:

.. code-block:: bash

   $ ./playground.py status

If you additionally specify ``--diffs`` with ``status`` the differences
will be shown in detail for each project.

Cleaning up
-----------

From time to time it can be useful to clean up things (to remove old
object files, etc). The following command assist you in that:

.. code-block:: bash

   $ ./playground.py clean [objs, sandbox, docu, all]

The options have the following meaning:

- **objs**: remove the object files from the projects
- **sandbox**: remove the WNS installation (all libs, docu and binaries)
- **docu**: remove the directory ``doxydoc`` from each project
- **all**: all of the above

.. note::
   All items which are removed here can be easily recreated. No
   hand-written code will be deleted.

.. note::
   For the target ``objs`` you may need to additionally specify the
   modifier ``--flavour`` to remove the object file for a certain flavour
   (dbg, opt, ...)

Preparing Simulation Campaigns
------------------------------

Please see: @ref simulation

Creating Documentation
----------------------

Freshening this docu after an upgrade.

.. code-block:: bash

   $ ./playground.py docu

Running the test suite
----------------------
To run the entire test suite do

.. code-block:: bash

   $ ./playground.py runtests 

This command runs all the tests (unit tests and system tests) for you. Before committing
changes, you should *always* follow this step, to assure that your changes don't break anything.

While developing, you can also run tests individually. To run all unit tests enter:

.. code-block:: bash

   $ cd tests/unit/unitTests/
   $ ./wns-core -t 

To run a certain unit test, use the option ``-T`` and give the name of the test you want to run, e.g.:

.. code-block:: bash

   $ ./wns-core -t -T "rise::tests::BeamformingTest::simple"  

If you want to configure certain parameters that are not explicitely configured in the test itself,
you can add them either in the unittest config file (``tests/unit/unitTests/config.py``) or by executing

.. code-block:: bash

   $ ./wns-core -t -T "rise::tests::BeamformingTest" \
     -y "WNS.modules.rise.debug.antennas=True" \
     -y "WNS.masterLogger.enabled=True"


To run a certain system test, go to the system test directory and execute 

.. code-block:: bash

   $ cd tests/system/WiMAC-Tests--main--1.0/
   $ ./systemTest.py  

In order to run only one specific test suite, execute the wns-core 
with the specific config file for the test suite. Note that the reference
output will not be checked automatically!

.. code-block:: bash

   $ cd tests/system/WiMAC-Tests--main--1.0
   $ ./wns-core ./wns-core -f configSDMA.py  

Customize the SDK contents (projects.py)
----------------------------------------

The openWNS SDK reads information on which modules to include from the file ``config/projects.py`` . This file is a plain Python file and can be edited. The structure of this file is

.. code-block:: python

   # Header
   from wnsbase.playground.Project import *
   import wnsbase.RCS as RCS

   bzrBaseURL = "bzr://bazaar.comnets.rwth-aachen.de/openWNS/main"

   # MODULE DEFINITIONS

   # MODULE LIST

   # PROCESSING INSTRUCTIONS


The module definition for the openwns executable looks like this:

.. code-block:: python

   wns_core     = Binary('./framework/wns-core--main--1.0',
   		         "wns-core--main--1.0", bzrBaseUrl,
                         RCS.Bazaar('./framework/wns-core--main--1.0',
                                    'wns-core', 'main', '1.0'),
                         [libwns, ])


You create an instance of a project (see wnsbase.playground.Project). The most commonly used is Library, Generic or Python.
Here we use Binary to tell the build-system to build an executable file. The first parameter is the path within the SDK where
the project will be stored (``./framework/wns-core--main--1.0``). The second parameter tells playground where the 
remote branch is located. The string is append to ``bzrBaseUrl`` and is passed on to ``bzr`` if needed. You then pass in an
instance of RCS. Bazaar where you need to repeat the path in the SDK and give three more parameters modulename, series 
name and version. Last but not least you need to tell playground.py if your project depends on another project within the SDK.
Here ``wns_core`` depends on ``libwns``. Note that ``libwns`` is an instance of ``wnsbase.playground.Project.Library``.

After all module definitions the ``projects.py`` file contains a list ``all`` which includes all projects to be used.
Make sure your project is included there. The ``prereqCommands`` is a list of ``command directory`` tuples. If you 
need some special setup commands you can place them here. ``playground.py`` executes the command in the specified directory
during setup.
