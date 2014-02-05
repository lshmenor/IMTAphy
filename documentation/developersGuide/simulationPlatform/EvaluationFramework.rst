.. _simulationPlatformEvaluationFramework:

Evaluation Framework
====================

Describe the following:

* Introduction
* Concepts
  * TreeNode
  * TreeNodeSet
  * Generators
* Quickstart
* HowTos
  #. Construct a new evaluation tree
  #. Working with TreeNodeSets
  #. Write your own Generators
  #. Advanced : Tagging NodeSets
* Internals
  #. Replacement of ProbeBusRegistry

Introduction
------------
The openWNS Evaluation Framework built on top of the ProbeBus implementation. A short recap
of the ProbeBus concept is given here to help you understand the basic concepts. For an in-depth
understanding of the evaluation framework you should also make yourself familiar with the ProbeBus
framework.

* ProbeBus : A ProbeBus may be observed by other ProbeBusses. A ProbeBus forwards the current
  simulation time, a measurement and context information to its observers. A ProbeBus may accept
  only a subset of the forwarded information depending on the provided context. In this way tree
  structures can be realized to sort measurements.

* Context : The context of a measurement is additional information that is provided to allow for
  sorting of measurements, e.g. when measuring SINR values it the context could be the current 
  position of the terminal. In this way the spatial SINR distribution could be evaluated.

* MeasurmentSource : A ProbeBus within your simulator where a measurement originates from and is
  published to subsequent ProbeBusses. A MeasurementSource has a unique name for identification.

Concepts
''''''''

The evaluation framework of openWNS is used to build trees of ProbeBusses. The whole evaluation framework
is only used during configuration time (in Python). The tree structures are mapped to tress of ProbeBusses
at run-time. You will encounter three concepts when working with the evaluation framework.

* \pycoshort{openwns.evaluation.TreeNode} : The fundamental concept. Represents a node within a tree. A TreeNode
  may have a parent and it may have multiple children.

* \pycoshort{openwns.evaluation.TreeNodeSet} : Contains several TreeNodes. One TreeNode is only included once.

* \pycoshort{openwns.evaluation.generators} : A generator creates new tree nodes.

Quickstart
----------

The openWNS Evaluation Framework is used to define how measurements are sorted according to their
Context. For example, say you have a measurement source 'AssociationEvents' in your simulation model
and each station (mobile and base station) provides the MAC.Id context with its own unique MAC-ID.
Assume that you want to have a time series for every base station of its association events. Here is
how to define this in your configuration by using the openwns evaluation framework.

.. code-block:: python

  bsIdList = [1, 2]
  node = openwns.evaluation.createSourceNode(sim, 'AssociationEvents')
  node.appendChildren(Separate(by = 'MAC.Id', forAll = bsIdList, format='MAC_ID%d'))
  node.getLeafs().appendChildren(TimeSeries())

.. graphviz::
  digraph overview {
  size="8,10"
  mac1 [label = "MAC_Id == 1"]
  mac2 [label = "MAC_Id == 2"]
  AssociationEvents -> mac1
  AssociationEvents -> mac2
  time1[label = "TimeSeries"]
  time2[label = "TimeSeries"]
  mac1 -> time1
  mac2 -> time2
  }

After your simulation has finished you will find two files in your output directory, which contain
the respective time series. The files are named:

.. code-block:: bash

   AssociationEvents_MACId1_Log.log.dat
   AssociationEvents_MACId2_Log.log.dat


Every file starts with its measurement source name. The Separate generator appends for each ID a suffix
according to the format specifier. In this case MACId1 and MACId2 are appended. For historical reasons
the TimeSeries generator appends the file suffix 'Log.log.dat'. Suffixes are separated by an underscore
character.

HowTos
------
Creating a new evaluation tree
''''''''''''''''''''''''''''''

To create a new evaluation tree for a measurement source you should first get the appropriate source node.

.. code-block:: python

   node = openwns.evaluation.createSourceNode(sim, 'SINR')


This will create a new TreeNode and attaches it to the measurement source 'SINR'. If a TreeNode for this
measurement source already exists, it will instead return that TreeNode (possibly including all the
child nodes already configured).


