Debugging your Code
===================

Finding Memory leaks
--------------------

To find memory leaks you can use valgrind. Reasonable settings are. Replace ROOTOFSDK and YOUR_PROGRAM

.. code-block:: bash

   > valgrind --tool=memcheck --leak-check=yes --num-callers=20 --leak-resolution=high \
     --log-file=val.log --suppressions=ROOTOFSDK/config/valgrind.supp YOUR_PROGRAM

CPU Profiling
--------------

To CPU profile your program you can also use valgrind. Use this commands:

.. code-block:: bash

    > scons optassuremsg callgrind=True

    > valgrind --tool=callgrind --instr-atstart=no ROOTOFSDK/sandbox/callgrind/bin/openwns

    > kcachegrind callgrind.out.*

Alternatively you can use gprof to perform CPU profiling.

.. code-block:: bash

    > scons optassuremsg profile=True

Afterwards use gprof to postprocess the profiling results.

SmartPtr Debugging
------------------

If you have cyclic SmartPtrs you probably want to make use of the SmartPtr debugging functionality of openwns. Execute

.. code-block:: bash

   > scons dbg smartPtrDBG=True

   > ROOTOFSDK/sandbox/smartptrdbg/openwns

At the end of the simulation run all SmartPtrs that were not properly deleted are shown. Each occurrence is accompanied by the call stack that led to its construction.

.. note::

   Please note that it is possible that openwns quits with a SIGSEGV at the simulation end.
   This is due to the undeterministic destruction sequence of static variables and the
   way SmartPtr debugging works.
