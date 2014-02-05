import re
import copy

class Parser:

    def __init__(self):
        self.root = Child()
        self.root.type = "Root"

        self.currentNode = self.root

    def parseFile(self, filename):
        f = open(filename)
        self.parseString(self._preParse(f.read()), filename)
        f.close()

    def parseString(self, content, filename = "__from_string__"):
        for line in content.split("\n"):
            self.root.parse(line, filename)
        self.root.onDocumentEnd()

    def _preParse(self, string):
        # Only lines between a line that starts with whitespace + /**
        # and a line that ends with + */ must be evaluation

        doxydocstring = ""

        indox = False
        inverbatim = False
        inverbatim = False

        for line in string.split("\n"):
            if line.count('/**') > 0:
                indox = True

            if not inverbatim:
                if line.count('*/') > 0:
                    indox = False

                line = line.rstrip('/ *')
                line = line.lstrip(' *')

            if indox:
                doxydocstring += line + "\n"

            if line.count('@verbatim') > 0:
                inverbatim = True

            if line.count('@endverbatim') > 0:
                inverbatim = False

            if line.count('@code') > 0:
                if indox:
                    doxydocstring += "Hack:LatexWillSkipThisInsteadofTheFirstCodeLine\n"

        return doxydocstring

class Child(object):

    page_re = re.compile(r'\s*@page\s+(\S+)\s+(.*)')
    section_re = re.compile(r'\s*@section\s+(\S+)\s+(.*)')
    subsection_re = re.compile(r'\s*@subsection\S+(\w+)\s+(.*)')
    tagorder = ['Root', 'page', 'section', 'subsection', 'paragraph']

    def __init__(self):

        self.title = None
        self.niceTitle = None
        self._rawContent = None
        self._content = None
        self.orderedChildren = []
        self.currentChild = None
        self.type = None
        self._filenameForDebugOutput = "no file. It has no file. It was automatically created"

    def getContent(self):
        if self._content is None:
            return ''
        else:
            if self._content == "":
                return "\n"

        return self._content

    def setContent(self, content):
        self._content = content

    @property
    def rawContent(self):
        if self._rawContent is None:
            return ''
        else:
            return self._rawContent

    def getNumChildren(self):
        return len(self.orderedChildren)

    def parse(self, line, filenameForDebugOutput):
        self._appendRawContent(line)

        # Do we need to start a new child?
        if self._startsNewChild(line):
            self._insertNewChild(line, filenameForDebugOutput)

        elif self.currentChild is not None:
            self.currentChild.parse(line, filenameForDebugOutput)
        else:
            self._appendContent(line)
    
    def onDocumentEnd(self):
        if self.currentChild is not None:
            self.currentChild.onDocumentEnd()
        self.currentChild = None

    def getChildNames(self):
        names = [c.title for c in self.orderedChildren]
        return names

    def getChildByName(self, name):
 
        for c in self.orderedChildren:
            if c.title == name:
                return c
        
        assert False, "Child %s not found" % name

    def appendChild(self, toBeAdopted):
        if self.type == 'Root':
            assert toBeAdopted.type == "page", "Unsupported nesting %s below %s" % (toBeAdopted.type, self.type)
        if self.type == 'page':
            assert toBeAdopted.type == "section", "Unsupported nesting %s below %s" % (toBeAdopted.type, self.type)
        if self.type == 'section':
            assert toBeAdopted.type == "subsection", "Unsupported nesting %s below %s" % (toBeAdopted.type, self.type)

        assert self.getChildNames().count(toBeAdopted.title) == 0, "Error duplicate key '%s' in child list" % self.toBeAdopted.title

        self.orderedChildren.append(toBeAdopted)

    def removeChild(self, child):
        self.orderedChildren.pop(child)

    def pushDown(self):
        myIndex = Child.tagorder.index(self.type)

        newIndex = myIndex + 1

        if newIndex >= len(Child.tagorder):
            newIndex = len(Child.tagorder)

        self.type = Child.tagorder[newIndex]

        for c in self.orderedChildren:
            c.pushDown()

    def swallow(self, child):
        myChild = copy.deepcopy(child)

        myIndex = Child.tagorder.index(self.type)

        myChildsIndex = Child.tagorder.index(myChild.type)

        while (myIndex >= myChildsIndex) or (myChildsIndex > len(Child.tagorder)):
            myChild.pushDown()

            myChildsIndex = Child.tagorder.index(myChild.type)

        self.appendChild(myChild)
        
        return myChild

    def writeToFile(self, file, release=False):
        if self.type!="Root":
            file.write("@%s %s %s\n" % (self.type, self.title, self.niceTitle))
            if not release:
                file.write("@note This sections linkid is \e %s. It can be found in @verbatim %s @endverbatim\n" % 
                           (self.title, self._filenameForDebugOutput))
            file.write(self.content)

        for c in self.orderedChildren:
            c.writeToFile(file, release)

    def writeAsReSTFile(self, file, release=False):
        if self.type!="Root":
            file.write(self.niceTitle + "\n")
            
            if self.type == "page":
                markup = "="

            if self.type == "section":
                markup = "-"

            if self.type == "subsection":
                markup = "'"

            file.write("%s\n" % (markup * len(self.niceTitle)))

            if not release:
                file.write("\n.. note::\n\n")
                file.write("  This sections linkid is *%s*. It can be found in ``%s``\n" % 
                           (self.title, self._filenameForDebugOutput))
            file.write(self.content + "\n")

        for c in self.orderedChildren:
            c.writeAsReSTFile(file, release)

    def _appendRawContent(self, line):
        # Alway insert content that passes us to raw content
        if self.type == "Root":
            return

        if self._rawContent is None:
            self._rawContent = line
        else:
            self._rawContent += "\n" + line

    def _appendContent(self, line):
        if self.type == "Root":
            return

        if self._content == None:
            self._content = line
        else:
            self._content += "\n" + line

    def _getRegExpAndType(self):
        if self.type == "Root":
            return (Child.page_re, "page")
        elif self.type == "page":
            return (Child.section_re, "section")
        elif self.type == "section":
            return (Child.subsection_re, "subsection")
        else:
            return (None, None)

    def _startsNewChild(self, line):
        (regexp, type) = self._getRegExpAndType()

        if regexp == None:
            return False

        match = regexp.match(line)
        return match is not None

    def _insertNewChild(self, line, filenameForDebugOutput):

        (regexp, childtype) = self._getRegExpAndType()

        match = regexp.match(line)

        if match is not None:
            self.currentChild = Child()
            self.currentChild.type = childtype
            self.currentChild.title = match.group(1)
            self.currentChild.niceTitle = match.group(2)
            self.currentChild._rawContent = line
            self.currentChild._filenameForDebugOutput = filenameForDebugOutput
            assert self.getChildNames().count(self.currentChild.title) == 0, "Error duplicate key '%s' in child list" % self.currentChild.title

            self.orderedChildren.append(self.currentChild)

        else:
            raise "Unexpected Nesting of elements"


    content = property(getContent, setContent)

def createRoot():
    root = Child()
    root.type = "Root"
    return root

def createPage(title, niceTitle):
    page = Child()
    page.type = "page"
    page.title = title
    page.niceTitle = niceTitle
    return page
