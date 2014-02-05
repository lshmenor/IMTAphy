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

""" openwns.evaluation uses the Python generator concept to create new children.
Everytime you call addChildren on a TreeNode or on a TreeNodeSet you need to pass
an instance of generator object that implments the ITreeNodeGenerator interface.

To implement your own generator you need to implement the __call__ method in your
class. When this method is called you must 'yield' your created TreeNodes subsequently.
The contents of the TreeNode must implement the openwns.evaluation.IObservable interface,
which describes how the contents are connected.

"""
 
import openwns.interface
import openwns.probebus
import tree
import wrappers
import statistics

class ITreeNodeGenerator(openwns.interface.Interface):
    """ Interface for a TreeNodeGenerator """
    @openwns.interface.abstractmethod
    def __call__(self, pathname):
        pass

class Accept(ITreeNodeGenerator):
    """ Accept measurements based on the context"""
    def __init__(self, by, ifIn, suffix = ''):
        self.by = by
        self.ifIn = ifIn
        self.suffix = suffix

    def __call__(self, pathname):
        yield tree.TreeNode(wrappers.ProbeBusWrapper(
                openwns.probebus.ContextFilterProbeBus(self.by, self.ifIn),
                self.suffix))

class Equidistant(ITreeNodeGenerator):
    """ Sort measurement in equidistant bins"""
    def __init__(self, by, start, end, step, format):
        self.by = by
        self.start = start
        self.end = end
        self.step = step
        self.format = format

    def __call__(self, pathname):
        for num in xrange(self.start, self.end + 1, self.step):
            if self.format == '':
                suffix = ''
            else:
                suffix = self.format % num

            yield tree.TreeNode(wrappers.ProbeBusWrapper(
                    openwns.probebus.ContextFilterProbeBus(self.by, range(num, num + self.step)),
                    suffix))

class Enumerated(ITreeNodeGenerator):
    """ Sort by context and rename numeric context values"""
    def __init__(self, by, keys, names, format):
        self.by = by

        assert len(keys) == len(names)

        self.keys = keys
        self.names = names
        self.format = format

    def __call__(self, pathname):
        for i in range(len(self.keys)):
            if self.format == '':
                suffix = ''
            else:
                suffix = self.format % self.names[i]
            yield tree.TreeNode(wrappers.ProbeBusWrapper(
                    openwns.probebus.ContextFilterProbeBus(self.by, [self.keys[i]]),
                    suffix))

class ProbeBusCreator(ITreeNodeGenerator):

    def __init__(self, classname, **kwargs):
        self.classname = classname
        self.kwargs = kwargs

    def __call__(self, pathname):
        if len(self.kwargs) > 0:
            c = self.classname(self.kwargs)
        else:
            c = self.classname()

        if 'setFilename' in dir(c):
            if callable(c.setFilename):
                c.setFilename(pathname)
        yield tree.TreeNode(wrappers.ProbeBusWrapper(c, ''))

class Separate(ITreeNodeGenerator):
    """ Sort by context and take contextProvider output as names"""
    def __init__(self, by, forAll, format):
        self.by = by
        self.forAll = forAll
        self.format = format

    def __call__(self, pathname):
        for id in self.forAll:
            if self.format == '':
                suffix = ''
            else:
                suffix = self.format % id
            yield tree.TreeNode(wrappers.ProbeBusWrapper(
                    openwns.probebus.ContextFilterProbeBus(self.by, [ id ]),
                    suffix))

class Instantiate(ITreeNodeGenerator):

    def __init__(self, prototype, *args, **kwargs):
        self.prototype = prototype
        self.args = args
        self.kwargs = kwargs

    def __call__(self, pathname):
        clone = self.prototype(*self.args, **self.kwargs)

        if isinstance(clone, openwns.probebus.NeedsFilename):
            clone.setFilename(pathname)

        yield tree.TreeNode(wrappers.ProbeBusWrapper(clone, ''))

class SettlingTimeGuard(ITreeNodeGenerator):
    """ Accept measurements based on the context"""
    def __init__(self, settlingTime):
        self.settlingTime = settlingTime

    def __call__(self, pathname):
            yield tree.TreeNode(wrappers.ProbeBusWrapper(
                    openwns.probebus.SettlingTimeGuardProbeBus(self.settlingTime),
                    ''))

class PDF(ITreeNodeGenerator):

    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs

    def __call__(self, pathname):
        pdf = statistics.PDFEval(*self.args, **self.kwargs)

        pb = openwns.probebus.StatEvalProbeBus(pathname + '_PDF.dat', pdf)

        yield tree.TreeNode(wrappers.ProbeBusWrapper(pb, ''))

class DLRE(ITreeNodeGenerator):

    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs

    def __call__(self, pathname):
        dlre = statistics.DLREEval(*self.args, **self.kwargs)

        pb = openwns.probebus.StatEvalProbeBus(pathname + '_' + dlre.typename + '.dat', dlre)

        yield tree.TreeNode(wrappers.ProbeBusWrapper(pb, ''))

class TextTrace(ITreeNodeGenerator):

    def __init__(self, key, *args, **kwargs):
        self.key = key
        self.args = args
        self.kwargs = kwargs

    def __call__(self, pathname):
        t = openwns.probebus.TextProbeBus(pathname, self.key, *self.args, **self.kwargs)
        t.prependSimTimeFlag = False
        yield tree.TreeNode(wrappers.ProbeBusWrapper(t, ''))

