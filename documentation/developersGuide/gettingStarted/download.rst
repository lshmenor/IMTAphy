.. _gettingStartedDownload:

--------
Download
--------


.. important::
  Before you actually start with the download make sure
  your system has all the necessary software to install and run openWNS: see
  :ref:`gettingStartedPrerequisites`.

.. note::
  This download instructions are a little bit longer than
  others (e.g. where you just need to download a .tgz-file). But it pays
  off as soon as you want to retrieve your first update. In the end the
  download should not take more than 5 minutes (time for downloading not
  included).

Short version
-------------

.. code-block:: bash

   $ bzr branch http://launchpad.net/~comnets/openwns-sdk/sdk--main--1.0 myOpenWNS
   $ cd myOpenWNS
   $ ./playground.py upgrade --noAsk


Configuring Bazaar
------------------

Like many other software projects openWNS is available via a revision
control system. The system is called Bazaar.  Thus, in order to
download openWNS you need Bazaar. For most distributions (SuSE,
Debian, Ubuntu, ...) Bazaar is available as package. In any other case
you will need to build it from the sources available here_

.. _here: http://bazaar-vcs.org/Download

To see if Bazaar is installed on your system and to check that you have
an up to date version you can try:

.. code-block:: bash

   $ bzr --version
   Bazaar (bzr) 0.92.0
   ...


If you haven't used Bazaar before you need to make yourself known to the system:

.. code-block:: bash

   $ bzr whoami "Joe Average <joe@average.com>"


Your id should be your name, followed by your email address in angle
brackets. Bazaar records your id in the log messages for your
commits. This information will @em not be used at any point when
you're just downloading openWNS (like now). For further information on
Bazaar you can have a look at http://bazaar-vcs.org .

Retrieve a copy of openWNS
--------------------------

Now it's time to retrieve a copy of openWNS. The following command will
checkout the current version of openWNS to 'myOpenWNS'. Choose
whatever you want as name.

.. code-block:: bash

   $ bzr branch http://launchpad.net/~comnets/openwns-sdk/sdk--main--1.0 myOpenWNS
   Branched 151 revision(s).
   $ cd myOpenWNS

Now you have a local copy of openWNS. Well, not really. What you have
is rather an empty house. If you inspect all the sub-directories of
``myOpenWNS`` at this moment, you would notice that they are almost
all empty. Apart from some bash and Python scripts there is not much
to see. Especially no C++ source code below the directory
``framework`` or ``modules``.

.. code-block:: bash

   $ ls framework/

So let's furnish this house! openWNS is designed to be a highly
modular simulation framework. Hence, it is made up of a number of
modules. Each module again is a Bazaar project (just like the one
you've just fetched). Normally you would have to fetch each Bazaar
project (or each module) by hand (like you did with this). This is
very tedious. Fortunately, there is a program that helps you with this
task (and other task as you will learn). It is called
``playground.py``. So to fetch all modules and necessary other data
simply enter:

.. code-block:: bash

  $ ./playground.py upgrade
  Warning: According to 'config/projects.py' the following directories are missing:
    [.. many directories ..]
  Try fetch the according projects? (Y/n) y

Just answer ``y`` to this question and all necessary projects will be
fetched. Depending on your link speed and the current size of openWNS this
can take several minutes.

After the download has finished you have all pieces available to
proceed with the installation of openWNS. Now there should be the
framework available:

.. code-block:: bash

  $ ls framework/*
  
  application/
  dllbase/
  library/
  pywns/
  rise/

