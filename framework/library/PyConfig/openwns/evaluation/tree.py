###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2007
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 16, D-52074 Aachen, Germany
# phone: ++49-241-80-27910,
# fax: ++49-241-80-22242
# email: info@openwns.org
# www: http://www.openwns.org
# _____________________________________________________________________________
#
# openWNS is free software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License version 2 as published by the
# Free Software Foundation;
#
# openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

class TreeNodeSet(set):

    def appendChildren(self, treenodegenerator):
        createdNodes = TreeNodeSet()

        for node in self:
            for creation in treenodegenerator(node.getConcatenatedShortDescriptors()):
                node.addChild(creation)
                createdNodes.add(creation)

        return createdNodes

    def getLeafs(self):
        leafs = TreeNodeSet()

        for node in self:
            leafs = leafs.union(node.getLeafs())
        return leafs

    def tag(self, tagname):
        for node in self:
            node.tag(tagname)

class TreeNode:
    """ detail """
    def __init__(self, content):
        self._content = content

        self._parent = None

        self._children = set()

        self._lockAddChild = False

        self.tags = set()

    def getContent(self):
        return self._content

    def setContent(self, content):
        self._content = content

    def setParent(self, node):
        if self._parent is not None:
            self._parent.removeChild(self)

        self._parent = node

        if self.hasParent():
            self.getParent().addChild(self)

    def getParent(self):
        return self._parent

    def hasParent(self):
        return self._parent is not None

    def removeChild(self, child):
        try:
            self._children.remove(child)

            if child.getParent() != None:
                child.setParent(None)
        except KeyError:
            # It is alreade gone. Very fine.
            pass

    def addChild(self, child):
        if not self._lockAddChild:
            try:
                self._lockAddChild = True
                self._children.add(child)

                self.getContent().addObserver(child.getContent())

                if child.getParent() != self:
                    child.setParent(self)
            finally:
                self._lockAddChild = False

    def children(self):
        for child in self._children:
            yield child

    def getChildren(self):
        result = TreeNodeSet()
        for child in self.children():
            result.add(child)
        return result

    def hasChildren(self):
        return len(self._children) > 0

    def leafsIterator(self):
        if not self.hasChildren():
            yield self
        else:
            for child in self.children():
                for leaf in child.getLeafs():
                    yield leaf

    def getLeafs(self):
        result = TreeNodeSet()
        for leaf in self.leafsIterator():
            result.add(leaf)
        return result

    def walkFromRoot(self):
        if self.hasParent():
            for node in self.getParent().walkFromRoot():
                yield node
        yield self

    def appendChildren(self, treenodegenerator):
        nodeset = TreeNodeSet()
        nodeset.add(self)
        return nodeset.appendChildren(treenodegenerator)

    def tag(self, tag):
        self.tags.add(tag)

    def hasTag(self, theTag):
        for aTag in self.tags:
            if aTag == theTag:
                return True
        return False

    def allNodesIterator(self):
        if self.hasChildren():
            for child in self.children():
                for node in child.allNodesIterator():
                    yield node
        else:
            yield self

    def getNodesByTag(self, tag):
        result = TreeNodeSet()

        for node in self.allNodesIterator():
            if node.hasTag(tag):
                result.add(node)

        return result

    def getConcatenatedShortDescriptors(self, separator='_'):
        filename = []
        for node in self.walkFromRoot():
            c = node.getContent()
            if c is not None:
                filename.append(c.getShortDescriptor())

        filename = [it for it in filename if it!='']

        return separator.join(filename)
