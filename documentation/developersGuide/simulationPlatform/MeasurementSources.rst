
.. index::
   single: measurements; measurement source
   single: measurement source

Measurement Sources
===================


Introduction
------------

openWNS decouples the generation and evaluation of
measurements. The evaluation, may that be logging, filtering, sorting
or simply storing them is described in detail in
:ref:`simulationPlatformEvaluationFramework`. 
In the following, the focus is solely on the generation of
measurements and adding context information to them, which than may be
processed by the evaluation framework.

Concept
-------

In the following we will be making use of some fundamental conecepts
regarding the implementation of measurement sources, which will be
defined now.

Measurement Source
    A measurement source is a logical concept that describes a
    location within the simulator where a measurement value is measured and is
    made available to the evaluation framework. A measurement value
    has a timestamp, which holds the time when the measurement value
    was generated. A  measurement value may have context information.

Measurement Value
    A ``double`` value representing a measurement of some phenomenon
    in your simulator

Context
    A dictionary mapping an arbitrary but unique name to an ``int``
    value. The context captures the state (within a certain scope) of
    your simulator when a measurement value is measured.

Realization
-----------

Measurement sources, values and their context information is realized by
``wns::probe::bus::ProbeBus``. The ``ProbeBus`` realizes the
connection of measurement sources to the evaluation
framework. ``wns::probe::bus::Context`` realizes the context
information.

Commonly, the entries in the context information of each measurement
source do not change during the whole simulation (of course the value
of each entry changes over time, but not the name). In such cases the
``wns::probe::bus::ContextCollector`` should be used to
automatically generate the context information and the timestamp when
a measurement souce generates a measurement value and passes it on to
the evaluation framework. The
``wns::probe::bus::ContextProviderCollection`` and
``wns::probe::bus::ContextProvider`` are used to define which context
information is to be gathered by the ``wns::probe::bus::ContextCollector``.

Implementing a Measurement Source
---------------------------------

To add a measurement source to some class of your simulator you need
to add

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTest.include.example

to the header file of that class. Suppose you have implemented a
simple queueing model with a ``Job`` class that has some priority and
that records the time when processing started. It may look like this:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTest.job.example

Furthermore, let us say that you have a class ``Processor`` that will
process a ``Job`` and then generate a measurement of the processing
time. The context of this measurement will be the ``Job``'s priority
class. The ``Processor`` class may look like this:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTest.processor.example
 
The class has two methods ``startJob()`` and ``onJobEnded``. It has 
a distribution member variable to draw randomly distributed processing
delays. To generate measurements we need a pointer to a
``wns::probe::bus::ProbeBus``. This is your connection point to the
evaluation framework. Let's have a look at the constructor of
``Processor``.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTest.constructor.example

To initialize our ``ProbeBus`` we ask the
``wns::probe::bus::ProbeBusRegistry`` to create one for us and
register it with a unique name. Since we want to measure processing
delay we choose the name ``processor.processingDelay``. You should
always prefix your measurement source name with the namespace of your
class to make it unique. You will see in
:ref:`simulationPlatformEvaluationFramework` that this name is used
to connect your evaluation to the measurement source.

Let's see what happens if someone request the processor to start a
job. Here is ``startJob``:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTest.startJob.example

We get the event scheduler and record the current time in the
``job.startedAt_``. Then we draw a random processing time form the distribution
and schedule a delayed call to our the ``onJobEnded`` method. If you
do not understand how the scheduling works please read
:ref:`simulationPlatformEventScheduler` first.

So let's see how to generate the processing delay measurements. This
is done int ``onJobEnded``.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTest.stopJob.example

Since we want to provide context information for our measurement value
we first create an empty ``wns::probe::bus::Context`` and populate it
with the job's priority which we read from the ``Job`` itself. Then we
get the current time from the scheduler and calculate the measurement
value, which is simply the current time minus the job's start time,
i.e. the processing delay.

Everything is prepared now. We have the *measurement value*, the
*timestamp* and the *context*. To pass it on to the evaluation
framework, we first assure that ``probeBus_`` is not ``NULL`` and then
call ``forwardMeasurement`` of our ``probeBus_``.

