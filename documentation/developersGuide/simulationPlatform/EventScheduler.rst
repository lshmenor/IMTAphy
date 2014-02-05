.. highlight:: c++

.. index:: scheduler, event scheduler, events

.. _simulationPlatformEventScheduler:

The Event Scheduler
===================

With the openWNS event scheduler you can schedule anything that is callable
in your simulation. A callable can be a function pointer, a function object
or anything else that implements the call operator, requires no arguments and
has a return value of void. This page shows you how to use boost::bind to
create such callables in a uniform way and use the resulting function
objects with the scheduler.

Scheduling free functions
-------------------------
We will start with a very simple example. Suppose there exists a free
function freeFunction within your simulation and you want to schedule calls
to this function at later points in time.  The example function we use here
looks like this:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.freeFunction.example

Every time it is called it sets a global variable to the value 101. To
schedule a call to this function at some later point in (simulation) time
you could simply do:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.bindFreeFunction.example

This tells the scheduler to call the function freeFunction after 10 seconds
of simulation time have passed. The argument you pass to the scheduleDelay
function is a Callable that carries a pointer to freeFunction and the delay
after which the call to this function should be made. This is quite simple if you deal with
free functions, i.e. functions that do not belong to a class and therefore
do not have a context. If you want to schedule calls to member functions
(which you probably want to do frequently within an object oriented simulator)
you need to be able to tell the scheduler on which object the scheduler should
make the call to a member function, you want to bind the function pointer to
an instance of an object. This is when you want to use boost::bind. Let us
just stick with the last example for a short moment and see how you could use
boost::bind to create a Callable for freeFunction.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.boostBindFreeFunction.example

Pretty simple to do that. Simply call boost::bind() and pass the function
pointer as the argument. This creates an object that is compatible with
wns::events::scheduler::Callable. However, this does not have any immediate
benefit for us. We will see later that by using boost::bind we get a
consistent syntax for all Callables which greatly improves readability.

Scheduling Member Functions
---------------------------
So let us have a look at member functions now. Suppose you have the following
class:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.classWithCallabck.example

The behaviour of ClassWithCallback::callback is pretty much the same as the
freeFunction of our last example, but now the scope is not global but limited
to the scope of the class instance.

Scheduling Member Functions by Pointer
--------------------------------------
When we pass an instance of some type to a boost::bind expression it is copied.
This is fine and yields the expected results if we work with pointers, but it
does not necessarily yield the expected results if you pass your arguments
by value (see next section). To schedule a member function of an object where
you have a pointer to can be done like this:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.boostBindMemberFunction.example

Scheduling Member Functions by Value
------------------------------------
The code below illustrates the usage of boost::bind if you have references to
your callback objects. To avoid copying of your callback object when you pass
it as an argument to boost::bind, just wrap in an boost::ref or boost::cref
(wrapper for const references) object. Thereby, only the reference wrapper is
copied but not the reference itself. You could also have simply taken the
address (& operator) of your callback object and pass it to boost::bind to
enforce pointer semantics.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.boostBindMemberFunctionRef.example

Scheduling functions that take arguments
----------------------------------------
The examples above lack a very important feature. None of the above callbacks
can take an argument. Most often within your simulation you want to schedule
calls that take parameters. Boost::bind offers partial binding of parameters,
too. If you bind all parameters in an boost::bind expression, the result is
a nullary function object that you can the pass the the scheduler. The follwing
example illustrates this:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.boostBindMemberFunctionParam.example

This concludes the scheduler best practices lesson.

Cancelling Events
-----------------

Whenever you schedule an event by using either the ``schedule`` or ``scheduleDelay``
method of the eventschdeduler these methods return an instance of ``IEventPtr`` . This
is a handle for your scheduled event and can be used to remove it from the scheduler
queue again. This is demonstrated in the next example.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.events.scheduler.bestpractices.unittest.cancel.example

