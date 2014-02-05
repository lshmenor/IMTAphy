------------
Installation
------------

After you have successfully downloaded openWNS according to :ref:`gettingStartedDownload` you are ready to
install openWNS. Make sure you satisfy all prerequisites in :ref:`gettingStartedPrerequisites`.

The installation itself is quite easy:

.. code-block:: bash

  $ ./playground.py install --flavour=dbg

After building has finished you can find the respective modules in
``./sandbox/dbg/``.

Under ``./sandbox/dbg/bin/`` you should find a binary which is called
``openwns``.

Test the Installation
---------------------

openWNS supports the testing of developed code using both unit and
system tests. To test the installation, you can first run the unit
tests. Change to the directory

.. code-block:: bash

  $ cd tests/unit/unitTests

and say

.. code-block:: bash

  $ ./openwns -t -v
  Loading...
  Library: ofdmaphy
  Module: ofdmaphy
  [...]
  [TST] wns::fsm::tests::FSMTest::multipleStateCreations                                          [OK]   0.017366 s

  OK (1052 tests)

``-t`` runs all available unit tests and ``-v`` puts ``openwns`` into verbose mode
(``--help`` shows all available options).

For a more extensive testing of all installed modules the system tests
can be used. System tests are made of a configuration file of the
openWNS which sets up a simple simulation scenario of few nodes, each
one equipped with certain modules, e.g. with a Layer 2 according to
IEEE 802.11. Additionally, a system test contains reference output
which is compared with the actual output of the simulation.

To run all system tests, you first have to compile the ``opt`` version using ``playground.py``:

.. code-block:: bash

  $ ./playground.py install --flavour=opt

After this, you can start all system test with

.. code-block:: bash

  $ ./playground.py runtests
  Starting test suites ...
  NOTE: you may see slow progress since the tests run simulations

  **********************************************************************
  SystemTestSuite: ...x/source/cleanWNS/tests/system/ip-Tests--main--1.0
  Configuration: config.py
  Description: A ring of three subnets to test IP
  ----------------------------------------------------------------------
  Preparation phase:
  Running simulations (no test, just preparing output) in debugging and
  optimized mode (may take very long):
  [...]
  Ran 1161 tests in 490.323s

  OK

If one of the tests did not end up with ``OK``, something went very
wrong during the installation. Please check if all previous steps were
successfull.

