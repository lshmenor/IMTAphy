Performance Tips
================

Distributed Compiling
---------------------

The first compilation of openWNS after a fresh checkout can be very
time consuming if it is performed on a single computer. Building a
version for debugging and an optimized version can take from one
to more than two hours depending on the capabilitie of your computer.

If a network of computers is available (preferably all installed
indentically) and ICECC is available on these computers (see
http://en.opensuse.org/Icecream) openWNS can be compiled distributed.

To enable distributed compilation the following options may be set in
``config/private.py``:

.. code-block:: python

  privateEnv.Replace(ICECC='icecc')
  privateEnv.configureCompiler(cc='/usr/bin/gcc', cxx='/usr/bin/g++')

.. note::

   The statements must occur in this order. Otherwise icecc will not be used.

In order to visualize the distributed compiling, open the ICECC monitor
on any computer that is part of the network, e.g., on 'myHost':

.. code-block:: bash

  $ icemon &

Object Files
------------

openWNS provides a way to put all object files into a separate
directory. It is a good idea to put this directory onto another
(fast) disk. When linking the libraries and executables the object
files can be read from the other disk and are written to the disk
where the SDK resides.

To enable this feature do the following:

.. code-block:: bash

   $ mkdir pathToExternalObjDirOnOtherHardDisk

After this simply edit your ``config/private.py`` to contain the
following line:

.. code-block:: python

   privateEnv.setExternalObjDir("pathToExternalExternalObjDirOnOtherHardDisk")

That's it. All object files will be written and read from there.

.. note::
   
   This can also be used in case the left space on a partition is getting low.