Basic Evaluation
''''''''''''''''

Now lets attach some evaluation to this measurement source.

.. code-block:: python

   from openwns.evaluation import
   node = openwns.evaluation.createSourceNode(sim, 'SINR')
   node.addChildren(Moments())

This will attach an evaluation node to this measurement source that reports basic statistics of ALL
measurements. The Moments generator creates exactly one TreeNode which evaluates the basic statistics.

.. code-block:: python

   SINR_Log.dat

Basic Sorting
'''''''''''''

Now lets do some sorting. Suppose all your SINR measurements have a context of your stations MAC_ID.
We now collect the basic statics for each of your station. In this example we will have 4 Stations
with MAC_IDs 1,2,3 and 4.

.. code-block:: python

   from openwns.evaluation import
   node = openwns.evaluation.createSourceNode(sim, 'SINR')
   node.addChildren(Separate(by = 'MAC_ID', forAll = [1,2,3,4], format='MAC_ID%d'))
   node.getLeafs().addChildren(Moments())


The Separate generator creates one TreeNode for each entry in the list 'forAll'. Each of these
nodes only accepts measurements where the context entry 'MAC_ID' is of the respective value.
The getLeafs() method returns a NodeTreeSet of all leafs of the tree. Calling addChildren() on
a NodeTreeSet will use the generator to create new nodes for each TreeNode in the TreeNodeSet. Here,
4 TreeNodes that evaluate the basic statistics will be created in total. The resulting tree looks like this.

.. graphviz::
  digraph overview {
  size="8,10"
  f [shape=plaintext, label = "Filename:"]
  s [shape=plaintext, label = "SINR"]
  m [shape=plaintext, label = "MAC_ID%d"]
  mo [shape=plaintext, label = "Log.dat"]
  mac1 [label = "MAC_Id == 1"]
  mac2 [label = "MAC_Id == 2"]
  mac3 [label = "MAC_Id == 3"]
  mac4 [label = "MAC_Id == 4"]
  SINR -> mac1
  SINR -> mac2
  SINR -> mac3
  SINR -> mac4
  mom1[label = "Moments"]
  mom2[label = "Moments"]
  mom3[label = "Moments"]
  mom4[label = "Moments"]
  mac1 -> mom1
  mac2 -> mom2
  mac3 -> mom3
  mac4 -> mom4
  { rank = same; "s"; "SINR"; }
  { rank = same; "m"; "mac1"; "mac2"; "mac3"; "mac4";}
  { rank = same; "mo"; "mom1"; "mom2"; "mom3"; "mom4";}
  f -> s [color = white]
  s -> m [arrowhead=none, label=" + '_'"]
  m -> mo [arrowhead=none, label=" + '_'"]
  }

You also have some control to the naming of your files. Filenames are constructed by concatenating the
format strings of each TreeNode starting from the root node and then going down to the child nodes. For
example, the leftmost Moments TreeNode will have a filename which is the concatenation of the format string
of the root TreeNode ('SINR', you do not have control on that) and the string 'MAC_ID1' and a string 'Log.dat', which
is attached by the Moments TreeNode itself. Each of these parts of the filename are concatenated by an underscore
character.

After your simulation has finished you will have four files in your output directory which contain
the SINR statistics for each station (i.e. for each MAC_ID).

.. code-block:: bash

   SINR_MAC_ID1_Log.dat
   SINR_MAC_ID2_Log.dat
   SINR_MAC_ID3_Log.dat
   SINR_MAC_ID4_Log.dat

Memory and CPU time probing
'''''''''''''''''''''''''''

The simulation library provides probing output of memory consumption and ratio of simulation time to real time. Following must be added to the configuration to write the output of those probes:

.. code-block:: python

  import openwns.evaluation.default

  openwns.evaluation.default.installEvaluation(openwns.simulator.getSimulator())

This will create four files in the output directory:

  * wns.Memory_Moments.dat
  * wns.Memory_TimeSeries.dat
  * wns.SimTimePerRealTime_Moments.dat
  * wns.SimTimePerRealTime_TimeSeries.dat

Moments probes contain the statistics over the whole simulation run, TimeSeries probes show the current memory consumption and current simulation time divided by total runtime of the simulator. They are updatet using the status writer. Update frequency can be adjusted by the following line in the configurtion:

.. code-block:: python

  openwns.simulator.getSimulator().statusWriteInterval = 30 # in seconds realTime

The interval should not be chosen too low or else the simulator will be busy writing output to disk all the time.


Writing your own Generators
---------------------------

.. code-block:: python

  from openwns.evaluation.generators import
  from openwns.evaluation.tree import
  from openwns.evaluation.wrappers import

  import openwns.probebus

  class SeparateByQoSClass(ITreeNodeGenerator):

      def __init__(self, format="QoS%s"):
         self.format = format
         self.contextKey = "QoSClass"
         self.QoSClasses = []
         self.QoSClasses[0] = 'background'
         self.QoSClasses[1] = 'streaming'
         self.QoSClasses[2] = 'conversational'
         self.QoSClasses[3] = 'control'

      def __call__(self, pathname):
          for i in len(self.QoSClasses):
              probebus = openwns.probebus.ContextFilterProbeBus(self.ContextKey,
	                                                        [ self.QoSClasses[i] ]
								)

              wrapper = wrappers.ProbeBusWrapper(probebus,
						 self.format % self.QoSClasses[i])

	      treeNode = tree.TreeNode(wrapper)

	      yield treeNode

