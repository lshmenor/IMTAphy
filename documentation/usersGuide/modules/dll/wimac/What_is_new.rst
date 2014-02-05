============
What is new?
============

* Removed or simplified control and management functionalities
    * Dynamic association
    * Handover
    * Bandwidth requests
    * MAPs
* Control and management traffic overhead is modeled as resources not available to user data traffic
* New scheduler
    * Supports multiple QoS classes
    * Supports OFDMA
    * Also used in LTE module, calibrated within the WINNER+ project for IMT-A 
* Dynamic segmentation and concatenation
    * DLL-PDUs exactly fit OFDMA phy layer resources => less fragmentation
* Many new probes and reference output to validate correct behavior  