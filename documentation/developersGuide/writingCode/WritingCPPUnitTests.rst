======================
Writing C++ Unit Tests
======================

WNS uses cppunit for unit testing. The full manual is available at
http://cppunit.sourceforge.net/. Here, you will be shown the basics of
writing unit tests in openWNS to get you started quickly.

Quickstart
----------

Let's have a look at a very simple unit test included in the openwns-library.
``StopWatchTest.hpp`` is the header file of this unit test.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.tests.StopWatchTest.example
   :linenos:

In line 1 we include the WNS TestFixture. The new class that implements our
tests inherits from ``wns::TestFixture`` (cmp. line 11). You must use some
cppunit macros to declare your test suite (``TEST_SUITE``, ``TEST_SUITE_END``) and 
each test method (``TEST``). This is done in line 13-16.

If you need initialization or cleanup code you can place these in ``prepare()``
or ``cleanup()``. ``prepare()`` is execute everytime *before* the test execution is passed to one
of your test methods. ``cleanup()`` is executed everytime when test execution is returened
to the test framework *after* your test method. If you have multiple test methods
``prepare()`` and ``cleanup()`` are called multiple times. This ensures that each test method sees
the same clean environment when it starts. Individual tests do not have side effects on
each other.

After that your can implement your test methods. These should be prefixed with ``test``.
Within your test you exercise your system under test and define your expectations
by using the ``ASSERT`` macros. 

.. note::

   Include hyperlink to ASSERT overviews.

The most important line, which is often forgotten is line 56. ``TEST_SUITE_NAMED_REGISTRATION``
registers your test fixture with the test framework. If your forget this line your tests
will not be executed. The first parameter is your test class, and the second defines
to which test suite you register. 

Why to use the openWNS Testfixure
---------------------------------

Many units which are under test in WNS need an 
EventScheduler or RandomNumberGenerator to work. These elements (EventScheduler, RNG)
are Singletons within the simulation, e.g. because the EventScheduler
needs to be accessible from anywhere. Hence, the EventScheduler must
be reset to be in a (defined) pristine state (no events pending,
etc.) before a test is run. Similar things hold for the
RandomNumberGenerator. 

You should always use the @c wns::TestFixture in openWNS unit tests. It takes care of the
proper initialization of the simulation platform and properly cleans up afterwards. 
``wns::TestFixture`` is derived from ``CppUnit::TestFixture`` and implements ``setUp()`` and ``tearDown()``.

To give the test writer the ability to prepare
and cleanup his test too, the ``wns::TestFixture`` provides two methods:
``prepare()`` and ``cleanup()``. The openWNS Testfixture implements the ``setUp()`` and ``tearDown()``
methods and takes care of initialization and resetting of simulator singletons.
You can prepare and cleanup  your test environment by implementing  the ``prepare()`` and
``cleanup()`` methods.

Commonly used ASSERT Macros
---------------------------

The most simple assertion is to test if a boolean @em condition is true. If the @em condition
is false during test execution the containing test fails and will be reported by the test
environment.

.. code-block:: c++

   CPPUNIT_ASSERT( boolean_condition)


Often you need to test if the **output** of your system under test meets your **expectation**, i.e.
if the output value is equal to your expected value. You should use ``CPPUNIT_ASSERT_EQUAL``
for this. You could also use the simple ``CPPUNIT_ASSERT`` and include a comparison statement. However,
the use of the ``CPPUNIT_ASSERT_EQUAL`` macro has the benefit that it includes the actual and
expected value when reporting an error. You should **always** prefer ``CPPUNIT_ASSERT_EQUAL`` over
``CPPUNIT_ASSERT`` if you compare two values.

.. code-block:: c++

   CPPUNIT_ASSERT_EQUAL( output  , expectation )

Testing equality of ``double`` values is not reliable due to the floating point representaion. You
should test equality of two double values *output* and *expectation* by testing if :math:`|output-expectation|<maxAbsoluteError`

.. code-block:: c++

   CPPUNIT_ASSERT_DOUBLES_EQUAL( output, expectation, maxAbsoluteError )

If you want to test for the relative error :math:`\left|\frac{output-expectation}{expectation}\right|<maxRelativeError`
you can use an openWNS extension of this macro

.. code-block:: c++

   WNS_ASSERT_MAX_REL_ERROR( output, expectation, maxRelativeError )

.. note::

   todo: Describe ``WNS_ASSERT_ASSURE_EXCEPTION``, ``WNS_ASSERT_ASSURE_NOT_NULL_EXCEPTION``, ``CPPUNIT_ASSERT_MESSAGE``

.. code-block:: c++

   CPPUNIT_ASSERT_MESSAGE( functionCall.getName(),
   			   functionCall.getName() == "wns::Backtrace::snapshot()");

Asserting that a certain Exception is thrown
--------------------------------------------

Taken from wns::AssureTest

.. code-block:: c++

   CPPUNIT_TEST_EXCEPTION( except, Assure::Exception );

Take from container/tests/RegistryTest.cpp:

.. code-block:: c++

   CPPUNIT_ASSERT_THROW( r.update("foo", foo), AReg::UnknownKeyValue );

.. note::
   
   todo: describe how to test that a certain exception is thrown

Controlling the Event Scheduler
-------------------------------

When testing units that are part of an event driven simulation you somehow need to test if the
behaviour in time of your system under test is correct. You need to have control over the progress
of time in your test.

If the simulator executable is run in testing mode the event scheduler is initialized but the main
eventloop is not started. The control on time progress is in the hands of the test developer. If
you derive from ``wns::TestFixture`` the event scheduler is reset before entering your ``prepare()`` method.

In a test you would normally use the following code to control the event scheduler:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.unittest.process.example

This tells the scheduler to execute exactly one event.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.unittest.gettime.example

This gives you the current time. You can use this to check if timeouts have been set correctly.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.unittest.reset.example

Resets the event scheduler. You probably won't use that, because @wns::TestFixture already executes
this for you. However, you can always clear the event queue with this call.
