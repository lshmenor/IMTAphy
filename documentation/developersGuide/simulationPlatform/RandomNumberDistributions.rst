===========================
Random Number Distributions
===========================

The openWNS random number distributions are based on the Boost Random library. openWNS provides a frontend to generate random numbers according to specific random distributions. There are many commonly used distributions available and can be found in the ``wns::distribution`` (``wns.Distribution`` in Python) namespace. Available distributions are:

 * Fixed (Deterministic)
 * Negative Exponential
 * Normal
 * (Standard/Discrete) Uniform
 * Binomial
 * Geometric
 * Erlang
 * Poisson
 * Pareto
 * Rice

as well as arithmetic concatenations and truncated versions of them.

Constructing Random Number Distributions
----------------------------------------

Each distribution offers three constructors:

.. code-block:: c++

   wns::distribution::Uniform::Uniform(double low,
                                       double high,
                                       wns::rng::RNGen* rng = wns::simulator::getRNG())

   wns::distribution::Uniform::Uniform(const wns::pyconfig::View& config)

   wns::distribution::Uniform::Uniform(wns::rng::RNGen* rng,
                                       const wns::pyconfig::View& config)


The first constructor takes the parameters of the distribution and a random number generator engine. Default engine is the global simulator random number engine. **You should not change this unless you really know what you are doing!** Taking a different engine could lead to unwanted correlations in your simulator run.

The second constructor is intended to be used with the ``wns::distribution::DistributionCreator`` static factory and is configured in Python. The corresponding Python classes can be found under ``wns.Distribution``.

The third constructor allows to supply a custom random number generator engine using the ``wns::distribution::RNGDistributionCreator``. **Above warnings apply**.


Drawing Random Numbers
----------------------

Here is an example how to draw random numbers:

.. code-block:: c++

   wns::distribution::StandardUniform stdUni;
   std::cout << "Random Number from [0; 1.0] = " << stdUni() << "\n";

Random numbers are drawn using the ``operator()``. They are always of type ``double``. Note that StandardUniform has no parameters passed to the constructor. Its parameters are fixed.

Creating Distributions from the Static Factory
----------------------------------------------

To make your code independent of a specific distribution you can use the static factory mechanism included in openWNS. With this you can decide within your configuration which random number distribution is used during runtime without changing your C++ code.

You do not need to change anything in Python, so:

.. code-block:: python

    import wns.distribution

    dis = wns.distribution.NegExp(4711.0)
    conf.distribution = dis

In C++ you now need to use the plugin mechanism provided by the static factory. The code looks like this:

.. code-block:: c++

    #include <WNS/distribution/Distribution.hpp>

    wns::distribution::Distribution* dis;

    // Of course you need to get the dist config variable
    wns::pyconfig::View distConfig = config.get("dist");

    // Use the plugin mechanism to create the distribution
    std::string pluginName = distConfig.get<std::string>("__plugin__");
    wns::distribution::DistributionCreator* dc =
		wns::distribution::DistributionFactory::creator(pluginName);
    dis = dc->create(distConfig);

Use ``RNGDistributionCreator`` and ``RNGDistributionFactory`` to supply an own ``RNG`` to ``dc->create``. **Above warnings apply**

Random Number Distribution Base Classes
---------------------------------------

Most distributions derive from ``wns::distribution::IHasMean``
Besides ``operator()`` to draw a random number they offer the ``getMean()`` method to return their mean value.

You might want to use something like this if you dynamically created a Distribution:

.. code-block:: c++

   Distribution* dis = new StandardUniform();
   double mean = dynamic_cast<wns::distribution::IHasMean*>(dis)->getMean();


Rice distribution does not offer this method and therefore does not derive from ``wns::distribution::IHasMean``

Concatenation and Truncation of Random Number Distributions
-----------------------------------------------------------

Distributions can be concatenated or truncated using Python configuration:

.. code-block:: python

   import wns.Distribution

   # Create triangle distribution by adding two uniform distributions
   triangleDist = wns.Distribution.Uniform(0.0, 10.0) + wns.Distribution.Uniform(0.0, 10.0)

   # Create a neg. exp. distribution shifted by 6.0 to the right 
   shiftedNegExp  = wns.Distribution.NegExp(0.5) + 6.0

   # Create a truncated normal distribution on (-inf; 15.0)
   truncNorm = wns.Distribution.BELOW(wns.Distribution.Normal(0.0, 1.0), 15.0)

Available concatenations are ``+ - * /`` resulting distribution offers the ``getMean()`` method.

Available truncation operators are ``ABOVE`` and ``BELOW``. Resulting distributions **do not** offer the ``getMean()`` method.

Simulation Time Dependent Random Number Distributions
-----------------------------------------------------

Distribution can depend on simulation time using ``wns::distribution::TimeDependent``:

.. code-block:: python

   import wns.Distribution

   dist = wns.Distribution.TimeDependent()
   dist.eventList.append(wns.Distribution.Event(0.0, wns.Distribution.Uniform(40.0, 60.0)))
   dist.eventList.append(wns.Distribution.Event(2.0, wns.Distribution.Normal(50.0, 20.0)))
   dist.eventList.append(wns.Distribution.Event(5.0, wns.Distribution.Rice(0.0, 10.0)))


Depending on simulation time a different random number distribution is used. ``getMean()`` is not available.

Distributions Cheat Sheet
-------------------------

In Python you just need to instantiate a distribution from ``openwns.distribution``, e.g.

.. literalinclude:: ../../../.createManualsWorkingDir/wns.distribution.test.devguide.pyco.example

You may select one of the following distributions:

=============================  ======================================================================================
Distribution                   Configuaration Usage Example
=============================  ======================================================================================
Fixed (Deterministic)          ``openwns.distribution.Fixed(value = 2.34)``
Negative Exponential           ``openwns.distribution.Fixed(mean = 100.112)``
Normal                         ``openwns.distribution.Normal(mean = 100.112, variance=10.6)``
(Standard/Discrete) Uniform    ``openwns.distribution.Uniform(high = 11.0, low=9.0)``
Binomial                       ``openwns.distribution.Binomial(N = 10, p=0.3)``
Geometric                      ``openwns.distribution.Geometric(mean = 17.0)``
Erlang                         ``openwns.distribution.Erlang(rate = 1.23, shape=1.1)``
Poisson                        ``openwns.distribution.Poisson(mean=7.0)``
Pareto                         ``openwns.distribution.Pareto(shapeA = 0.4, scaleB = 0.3, xMin = 0.0, xMax = 1e100)``
Rice                           ``openwns.distribution.Rice(losFactor = 0.4, variance = 3.2)``
=============================  ======================================================================================

In C++ you can then create the distribution you just configured:

.. code-block:: c++

    #include <WNS/distribution/Distribution.hpp>

    wns::distribution::Distribution* dis;

    // Of course you need to get the dist config variable
    wns::pyconfig::View distConfig = config.get("dist");

    // Use the plugin mechanism to create the distribution
    std::string pluginName = distConfig.get<std::string>("__plugin__");
    wns::distribution::DistributionCreator* dc =
		wns::distribution::DistributionFactory::creator(pluginName);
    dis = dc->create(distConfig);

    // Draw random numbers
    double randomNumber = dis();


