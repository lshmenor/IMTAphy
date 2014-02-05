################################
Experiment 7: IEEE 802.11 DraftN
################################

So far, the available parameters to configure a mesh or a STA were
restricted to setting e.g.

* the transmission power: ``txPower``
* the beacon delay: ``layer2.beacon.delay``
* the rate adaptation strategy: ``layer2.ra.raStrategy``
* the rtsctsThreshold: ``layer2.rtsctsThreshold``

In this experiment, we will explore the complete configuration
structure of the WiFiMAC, which allows the fine-tuning of multiple
parameters of the IEEE 802.11 protocol. As a goal of this experiment,
the configuration from experiment 5 shall be changed so that 3x3
Multiple Input / Multiple Output (MIMO) transmissions between the APs
and the MPs are used; furthermore, the efficiency of the MAC protocol
is increased by A-MPDU aggregation and block acknowledgement.

.. note:

   In addition to the process described here, it is also possible to
   view the complete configuration of a scenario (including the
   configuration of all nodes) :ref:`PyTree`.

*******************************
WiFiMAC Configuration Structure
*******************************

All files belonging to a campaign can be found in the ``sandbox``
directory, right under the root directory of the campaign. The
configuration files for the WiFiMAC module can be found in the directory

.. code-block:: bash

   sandbox/opt/lib/PyConfig/wifimac

Each subdirectory provides

#. Configuration settings for the Functional Units (FUs) of the WiFiMAC, and

#. Functions to connect the FUs to a Functional Unit Network (FUN).

To explore the configuration possibilities, we start with the class
that is used to create MP/AP transceiver,
``wifimac.support.Transceiver.Mesh``. According to its name, it can be
found in the file ``Transceiver``, located in the directory
``support``:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.pyconfig.support.transceiver.Mesh
   :language: python

Besides being derived from the class ``Basic``, it activates the
beaconing by setting ``self.layer2.beacon.enabled`` to ``True``. The
class ``Basic`` can be found some lines above:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.pyconfig.support.transceiver.Basic
   :language: python

This class defines the basic properties of every IEEE 802.11 -
transceiver: its frequency, the transmission power and its
position. Furthermore, the transceiver has to have an implementation
of the Layer 2 (the PHY Layer is added by the NodeCreator, which can
also be found in the ``support`` directory.

In the ``__init__`` function of the class ``Basic``, the frequency is
set and the configuration of the Layer 2 is instantiated with the
default configuration of ``wifimac.Layer2.Config``, which can be found in the file

.. code-block:: bash

   sandbox/opt/lib/PyConfig/wifimac/Layer2.py

At the beginning of the file, the creation of the FUNs for stations,
MPs and APs is encapsulated in different functions. At the end, the
class ``Config`` is defined. In the first part, variables are
declared that will hold the configurations for different FUs:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.pyconfig.layer2.config.start
   :language: python

Then, some variables are declared that contain settings for FUs that
do not have their own configuration class, or for variables that steer
the creation of the FUN.

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.pyconfig.layer2.config.variables
   :language: python

After this, variables are declared which are used in multiple FUs. The
``rtsctsThreshold``, for example, is not only required in the RTS/CTS
- FU, but also in the acknowledgement - FU, as this size controls
whether the long- or the short retry counter limit is applied.

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.pyconfig.layer2.config.multiusevariables
   :language: python

In the ``__init__`` function, the several sub-configuration classes
are initialised with their default sub-configurations:

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.pyconfig.layer2.config.init
   :language: python

The remaining part of the function then controls the initialisation of
the variables which are used in multiple FUs.

Therefore, a configuration variable is either found on this level of
the configuration tree (e.g. ``bufferSize`` or ``sifsDuration``), or
in one the sub-configurations. Especially interesting is the variable
``funTemplate``, which is a pointer to a class that is able to
generate the functional unit network of the node. Its default value,
``wifimac.FUNModes.Basic``, represents the class that creates a FUN
for the basic operation of IEEE 802.11. To enable the enhancements of
the "n" amendement, it has to be set to ``wifimac.FUNModes.DraftN``.


The file structure where the sub-configuration classes can be found
can easily be derived from the name of the class: For example, the
configuration for the beacon, stored in ``self.beacon``, is
initialised with ``wifimac.management.BeaconConfig()``. Hence, it can
be found in the file

.. code-block:: bash

   sandbox/opt/lib/PyConfig/wifimac/management/Beacon.py

.. literalinclude:: ../../../../../.createManualsWorkingDir/wifimac.pyconfig.layer2.management.beacon.beaconconfig
   :language: python

As we can see from the configuration below, it is possible to change

* the initial start delay - this is the parameter we have already used
  to avoid beacon collisions,

* the beacon transmission period,

* the scan duration for STAs and

* the PHY mode id with which the beacons are transmitted.

************
Experiments
************

#. Draw a "Configuration Tree" (with pencil & paper): Starting from the configuration class
   in ``Layer2.py``, collect all settings in the sub-configuration and
   display them in a tree-like structure. Alternatively, see
   :ref:`PyTree`, e.g. for the configuration file of experiment 1,
   search for the AP-node and then for its Layer2 configuration.

#. Selecting the class ``wifimac.FUNModes.DraftN`` for the variable
   ``funTemplate`` activates the MAC enhancements of the amendment
   IEEE 802.11n. This is done by adding an appropriate Line to the classes ``MyMeshTransceiver`` and ``mySTATransceiver``. How are the following parameters changed:

   #. Maximum number of frames in an aggregated frame

   #. Maximum number of frames that can be on air before the
      transmission of a BlockACK - request.

   #. Number of transmit/receive antennas.

#. Create a new sub-campaign that uses the configuration file from
   experiment 5. Enable the enhancements from IEEE 802.11n in both the
   mesh and the STA transceiver, with the following settings:

   * Maximum frame aggregation size: 10

   * Maximum frames on air before the BlockACK - request: 10

   * Number of antennas for mesh transceivers: 3

   * Number of antennas for STA transceivers: 1

   Additionally, the mesh transceivers shall use the rate adaptation
   strategy ``SINRwithMIMO`` to enable the transmission of multiple
   spatial streams; STAs shall use the rate adaptation strategy
   ``OpportunisticwithMIMO``.

#. Evaluate the saturation throughput of the scenario in experiment 5 using

   #. Different number of hops (e.g. from one to three)

   #. Different packet sizes
