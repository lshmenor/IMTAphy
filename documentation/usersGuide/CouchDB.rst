===============================
CouchDB channel schedule viewer
===============================

From Ubuntu Linux 10.04 (Lucid) on a tool integrated in Wrowser is available to visualize the channel occupation. The wireless system modules WiFiMAC and WiMAC produce the required output when ran in **debug** **(dbg)** mode.

For that the method traceIncoming has been added in src/PhyUser.cpp in WiMAC and src/convergence/FrameSynchronization.cpp in WiFiMAC.

The output is written to a probing measurement source called wimac.phyTrace and wifimac.linkquality.phyTrace for WiMAC and WiFiMAC respectively. The following lines need to be added to the simulator configuration file to enable the measurement sources:

WiMAC:

.. literalinclude:: ../../.createManualsWorkingDir/wimac.test.couchdb
   :language: python

WiFiMAC:

.. literalinclude:: ../../.createManualsWorkingDir/wifimac.test.couchdb
   :language: python


After adjusting the configuration, the simulation must be started and run until it finishes.

When Wrowser is started on a Linux system with CouchDB available and running (this should be the case for a default installation from Ubuntu Linux version 10.04 "Lucid" on), the CouchDB option should be available under the "File" menu. When activated the screen shown in Figure 1 opens.


.. _figure-CouchDB01:

.. figure:: pics/CouchDB01.*
   :align: center



Click the "Import" button on the lower left side and browse to the "output" directory of the previously executed simulation run. There should be a file named wimac.phyTrace_Text.dat or wifimac.linkquality.phyTrace_Text.dat depending on the simulated system. Select the file and pick a database name. CouchDB requires database names to only include lower case letters.

After successful import, the new database is visible in the list. It can be opened by double-clicking it. After that the screen shown in figure 2 opens. The drawing area on the right shows the channel occupation for a given time period. The x-axis represents the simulation time, the y-axis represents the OFDMA subchannel number for WiMAC simulations and the address of the source node for WiFiMAC simulations. Navigation functionalities to slide further in time are available on the upper left side. The unit is 1 ms, while the figure always shows 10 ms. The +10 and -10 buttons can be used to jump 10 ms forward or backward in time. The text box shows the current start time and can be used to jump to an arbitrary time.


.. _figure-CouchDB02:

.. figure:: pics/CouchDB02.*
   :align: center



The colors of the boxes represent the user terminals (UTs) (the term "UT" is also used for WiFi Stations (STAs)) transmitting or receiving at the given time on the given resource. The "Source" and "Receiver" lists below the navigation tools can be used to filter and only show selected transmissions. When clicking such a colored box representing a transmission, additional information is shown in the table below. Actually the drawn boxes only represent the "top" transmission happening on a resource. If multiple transmissions are active simultaneously, the table on the bottom lists all transmissions. Information shown includes: Where was the measurement recorded (ReceiverID), who is sender (SenderID), who is the source in a multi-hop scenario (SourceID), who should receive the transmission (DestinationID), what is the measured SINR of this transmission in this receiver (SINR), etc.

The text box on the left can be used to define arbitrary filters by writing Python code. The function receives a database entry and must return "True" if it should be plotted or "False" otherwise.
