.. _gettingStartedPostgreSQL:

----------------------------------
Installing the PostgreSQL database
----------------------------------

To allow for an efficient viewing, analyzing and management of large
amounts of simulation output, the openWNS supports a database frontend
based on the open-source PostgreSQL_ database. In the following, this
database will be configured to work together with the openWNS.

.. important::
   The following steps are only required if you want to
   store simulation results into a PostgreSQL database. To run a
   single simulation this is not required.

Prerequisites
-------------

  * PostgreSQL_ A powerful, open source object-relational database system.
  * python-psycopg2_ (>= 2.0) A PostgreSQL database adapter for the Python programming language.

For Ubuntu 10.04 and below
   .. code-block:: bash

      $ sudo apt-get install postgresql python-psycopg2

For Ubuntu 12.04 (currently no support for PostgreSQL 9.X)
   .. code-block:: bash

      $ sudo apt-get install postgresql-8.4 python-psycopg2



.. _PostgreSQL: http://www.postgresql.org/
.. _python-psycopg2: http://freshmeat.net/projects/psycopg/

Adding a database user
----------------------

The following section is only valid for a virgin installation of the
PostgreSQL without any existing users. If you would like to use an
already existing PostgreSQL, some of the following steps are not
neccessary. Please contact your database administrator and the
PostgreSQL manual for further information.

#. After a virgin installation, the database-admin user ``postgres``
   does not have any password. To change it to ``foobar``, type

   .. code-block:: bash

      $ su postgres
      $ psql
      Welcome to psql 8.3.6, the PostgreSQL interactive terminal.

      postgres=# ALTER ROLE postgres PASSWORD 'foobar';
      ALTER ROLE
      postgres=# \q
      $ exit

#. Furthermore, the database can be accessed from everywhere, not
   only locally. To change this, edit the file
   ``/etc/postgresql/8.3/main/pg_hba.conf``:
 
   * Comment out the line ``local all postgres ident sameuser``.
   * Change the line ``local all all ident sameuser`` to
     ``local all all md5``.

   Then, restart PostgreSQL with

   .. code-block:: bash

      $ /etc/init.d/postgresql-8.3 restart

#. Now, you can create the database required for the simulation. The
   default name in all scripts is ``simdb``, so it is recommended to use
   this name:

   .. code-block:: bash

      $ su postgres
      $ psql
      Password: foobar
      Welcome to psql 8.3.6, the PostgreSQL interactive terminal.

      postgres=# CREATE DATABASE simdb;
      postgres=# \q
      $ exit

#. Then, the neccessary tables to store simulation campaign
   parameters and results have to be created. For this purpose, a
   script can be used which is part of the wrowser. It can be found
   in the directory ``wrowser/simdb/sql``:

   .. code-block:: bash

      $ cd wrowser/simdb/sql
      $ psql -U postgres -d simdb -f setupSimDB.sql

#. Finally, a database user account for your user account must be
   created. Again, a script is prepared for this, now in the
   directory ``wrowser/simdb/scripts``:

   .. code-block:: bash

      $ cd wrowser/simdb/scripts
      $ ./createUser.py

   .. note::
      Before running the ``./createUser``, check that in this
      script the two variables ``hostname`` and ``dbName`` are set to
      the correct values, i.e., ``localhost`` and ``simdb`` if this
      installation guide is followed.

   .. important::
      The script assigns a default password to the created
      user account, which is ``foobar``. You can change this password
      by editing the variable ``password`` in the script before
      running. Do not use your default user password - the database
      user account password is not stored secure!

Now, a account is created on the database which can be used to store
the simulation results.

Configuring the database information in the Wrowser
---------------------------------------------------

The Wrowser needs to know where to find the campaign database. This
can be configured by starting the Wrowser and then selecting
``Extra``, ``Preferences`` in the menu. In the following dialog, see
:ref:`figure-gettingstarted-wrowser-configDB`, please fill in the
following values:

 * Hostname: ``localhost``

 * Databasename: ``simdb``

 * Username: Your user account name

 * Password: ``foobar`` if not changed in the ``createUser.py`` script.


.. _figure-gettingstarted-wrowser-configDB:

.. figure:: images/wrowser-ConfigDB.*
   :align: center

   Setting the database.

Then, select the tab ``Sandbox`` and fill in the complete path to the
sandbox of your openWNS installation, e.g.,
``/home/userName/myOpenWNS/sandbox``.
