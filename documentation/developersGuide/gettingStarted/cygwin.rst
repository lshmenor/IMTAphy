.. _gettingStartedCygwin:

------
Cygwin
------

Installing Cygwin
-----------------

Download Cygwin from `here <http://www.cygwin.com>`_. For this guide version 1.5.25-15 has been tested. A full installation of all Cygwin packets was done to assure dependencies are met.

Scons
-----

There is no binary package of the build tool Scons for Cygwin so you need to download the Tarball from `here <http://www.scons.org>`_. Version 1.2.0 was succesfully tested. Untar and unzip the downloaded file, enter the newly created directory and run

.. code-block:: bash

    $ python setup.py install

Boost
-----

Boost 1.33 libraries are included in tested Cygwin version. Unfortunatelly there is a bug in one file that needs to be patched. Alternatively Boost >=1.34 could be installed. Get the patch from `here <https://svn.boost.org/trac/boost/changeset/39227>`_ or simply edit /usr/include/boost-1_33_1/boost/numeric/ublas/lu.hpp.

Create a symbolic link to the Boost header files:

.. code-block:: bash

    $ ln -s /usr/include/boost-1_33_1/boost /usr/include/boost

Create symbolic links to the libraries:

.. code-block:: bash

    $ ln -s /usr/lib/libboost_date_time-gcc-mt-s.a /usr/lib/libboost_date_time.a
    $ ln -s /usr/lib/libboost_program_options-gcc-mt-s.a /usr/lib/libboost_program_options.a
    $ ln -s /usr/lib/libboost_signals-gcc-mt-s.a /usr/lib/libboost_signals.a

Python
------

Create a symbolic link to the Python library:

.. code-block:: bash

    $ ln -s /usr/lib/python2.5/config/libpython2.5.dll.a /usr/lib/libpython2.5.a

Compiling openWNS
-----------------

You have to compile openWNS statically under Cygwin using by including the --static option to playground.py install.

.. code-block:: bash

    $ ./playground.py install --static
    
