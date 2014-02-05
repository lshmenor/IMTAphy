======
HowTos
======

Adding a postprocess hook to your config
----------------------------------------

.. code-block:: python

    def myHook(simulator):
        # Do your postprocessing here.
        # You may access the simulator object which is passed to you

        # If you return False postprocessing will stop
        return True

    openwns.simulator.getSimulator().addPostProcessing(myHook)