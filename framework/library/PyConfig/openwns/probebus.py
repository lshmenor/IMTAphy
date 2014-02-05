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

import openwns
import openwns.logger
from openwns.pyconfig import attrsetter

import openwns.interface

class NeedsFilename(openwns.interface.Interface):

    @openwns.interface.abstractmethod
    def setFilename(self, filename):
        pass

class MeasurementSource(object):

    def __init__(self):
        object.__init__(self)
        self.observers = []

    def addObserver(self, probeBus):
        self.observers.append(probeBus)
        return probeBus

class ProbeBus(MeasurementSource):

    def __init__(self):
        MeasurementSource.__init__(self)

    def observe(self, probeBus):
        probeBus.addObserver(self)
        return probeBus

class ProbeBusRegistry(object):

    def __init__(self):
        super(ProbeBusRegistry, self).__init__()
        self.measurementSources = {}
        self.logger = openwns.logger.Logger("WNS", "ProbeBusRegistry", True)

    def getMeasurementSource(self, probeBusID):
        if not self.measurementSources.has_key(probeBusID):
            self.measurementSources[probeBusID] = MeasurementSource()

        return self.measurementSources[probeBusID]

    def removeMeasurementSource(self, probeBusID):
        self.measurementSources.pop(probeBusID)

    def getMeasurementSources(self):
        return self.measurementSources

class PassThroughProbeBus(ProbeBus):
        """ The PassThroughProbeBus always accepts and always forwards. """
        nameInFactory = "PassThroughProbeBus"

        def __init__(self):
                ProbeBus.__init__(self)

class SettlingTimeGuardProbeBus(ProbeBus):
        """ The SettlingTimeGuardProbeBus only accepts if the global settling time (transient phase)
        has elapsed"""
        nameInFactory = "SettlingTimeGuardProbeBus"

        def __init__(self, settlingTime):
                ProbeBus.__init__(self)
                self.settlingTime = settlingTime

class LoggingProbeBus(ProbeBus):
        """ The LoggingProbeBus always accepts and logs the message to the logging subsystem.
        """
        nameInFactory = "LoggingProbeBus"

        def __init__(self, probeName='', parentLogger=None):
            ProbeBus.__init__(self)
            if len(probeName) > 0:
                probeName = '.' + probeName
                self.logger = openwns.logger.Logger("WNS", "LoggingProbeBus"+probeName, True, parentLogger)

class PythonProbeBus(ProbeBus):
        """ Use the PythonProbeBus to do all your probing work in python. Specify what to do
        in accepts, onMeasurement, output from within your configuration file."""
        nameInFactory = "PythonProbeBus"

        def _dummyOnMeasurement(timestamp, value, reg):
                pass

        def _dummyOutput():
                pass

        def __init__(self, acceptsFunction, onMeasurementFunction = _dummyOnMeasurement, outputFunction = _dummyOutput):
                ProbeBus.__init__(self)
                self.accepts = acceptsFunction
                self.onMeasurement = onMeasurementFunction
                self.output = outputFunction
                self.reportErrors = True

class TimeWindowProbeBus(ProbeBus):
        """ Only accepts for a certain time window given by start and end time"""

        nameInFactory = "TimeWindowProbeBus"

        def __init__(self, start, end):
                ProbeBus.__init__(self)
                self.start = start
                self.end = end

class TimeSeriesProbeBus(ProbeBus):
        """ The LogEval ProbeBus always accepts and logs the values into a file.
        """
        nameInFactory = "TimeSeriesProbeBus"

        outputFilename = None
        format = None
        timePrecision = None
        valuePrecision = None
        name = None
        description = None
        contextKeys = None

        def __init__(self, outputFilename, format, timePrecision, valuePrecision, name, desc, contextKeys):
            ProbeBus.__init__(self)
            self.outputFilename = outputFilename
            self.format = format
            self.timePrecision = timePrecision
            self.valuePrecision = valuePrecision
            self.name = name
            self.description = desc
            self.contextKeys = contextKeys