class JSONTrace(ITreeNodeGenerator):
    def __init__(self, key, *args, **kwargs):
        self.key = key
        self.args = args
        self.kwargs = kwargs

    def __call__(self, pathname):
        t = openwns.probebus.JSONProbeBus(pathname, self.key, *self.args, **self.kwargs)
        t.prependSimTimeFlag = False
        yield tree.TreeNode(wrappers.ProbeBusWrapper(t, ''))

class TimeSeries(ITreeNodeGenerator):

    def __init__(self, format = "fixed", timePrecision = 7, 
                 valuePrecision = 7, name = "no name available", 
                 description = "no description available",
                 contextKeys = []):
      
        self.format = format
        self.timePrecision = timePrecision
        self.valuePrecision = valuePrecision
        self.name = name
        self.description = description
        self.contextKeys = contextKeys

    def __call__(self, pathname):
        pb = openwns.probebus.TimeSeriesProbeBus(
            pathname + "_TimeSeries.dat", self.format, 
            self.timePrecision, self.valuePrecision, 
            self.name, self.description, self.contextKeys)

        yield tree.TreeNode(wrappers.ProbeBusWrapper(pb, ''))

class Moments(ITreeNodeGenerator):

    def __init__(self, format = "fixed", name = "no name available", 
                 description = "no description available",
                 scalingFactor = 1.0):
        self.format = format
        self.name = name
        self.description = description
        self.scalingFactor = scalingFactor

    def __call__(self, pathname):
        momentseval = statistics.MomentsEval(format = self.format, 
                                             name = self.name,
                                             description = self.description,
                                             scalingFactor = self.scalingFactor)
        
        pb = openwns.probebus.StatEvalProbeBus(pathname + '_Moments.dat', momentseval)

        yield tree.TreeNode(wrappers.ProbeBusWrapper(pb, ''))

class Plot2D(ITreeNodeGenerator):

    def __init__(self, 
                    xDataKey, 
                    minX, 
                    maxX, 
                    resolution = None, 
                    statEvals = ['mean'], 
                    name = None, 
                    description = None):
        
        self.xDataKey = xDataKey
        self.minX = minX
        self.maxX = maxX
        if resolution == None:
            resolution = maxX - minS
        else:        
            self.resolution = resolution
            
        self.statEvals = statEvals
        
        if name == None:
            self.name = "y over " + xDataKey
        else:    
            self.name = name
            
        if description == None:
            self.description = "y over " + xDataKey
        else:          
            self.description = description        

    def __call__(self, pathname):
        assert len(self.statEvals) >= 1, "You must provide at least one statistical Evaluation"
        
        momentseval = statistics.MomentsEval(format = "fixed", 
                            name = self.name,
                            description = self.description,
                            scalingFactor = 1.0)
        
        root = tree.TreeNode(wrappers.ProbeBusWrapper(
            openwns.probebus.StatEvalProbeBus(
                pathname + "_" + self.statEvals[0] + '_Log.dat', momentseval), ''))
        
        leaf = root
        
        for statE in self.statEvals[1:]:
            momentseval = statistics.MomentsEval(format = "fixed", 
                                                name = self.name,
                                                description = self.description,
                                                scalingFactor = 1.0)
                                             
            pb = tree.TreeNode(wrappers.ProbeBusWrapper(
                openwns.probebus.StatEvalProbeBus(pathname + "_" + statE + '_Log.dat', momentseval), ''))
            leaf.addChild(pb)
            leaf = pb
          
        tableNode = Table(axis1 = self.xDataKey, 
            minValue1 = self.minX, 
            maxValue1 = self.maxX, 
            resolution1 = self.resolution, 
            values = self.statEvals,
            formats = ['LogEvalReadable'])
            
        for creation in tableNode(pathname):
            leaf.addChild(creation)  

        yield root

class Logger(ITreeNodeGenerator):

    def __init__(self):
        pass

    def __call__(self, pathname):
        yield tree.TreeNode(wrappers.ProbeBusWrapper(openwns.probebus.LoggingProbeBus(probeName=pathname), ""))

class Table(ITreeNodeGenerator):

    def __init__(self, **kwargs):
        assert kwargs.has_key("values"), "You must sepecify at least one value"
        assert kwargs.has_key("formats"), "You must specify at least one format"

        self.values = kwargs.pop("values")
        self.formats = kwargs.pop("formats")
        self.parseAxes(kwargs)

    def parseAxes(self, paramdict):

        numAxes = 1
        self.tabPars = []
        assert paramdict.has_key("axis1"), "You need to specify at least one axis. Use (axisN, minValueN, maxValueN, resolutionN) with an integer N starting at 1"
        while(1):
            if paramdict.has_key("axis%d" % numAxes):
                assert paramdict.has_key("minValue%d" % numAxes), "You need to specify all parameters for an axis (axisN, minValueN, maxValueN, resolutionN)"
                assert paramdict.has_key("maxValue%d" % numAxes), "You need to specify all parameters for an axis (axisN, minValueN, maxValueN, resolutionN)"
                assert paramdict.has_key("resolution%d" % numAxes), "You need to specify all parameters for an axis (axisN, minValueN, maxValueN, resolutionN)"

                self.tabPars.append(openwns.probebus.TabPar(paramdict["axis%d" % numAxes],
                                                            paramdict["minValue%d" % numAxes],
                                                            paramdict["maxValue%d" % numAxes],
                                                            paramdict["resolution%d" % numAxes]))
                numAxes += 1
            else:
                break

    def __call__(self, pathname):
        pb = openwns.probebus.TableProbeBus(self.tabPars, pathname, self.values, self.formats)
        yield tree.TreeNode(wrappers.ProbeBusWrapper(pb, ''))
