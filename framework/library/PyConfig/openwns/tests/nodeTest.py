import unittest
from openwns.node import Node

class NodeTest(unittest.TestCase):

    lastNodeID = None

    def setUp(self):
        self.lastNodeID = Node.id

    def testUniqueID(self):
        node1 = Node("Me")
        node2 = Node("first")
        node3 = Node("and")
        node4 = Node("the")
        node5 = Node("gimme")
        node6 = Node("gimmies")

        self.assertEqual(node1.nodeID, self.lastNodeID)
        self.assertEqual(node2.nodeID, 1 + self.lastNodeID)
        self.assertEqual(node3.nodeID, 2 + self.lastNodeID)
        self.assertEqual(node4.nodeID, 3 + self.lastNodeID)
        self.assertEqual(node5.nodeID, 4 + self.lastNodeID)
        self.assertEqual(node6.nodeID, 5 + self.lastNodeID)
