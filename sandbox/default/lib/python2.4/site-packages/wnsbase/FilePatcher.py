import re
import unittest
import os

class FilePatcher:

    def __init__(self, filename, search, replace, ignoreCase=True):
        if ignoreCase==True:
            self.regexp = re.compile(search, re.IGNORECASE)
        else:
            self.regexp = re.compile(search)

        self.replaceWith = replace
        self.filename = filename
        self.ignoreCase = ignoreCase

    def replaceAll(self):
        f = open(self.filename, 'r')

        output = ""
        for l in f:
            if self.regexp.search(l) != None:
                l = self.regexp.sub(self.replaceWith, l)
            output += l

        # close file
        f.close()

        # re-write new file
        fc = open(self.filename, 'w')
        fc.write(output)
        fc.close()

class FilePatcherTest(unittest.TestCase):

    def setUp(self):
        self.fileContents = "Eins = 3\nZwei = 1\nDrei = 2\n"
        self.filename = "/tmp/__test__filePatcherTest"
        fc = open(self.filename, 'w')
        fc.write(self.fileContents)
        fc.close()

    def testPatch(self):
        fp = FilePatcher(self.filename, "Eins\s=\s+\S+", "Eins = 1\n")
        fp.replaceAll()

        fp = FilePatcher(self.filename, "Zwei\s=\s+\S+", "Zwei = 2\n")
        fp.replaceAll()

        fp = FilePatcher(self.filename, "Drei\s=\s+\S+", "Drei = 3\n")
        fp.replaceAll()

        f = open(self.filename, 'r')
        contents = f.readlines()
        f.close()

        self.assertEqual(len(contents), 3)
        self.assertEqual(contents[0], "Eins = 1\n")
        self.assertEqual(contents[1], "Zwei = 2\n")
        self.assertEqual(contents[2], "Drei = 3\n")
