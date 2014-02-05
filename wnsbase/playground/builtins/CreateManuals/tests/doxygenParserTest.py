import unittest
from wnsbase.playground.builtins.CreateManuals import doxygenParser
class ParserTests(unittest.TestCase):

    def setUp(self):
        self.sample = """/**
@page pageTitle This is some test

And is has some content with \ref references.
@section theSection Also it has some Sections
With content, too.

@subsection theSubSection And finally it also has some subsections.
ContentContentContnet

@section theSection2 Also it has some Sections
It is always only about the content.
@page anotherPage Test with no content
@section anotherSection The last page does not have any content
*/"""

        self.p = doxygenParser.Parser()

    def testNumChildren(self):
        self.p.parseString(self.sample)
        
        self.assertEqual(self.p.root.getNumChildren(), 2)

    def testGetChildNames(self):
        self.p.parseString(self.sample)

        self.assertEqual(self.p.root.getChildNames(), ['pageTitle', 'anotherPage'])
    
    def testGetChildByName(self):
        self.p.parseString(self.sample)

        self.assertEqual(self.p.root.getChildByName('pageTitle').title, 'pageTitle')

        self.assertEqual(self.p.root.getChildByName('pageTitle').niceTitle, 'This is some test')

    