class ContextFilterProbeBus(ProbeBus):
        nameInFactory = "ContextFilterProbeBus"
        idName = None
        idValues = None

        def __init__(self, _idName, _idValues, _outputName = None):
                ProbeBus.__init__(self)
                self.idName = _idName
                self.idValues = _idValues

class ConstantContextProvider(object):

        __plugin__ = "wns.ProbeBus.ConstantContextProvider"
        """ Name in the static factory """

        key = None
        """ The name of the context """

        value = None
        """ A constant integer value """

        def __init__(self, key, value):
                super(ConstantContextProvider, self).__init__()
                self.key = key
                self.value = value

class StatEvalProbeBus(ProbeBus):

        nameInFactory = "StatEvalProbeBus"

        statEval = None
        appendFlag = None

        def __init__(self, outputFilename, statEvalConfig):
            ProbeBus.__init__(self)
            self.outputFilename = outputFilename
            self.statEval = statEvalConfig
            if (statEvalConfig.appendFlag == None):
                self.appendFlag = False
            else:
                self.appendFlag = statEvalConfig.appendFlag

class TabPar:
    """
    Helper Class to configure the TableProbeBus.

    Configure one of these for each dimension of your table.
    Parameters:

    idName: the name in the IDregistry/Context under which the
            value for this axis should be searched
    minimum: min value of the axis
    maximum: max value of the axis
    resolution: number of equidistant intervals into which the
                range from min to max will be divided. Note that
            the maximum value will be counted into the last interval
    """
    idName = None
    minimum = None
    maximum = None
    resolution = None
    def __init__(self, idName, minimum, maximum, resolution):
        self.idName = idName
        self.minimum = minimum
        self.maximum = maximum
        self.resolution = resolution

class TableProbeBus(ProbeBus):
    """
    The TableProbeBus consumes measurement values and sorts them
    into n-dimensional tables of statistical evaluation objects.

    Parameters:

    axisParams: list of TabPar objecst, one for each dimension of the desired table
    outputFilename: base name of the output files produced by the TableProbeBus
    evals: list of strings with the requested statistics, possible values are:
           'mean', 'variance', 'relativeVariance', 'coeffOfVariation', 'M2', 'M3', 'Z3',
           'skewness', 'deviation', 'relativeDeviation', 'trials', 'min', 'max'
    formats: list of strings with the requested output formats, possible values are:
           'HumanReadable', 'PythonReadable', 'MatlabReadable', 'MatlabReadableSparse'

    """
    nameInFactory = "TableProbeBus"
    axisParams = None
    outputFilename = None
    evals   = None
    formats = None

    def __init__(self, axisParams, outputFilename, evals = ['mean'], formats = ['HumanReadable']):
        ProbeBus.__init__(self)
        self.axisParams = axisParams
        self.outputFilename = outputFilename
        self.evals = list(set(evals)) # filter out potential duplicates
        self.formats = list(set(formats)) # filter out potential duplicates

class TextProbeBus(ProbeBus):
    """
    Wrapper for a ProbeText StatEval
    """
    nameInFactory = "TextProbeBus"
    key = None
    outputFilename = None
    evalConfig = None
    writeHeader = None
    prependSimTimeFlag = None
    simTimePrecision = None
    simTimeWidth = None
    skipInterval = None

    def __init__(self, outputFilename, key, description):
        ProbeBus.__init__(self)
        self.key = key
        self.outputFilename = outputFilename
        self.writeHeader = True
        self.prependSimTimeFlag = True
        self.simTimePrecision = 7
        self.simTimeWidth = 10
        self.skipInterval = 0
        self.isJSON = False

class JSONProbeBus(TextProbeBus):

    def __init__(self, name, key, description):
        TextProbeBus.__init__(self, name, key, description)
        self.isJSON = True