------------------------------
Writing Functional Units Tests
------------------------------

Setting up Functional Unit Networks
===================================

Whenever you need to test a functional unit you will need a test environment that provides a functional unit network
for your testee. This example shows you how to setup a simple one and is taken from the ``wns::ldk::crc::CRCTest``.
You will need to include some headers for this to work:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.ldk.crc.tests.CRCTest.headers.example

Now follows a walkthrough of the test setup of ``wns::ldk::crc::CRCTest``. The setup constructs a simple FUN which is shown in
the figure below.

.. graphviz::
  digraph overview {
  size="8,10"
  node [shape="box",fontsize="10"];
  upper [label = "upper : wns::ldk::tools::Stub"]
  testee [label = "testee"]
  lower [label = "upper : wns::ldk::tools::PERProviderStub"]
  upper -> testee -> lower
  }

First the following code is included
in the ``prepare`` or ``setUp`` method.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.ldk.crc.tests.CRCTest.prepare.example
   :linenos:

This creates a stub for the layer, which is needed to construct a functional unit network (cmp. line 4 and 5). Make ``layer``
a member of your test and declare it as type ``wns::ldk::ILayer*``.

.. warning::
   
   Do not forget to delete ``fun`` and ``layer`` in your ``tearDown`` or ``cleanup`` method. Otherwise
   you will get a memory leak which shows up if you check the tests for leaks.

In line 7 an empty ``wns::pyconfig::Parser`` is constructed, which is essentially an empty ``wns::pyconfig::View`` which
can be passed to your FU if it does not use config variables. Line 8 constructs the ``upper`` FU which is of 
type ``wns::ldk::tools::Stub``. It is used to inject compounds to the functional unit network and inspect the
incoming path. It does not require any configuration parameters

.. warning::
   
   Do not delete any functional units in the ``tearDown`` or ``cleanup`` method of your test. Once a functional unit
   is constructed the functional unit network is responsible for the memory of its functional units. If you delete
   the FU after test execution this will result in a double delete.

Now let's have a look at the ``setUpCRC`` method of this test suite.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.ldk.crc.tests.CRCTest.setupCRC.example
   :linenos:

In line 5 to 12 the configuration of the testee is prepared. If you need to provide a config use the ``loadString``
method of ``wns::pyconfig::Parser`` which interprets the given string as Python code. It is a good idea to use
the original PyConfig configuration class to setup your testee. In line 16 the CRC testee is constructed.

Now, the setup of the test environment is nearly complete. The CRC functional unit needs to access
to a command which provides the packet error rate. We need to include a functional unit that provides this
command in our test setup. The next chapter describes in detail how the lower FU of type ``PERProviderStub`` is
declared. To finish this setup let us have a look at the method ``setUpPERProvider``.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.ldk.crc.tests.CRCTest.setupPERProvider.example
   :linenos:


Line 1 to 11 us used to construct the PERProviderStub and its configuration. Then the FUs are connected according
to the desired test environment setup. Afterwards, all FUs must be added to the functional unit network. Finally,
the setup of the functional unit network is finalized by a call to ``onFUNCreated``.


Satisfying a Command Dependency of your Testee
==============================================

Suppose your testee needs to have a access to the command of another FU. The other FU however is quite complex
and has a lot of dependencies which makes it infeasible to include that FU directly in your test. What you
can do is create a stub for that FU that provides the proper command. Here is an example from the PERProviderStub.
The command is defined as:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.ldk.tools.PERProviderCommand.example

However, it can be any command of your choice. You do not need derive from ``EmptyCommand`` or ``ErrorRateProviderInterface``.
Now what you need to do is create a Stub FU that provides this command in a simple functional unit network of your
test. Within your test declare a class as follows:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.ldk.tools.PERProviderStub.example

It is a good idea to derive it from ``wns::ldk::tools::StubBase`` which provides control over accepting/wakeup functionality
and records received and sent compounds. If this is used you need to implement the calculateSizes member method in your stub. Here
is a minimal implementation.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.ldk.tools.PERProviderStub.calculateSizes.example

Sending Compounds
=================


Now that we have properly setup a test environment for a functional unit, we still need to test something.
Let's have a look at the ``testNoErrors`` test of the ``CRCTest``.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.ldk.crc.tests.CRCTest.testNoErrors.example
   :linenos:

First of all the test environment is setup. Remember that ``setUp`` or ``prepare`` is called by the testing framework.
After this we call ``setUpCRC`` and ``setUpPERProvder`` to complete the setup.

In line 7 we tell the functional unit network to create an empty compound and then we use the ``sendData`` method
of our upper FU to inject this compound. The upper FU will then try to pass it along the functional unit network
calling ``isAccepting`` and ``sendData`` appropriately. Within this testcase we expect that the compound we pass
to the network arrives at the lower FU. This is checked in line 9.

Then we use the ``onData`` method of the lower FU and pass on the compound in the incoming path. Here we also expect
the compound to arrive at the upper FU. We check this in line 13.

