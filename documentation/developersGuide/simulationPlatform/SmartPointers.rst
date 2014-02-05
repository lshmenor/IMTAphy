==============
Smart Pointers
==============

Motivation
----------
As classes get created dynamically memory management, especially releasing memory occupied by data structured not used anymore becomes an issue. The C++ programming language uses the keywords new and delete for this. In most cases this task is simple. A new object is created within some existing class instance and stored there. Many times this is done at startup. So the class instance, which created the new object, will also delete it, most likely at the end of its own lifetime.

Things get more complicated if objects are created and "passed" around to other objects. An example for that are protocol data units (PDUs), in the following called packets, in a network simulator. Some source object in a traffic generator creates packets and then passes them down the protocol stack. At the receiving site packets are passed up the stack until they reach a sink. Now who should delete a packet once it is not needed anymore? The source? But how can it know if the packet was received. The sink? What if the packet gets lost during transmission and never reaches the sink? What if it is a broadcast or multicast packet received by multiple sinks? 

There could be many more possible places that could be potentially used to delete the packet object, but they all have drawbacks. The answer on when the packet object should be deleted: "When nobody needs it anymore". 

To achieve this, the packet object itself needs to keep track of how many other objects are referencing it. Once it discovers that there is nobody referencing it, it can safely call the delete method.


Usage Example
-------------
In the queuing network example ``WNS/queuing/MM1StepX.cpp`` job objects of class ``wns::queuing::Job`` are created and stored in a Smart Pointer of type ``wns::queuing::JopPtr``. The Smart Pointer declaration is just a typedef to ``wns::SmartPtr<Job>``. Jobs are created using the new in onCreateJob and stored in the Smart Pointer using its constructor. They are then stored in the queue. After a job is processed in onJobProcessed there is no need to call the delete method. After the job is extracted from the queue the queue does not reference it anymore. It is only referenced by the local variable XXX in the onJobProcessed method. Once this methods returns the lifetime of XXX ends. Now there is no reference to the job anymore. The Smart Pointer will then automatically release the memory occupied by the job.

Accessing Smart Pointers
------------------------
Smart Pointers must be accessed like any other pointer. Members are accessed using the "->" operator. The object referenced by the Smart Pointer is accessed using the star "*" operator.  

Creating Own Smart Pointers
---------------------------
A class MyClass which should be stored in a Smart Pointer needs to derive from wns::RefCountable. This assures it is able to keep track how often it is referenced. The Smart Pointer itself is declared as ``wns::SmartPtr<MyClass>``. It is common to provide a typedef in ``MyClass.hpp``.
``typedef  wns::SmartPtr<MyClass> MyClassPtr;``

Notes on Smart Pointers
-----------------------
Objects kept in Smart Pointers should never be also accessed using normal pointers. The Smart Pointer does not have knowledge about normal pointers referencing the object and could delete it while it is still needed by code using the normal pointers.

Sorting Smart Pointers: When storing Smart Pointers in a sorted container you most likely want to use the comparison method of the class the Smart Pointer points to. You therefore need to provide XXX as the sorting method which will automatically call the intended sorting method.  

Circular references: A Smart Pointer should never keep another Smart Pointer to itself or to another class pointing back to itself. Example: Imagine a class for a double linked list keeping a pointer to the next and previous object in the list. Object 1 has a next pointer to object 2 and object 2 a previous pointer to object 1. If object 1 gets removed from the list occupied memory will not be released since it is still referenced by object 2. The memory occupied by object 2 will never be released since object 2 is referenced by object 1.

Debugging Smart Pointers
------------------------
ToDo
