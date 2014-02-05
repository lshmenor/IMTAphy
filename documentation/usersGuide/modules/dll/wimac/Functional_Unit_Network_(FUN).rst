=============================
Functional Unit Network (FUN)
=============================

.. _figure-wimac-FUN:

.. figure:: images/FUN.*

* UpperConvergence: Write source and destination MAC address
* Classifier: Write connection ID, create IDs for new addresses
* TickTack / Probes: Measure
* Buffer: Queue data that cannot be sent this frame
* DeSeg / DeConcat: Reassamble IP PDUs (redo Seg & Concat of Scheduler Queue)
* CRC: Random drop by PER
* ErrorModel: Calculate PER
* Scheduler: Assign resources
* PhyUser: Interface to OFDMA Phy layer
