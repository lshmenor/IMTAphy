.. _PyTree:

==============================================
Using the openWNS Configuration Browser PyTree
==============================================

PyTree is the openWNS configuration browser. It's main purpose is to
help you understand and debug simulation configurations. The Figure
:ref:`figure-PyTreeScreenshot` shows a small example of the PyTree.


.. _figure-PyTreeScreenshot:

.. figure:: pics/PyTreeScreenshot.*
   :align: center

   PyTree Screenshot

PyTree displays your configuration in a tree view. You can see the tree
structure on the left hand side. If you navigate the tree the current 
information on a tree node is shown on the right hand side.

PyTree can also plot Functional Unit Networks and visualize protocol stacks
that you have configured. The screenshot above shows the WiFi-MAC FUN.

Starting PyTree
---------------

To start PyTree change to a directory that contains your configuration
(we will use the WiFi-Mac test here).

::

  cd tests/system/wifimac-tests/


PyTree is located in the ``bin`` subdirectory of your openWNS-sdk.

::

    ../../../bin/pytree -g -p ../../../sandbox/dbg/lib/PyConfig config.py

We use several switches here. First, ``-g`` tells PyTree to run in GUI mode.
If you omit the switch the configuration is dumped to the console. You can
then use ``grep`` or other command line tools to find the information you need.
The second parameter ``-p`` tells pytree where to search for the ``PyConfig``
files. We tell PyTree to search in debug flavour within the sandbox.

.. note::
  You need to build your openwns-sdk before you can use PyTree. Make sure
  you have executed ``./playground.py install`` or at least have installed
  the PyConfig to the sandbox by running ``scons sandbox/dbg/lib/PyConfig``

The last argument to PyTree defines which configuration file to parse. That's
it. The PyTree window should appear and you are ready to go.


Navigating openWNS Configurations
---------------------------------

Most of the simulation configurations have a common structure. PyTree inserts
a tree node ``FAVOURITES`` at the top of the tree view. This entry is not actually a part
of your configuration but gives you some quicklinks to some commonly used information.
It contains the following entries:

  * ``measurementSources`` : Shows you which evaluation is installed by your configuration and which files will be written.
  * ``nodes`` : List of the nodes within your configuration.
  * ``simulator`` : The main simulator object.

Nodes consists of components. So each node has a list of components that it contains.
So, to find the MAC layer of a 802.11 station within this scenario go to

::

  nodes-->AP1-->components-->MAC AP1


The WiFi module of openWNS implements this simulation model. It builts the protocol stack
by using the layer development kit (LDK) of openwns-library. Protocols stacks in LDK 
are built by Functional Unit Netoworks (FUNs). You can use PyTree to plot those networks
by selecting the FUN in the tree view. So select:

::

  nodes-->AP1-->components-->MAC AP1-->fun

.. note::
  You need to have the ``dot`` tool installed. It is included in the ``graphviz`` package.

You can also customize the verbosity of the tree view. Use the View menu to tell PyTree to be
more verbose.

.. note::
  You must navigate the tree view to make the changes visible.



