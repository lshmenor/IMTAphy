=========
Scheduler
=========

.. _figure-wimac-scheduler:

.. figure:: images/scheduler.*


Components:

* General purpose & technology specific parts
* Strategy: How to schedule the PDUs 
* Queue: What PDUs are available for scheduling
* RegistryProxy: Provide system specific information:
* Callback: System specific interpretation of schedule


Timing:

* Start Collection: Ask upper FUs (Buffers) to pass PDUs to *Queue*
* Schedule: Let the *Strategy* analyze queued data and create a schedule
* Finish Scheduling: Pass the schedule and scheduled data to the *Callback*
* Deliver Schedule: Pass the technology specific schedule and the PDUs to the PhyUser and on to layer 1


Scheduler Spots:

* Three scheduler spots:
    * Downlink Master: Create DL schedule, all information is available
    * Uplink Master: Create UL schedule, needs information about queued bit in UT 
    * Uplink Slave: Read the MAP and schedule queued PDUs in the granted resources

Queue:

* Internally one queue per connection ID (CID)
    * Single QoS class: One CID per UT per direction
    * Multiple QoS classes: One CID per UT per direction per QoS class
* SimpleQueue: Retrieve head of queue PDU
* SegmentingQueue: Obtain a segment with given size in bit fitting a resource block
* QueueProxy: Virtual queue holding a copy of all UT queues
    * Used in the UL Master Scheduler
    * Could alternatively process system specific bandwidth requests


Strategy: Two Stage Static Priority Scheduling Strategy:

* Two tasks: Resource and packet scheduling
* Schedule highest priority which has data queued (stage 1, packet scheduling)
* Use sub-strategy to determine which UT should be scheduled (stage 2, resource scheduling)
* DynamicSubchannelAssignment (DSA): Strategy to find frequency/time resource
    * eg. for frequency selective fading or coexistence
* AdaptivePowerControl (APC): Assign transmission power to each subchannel


RegistryProxy: Provide system specific information

* Associated UTs
* Channel quality
* Available MCSs and switching points
* QoS classes
* Can be extended to provide any information useful for scheduling
    * eg. node positions
    * Interfering system type
    * ...

