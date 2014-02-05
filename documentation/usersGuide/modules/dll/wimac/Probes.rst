======
Probes
======

.. _figure-wimac-probes:

.. figure:: images/probes.*
   :align: center


* a (Frame start): Fill the scheduler queue, queue is limited per connection
    * (1): Wait for frame start + wait for space in scheduler queue
* b (Scheduled): IP PDU **segment** is removed from queue
    * (2): Delay until **last segment** of IP PDU is extracted
    * \(2) UL: Wait to receive MAP, extract PDUs at UL subframe start
* c (TX Start): Transmission start
    * (3): Time offset within the currently scheduled frame
* d (TX Stop): Transmission stop
    * (4): Transmission time (PDU length in bit / data rate)
* e (Reassembled): IP PDU fully reassembled and passed to layer 3
    * (5): Reassembly delay
        * Wait for last segment of IP PDU
        * Wait for last segment of earlier IP PDUs or wait for them to be dropped


.. _figure-wimac-FUN:

.. figure:: images/FUN.*


Probe delay and loss:

* Between two points ("Tick", "Tack")
* delay: tackTime - tickTime
* loss: sum(tickBit) - sum(tackBit)
* "Tick" is a FU
* "Tack" is FU or implemented in a FU / class



.. |arrow_return| image:: images/arrows/arrow_return.*

|arrow_return|  from sender to reciever


.. |arrow_down| image:: images/arrows/arrow_down.*

.. |arrow_up| image:: images/arrows/arrow_up.*

|arrow_down| at sender |arrow_up| at resciever


.. |arrow_dual_down| image:: images/arrows/arrow_dual_down.*

|arrow_dual_down|   "Tick" & "Tack" in same FU//class



Probes:

* TopProbe: wimac.top.packet.
    * incoming.delay
    * incoming.size
    * outgoing.size
* BufferTickTack: wimac.buffer.
    * delay
    * start.compoundSize
    * stop.compoundSize
* SchedulerTick => Queue: 
    * wimac.schedulerQueue.delay
* Callback (system specific scheduler output interpreter)
    * wimac.frameOffsetDelay
    * wimac.transmissionDelay
* CRCTickTack: wimac.crc.
    * start.compoundSize
    * stop.compoundSize (Probe channel errors)
* DeSegDeConcanat: wimac.reassembly.
    * minDelay
    * maxDelay
    * start/stop.compoundSize


