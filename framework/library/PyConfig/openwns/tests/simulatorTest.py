# begin example "wns.simulatorTest.unittest.example"
import unittest
import openwns.simulator

class ModuleMock:
    def __init__(self, plugin):
        self.__plugin__ = plugin

class ModulesTests(unittest.TestCase):

    def setUp(self):
        self.testee = openwns.simulator.Modules()
        self.testee.foo = ModuleMock("moduleA")
        self.testee.bar = ModuleMock("moduleB")

    def testLen(self):
        self.assertEqual(len(self.testee), 2)

    def testAccess(self):
        self.assertEqual(self.testee[0].__plugin__, "moduleA")
        self.assertEqual(self.testee[1].__plugin__, "moduleB")
        self.assertEqual(self.testee[-1].__plugin__, "moduleB")

    def testIter(self):
        foo = []
        for it in self.testee:
            foo.append(it)

        self.assertEqual(foo[0].__plugin__, "moduleA")
        self.assertEqual(foo[1].__plugin__, "moduleB")

    #def testDenyDoubleAttributeAddition(self):
    #    def provokeException():
    #        self.testee.foo = ModuleMock("moduleC")
    #    self.assertRaises(Exception, provokeException)

    def testGetUnknownModule(self):
        def provokeException():
            tmp = self.testee.baz
            self.assertRaises(Exception, provokeException)

        try:
            provokeException()
        except Exception, e:
            self.assertEqual("baz not available.\nAvailable modules are: foo, bar", str(e))

    #def testModuleAlreadyRegistered(self):

    #    def provokeException():
    #        self.testee.baz = ModuleMock("moduleC")
    #        self.testee.zab = ModuleMock("moduleC")
    #    self.assertRaises(Exception, provokeException)
# end example
