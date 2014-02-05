=================
Context Collector
=================

Filling the context of a measurement by hand can be cumbersome. Especially, when you want
to collect context from different locations in your simulator. This is where the ``ContextCollector`` 
can help you. We will follow the example of the previous section and extend it to use
the ``ContextCollector``. You will see that the code for filling the context can be
organized in a way that makes the particular structure of a context reusable.

Concepts
--------

Context Provider
	A Context Provider can provide context for measurement when asked.

Context Provider Collection 	
	The name says it all. A collection of Context Providers.

Context Collector
	Collects context. Additionally it wraps a ProbeBus and takes
	care of fetching the ``context`` and ``timestamp`` for the call
	to ``forwardMeasurement``. You only need to provide the measurement
	value.

Simple use of the Context Collector
-----------------------------------

First of all we need to include the ContextCollector's header file.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.include.example

Ok. This was not so hard. We will leave the ``Job`` class unchanged, but derive it from ``wns::RefCountable``. This is needed because we will use ``wns::SmartPointer`` to do the memory management for all ``Job`` instances.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.job.example

We add a ``ContextCollector`` as private member to the ``Processor`` class. The ``ContextCollector`` simplifies collecting
measurements. It takes care of filling the context and also take care to pass the current simulation time to the underlying ``ProbeBus``.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.processor.example

The constructor of a the ``ContextCollector`` takes the name of the measurement source as a parameter. There is a more
complex constructor that is described in the second part of this chapter.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.constructor.example

If you want to publish a measurement you simply use the ``put`` method of the ``ContextCollector``. The first argument is the measurement value. If you want to provide some additional context you can pass a ``boost::tuple`` (see boosttuple_) as second parameter. Here we use ``boost::make_tuple`` to construct a tuple. The entries with odd indices (starting at 1) of the tuple are the keys and must always be of type ``std::string`` the even ones are the values and can either be of type ``int`` or ``std::string``.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.stopJob.example

If you compare this to the ``Processor::onJobEnded`` method of the last example, you can see that you basically need two lines of code where you previously needed around 10.

.. _boosttuple: http://www.boost.org/doc/libs/1_37_0/libs/tuple/doc/tuple_users_guide.html

Using and Implementing Context Providers
----------------------------------------

If you want to add context from other locations in your simulator without introducing thight coupling between the entity that provides the context and the entity that provides the measurement you should make use of ``ContextProviders``. A ``ContextProvider`` encapsulates the provisioning of context entries in a separate class. This section shows you how to implement a ``ContextProvider`` and add it to a ``ContextProviderCollection`` which defines the ``Context`` of a ``ContextCollector``. So lets get started. Again, we first need to include the ContextCollector's header file.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestCollector.include.example

Ok. This was not so hard. We will leave the ``Job`` class unchanged, but derive it from ``wns::osi::PDU`` instead of ``wns::RefCountable?``. openWNS is a system level simulation tool that focusses on protocol behaviour. The ``ContextCollector`` expects jobs to be PDUs.
So it this is a reasonable approach.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestCollector.job.example

We will make some extensions to the ``Processor`` class. First of all the signatures of ``startJob()`` 
and ``onJobEnded()`` now take a ``wns::SmartPtr<Job>`` as argument. This makes it more 
realistic, because normally a ``Job`` traverses many different classes in your simulator.
None of these classes knows when to delete the Job, therefore memory management is handled by 
a ``SmartPtr``.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestCollector.processor.example

You can see that there is a public method ``getID()`` and a private method ``generateID()`` as
well as a private member ``id_``.
We assume that there will be many processor in a simulation run which will be true for any non-trivial
simulation. Each processor has a unique ``processorID`` which you can get by using ``getID()``. During
construction of a processor a new unique processorID is automatically generated by ``generateID()``. Here
is the code for it:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestCollector.generateID.example

What we will do now is to create a context provider collection during startup of each processor.
Then a context collector will be initialized to use this collection. The collection will contain
two context providers. One that can read the ``priority`` from the ``Job`` that is evaluated, the other
will callback the processor to provide its own ``processorID``. Both context entries will then
be passed to the evaluation framework.

Let's start by implementing the ``PriorityProvider`` that reads the priority of a job. Here is
the class declaration:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestCollector.jobcp.example

We derive ``PriorityProvider`` from a template class ``wns::probe::bus::PDUContextProvider``. The 
template parameter is the ``Job`` class. This will take care of some basic conversion for us. On
construction we pass a ``key`` to the provider. This will be used when making entries in a context. The
``PDUContextProvider`` is an abstract class that forces us to implement the ``doVisit`` method.
This method is called whenever a ``ContextCollector`` needs to construct context for a measurement
value. It is rather simple:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestCollector.jobcpDoVisit.example

The Visitor Pattern is used here. The context is passed to each context provider as reference, so 
it can be written by each of them. The current job is also passed along, but unlike the context
it is passed in read-only mode. The only thing we do here is to insert the job's priority into
the context and give it the name ``key_``, which is set upon construction.

To provide the processorID we will use a generic purpose provider that can be used whenever
a simple property of some object is to be read. So basically now we have the context providers ready
and can start defining the context collector and its context provider collection. Let's have
a look at the constructor of ``Processor``.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestCollector.constructor.example

Here, the ``processingDelayCC_`` is constructed. This is the context collector for our processing delay.
Focus on the inner construction of the ``ContextCollector``. To ease the memory management we wrap that
object by a smart pointer ``ContextCollectorPtr``. There is no magic about that.

The constructor of a `ContextCollector`` takes a context provider collection as its first argument, and
the name of its measurement source a second argument. The context provider collection is constructed by
``getContextProviderCollection()``. Which is defined as:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestCollector.getcpc.example

First of all we construct an empty ``ContextProviderCollection`` named ``cpc``. We construct
one of our ``PriorityProvider``s and then use the general purpose ``Callback`` context provider.
This provider takes the name of the measurement source as first argument. The second argument
is any callable that returns an integer an takes zero arguments. We use ``boost::bind`` to
tell ``Callback`` that whenever it needs context information it should call the ``getID`` method
of ``this`` instance. Then we add both providers to the context provider collection and return it
to the caller. This finishes the setup of the context provider collection and the context collector.
How can it be used? Here is the the new code of ``onJobEnded()``.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.probe.bus.tests.DevelopersGuideTestCollector.stopJob.example

As you can see it became much shorter. We only calculate the processing time and
the call ``put``. The first argument is the job and the second the actual measurement.
The ``ContextCollector`` takes care that the job is passed to the ``PriorityProvider`` and
that the current ``processorID`` is retrieved.  We have sucessfully captured all the code 
to create context in the constructor of ``Processor``. The context provider collection
can be copied, passed around and reused and even hierarchically organized. This 
will be demonstrated in the next sections.

.. note::

   todo: Describe the usage of the ContextCollector within the Node/Component concept.
