#!/usr/bin/env python

import os
import pywns.TableParser

class ProbeTypeError(Exception):
    """
    Raised if not a probe of desired type
    """
    pass


class Probe(object):
    """
    Base class to read probes from files
    """

    valueNames = ["minimum", "maximum", "trials", "mean", "variance", "relativeVariance",
                  "standardDeviation", "relativeStandardDeviation", "skewness",
                  "moment2", "moment3"]

    def __init__(self, probeType, filename):
        """
        Raises an error if file is not available or of desired type
        """

        self.filename = filename
        self.absFilename = os.path.abspath(self.filename)

        self.__items = self.parseFile(self.absFilename)

        self.dirname, self.filenameWithoutDir = os.path.split(self.filename)
        if self.dirname == "":
            self.dirname = "./"

        # check if is probe file of desired type
	
	# DISABLED: Does not work during LogEval / Moments / TimeSeries migration
	
        # evaluation = self.getValue("Evaluation")
        # if not probeType in evaluation:
        #    raise ProbeTypeError(str(self) + " tried to read a probe of type: " + probeType)

        # This name is the name provided by the probe itself. It may
        # be not unique for probe files being kept in a directory
        # since it is a matter of configuration ...
        self.name                      = self.getValue("Name")
        # This name is built from the filename and therfor unique, at
        # least for all probes in one directory
        altName, ext = os.path.splitext(self.filenameWithoutDir)
        self.altName                   = altName
        self.description               = self.getValue("Description")
        self.minimum                   = self.getValue("Minimum")
        self.maximum                   = self.getValue("Maximum")
        self.trials                    = self.getValue("Trials")
        self.mean                      = self.getValue("Mean")
        self.variance                  = self.getValue("Variance")
        self.relativeVariance          = self.getValue("Relative variance")
        self.standardDeviation         = self.getValue("Standard deviation")
        self.relativeStandardDeviation = self.getValue("Relative standard deviation")
        self.skewness                  = self.getValue("Skewness")
        self.moment2                   = self.getValue("2nd moment")
        self.moment3                   = self.getValue("3rd moment")
        self.sumOfAllValues            = self.getValue("Sum of all values")
        self.sumOfAllValuesSquare      = self.getValue("(Sum of all values)^2")
        self.sumOfAllValuesCubic       = self.getValue("(Sum of all values)^3")



    def parseFile(fileName):
        """ parses self.filename

        searches for the pattern: '# key: value', returns a dict with
        the found keys and values
        """
        items = {}
        for line in file(fileName):
            # strip spaces and newlines
            line = line.strip()
            if line.startswith("#"):
                if ":" in line:
                    # strip spaces and "#" at the beginning of the line
                    line = line.lstrip("# ")
                    key, value = line.split(":")
                    # strip spaces and new lines around key and value
                    key = key.strip()
                    value = value.strip()
                    if not items.has_key(key):
                        items[key] = value
                    else:
                        raise Exception("Tried to add '" + key + "' but this was already found.")
            else:
                # when no "#" is found, we can stop parsing
                break
        return items
    parseFile = staticmethod(parseFile)


    def getValue(self, parameter):
        """ Try to find the value for 'parameter'
        """
        value = self.__items[parameter]
        # automatic conversion
        # try int, float, string (in this order)
        try:
            return int(value)
        except ValueError:
            try:
                return float(value)
            except ValueError:
                return value


    # @staticmethod (this syntax works only for python >= 2.4)
    def readProbes(probeType, probeClass, dirname):
        result = {}
        for ff in os.listdir(dirname):
            filename = os.path.join(dirname, ff)
            if os.path.isfile(filename):
                if filename.endswith(probeType):
                    try:
                        probe = probeClass(filename)
                        result[probe.filenameWithoutDir] = probe
                    except ProbeTypeError, e:
                        pass
        return result
    readProbes = staticmethod(readProbes)


# Moments probe specific part
class MomentsProbe(Probe):
    fileNameSig = "_Moments.dat"

    probeType = "Moments"

    def __init__(self, filename):
        super(MomentsProbe, self).__init__("Moments", filename)

    # @staticmethod (this syntax works only for python >= 2.4)
    def readProbes(dirname):
        return Probe.readProbes(MomentsProbe.fileNameSig, MomentsProbe, dirname)
    readProbes = staticmethod(readProbes)


# PDF probe specific part

class PDFHistogramEntry(object):

    __slots__ = ["x", "cdf", "ccdf", "pdf"]

    def __init__(self, listOfValues):
        self.x = float(listOfValues[0])
        self.cdf = float(listOfValues[1])
        self.ccdf = float(listOfValues[2])
        self.pdf = float(listOfValues[3])


class PDFProbe(Probe):

    fileNameSig = "_PDF.dat"
    valueNames = ["P01","P05","P50","P95","P99","minX", "maxX", "numberOfBins", "underflows", "overflows"] + Probe.valueNames
    histogram = None

    probeType = "PDF"

    def __init__(self, filename):
        super(PDFProbe, self).__init__("PDF", filename)

        # read Percentiles
        self.P01 = self.getValue("P01")
        self.P05 = self.getValue("P05")
        self.P50 = self.getValue("P50")
        self.P95 = self.getValue("P95")
        self.P99 = self.getValue("P99")

        # These parameters have not been measured but configured ...
        self.minX           = self.getValue("Left border of x-axis")
        self.maxX           = self.getValue("Right border of x-axis")
        self.numberOfBins   = self.getValue("Resolution of x-axis")
        self.underflows     = self.getValue("Underflows")
        self.overflows      = self.getValue("Overflows")

        self.__histogram = []
        self.__histogramRead = False

    def __getHistogram(self):
        if self.__histogramRead == False:
            self.__histogram = []
            for line in file(self.absFilename):
                if not line.startswith('#'):
                    self.__histogram.append(PDFHistogramEntry(line.split()))
            self.__histogramRead = True
        return self.__histogram

    histogram = property(__getHistogram)

    # @staticmethod (this syntax works only for python >= 2.4)
    def readProbes(dirname):
        return Probe.readProbes(PDFProbe.fileNameSig, PDFProbe, dirname)
    readProbes = staticmethod(readProbes)

    def __getPureHistogram(self):
        # actually there is one bin more than stated in numberOfBins
        if len(self.histogram) == self.numberOfBins + 3:
            # underflows and overflows
            return self.histogram[1:self.numberOfBins + 1]
        elif len(self.histogram) == self.numberOfBins + 2:
            # underflows or overflows
            if self.overflows > 0:
                # overflows
                return self.histogram[:self.numberOfBins + 1]
            elif self.underflows > 0:
                # underflows
                return self.histogram[1:self.numberOfBins + 2]
            else:
                raise "Did not expect to reach this line"
        else:
            # everything was fine already
            return self.histogram

    pureHistogram = property(__getPureHistogram)


class TimeSeriesProbe(object):
    fileNameSig = "_TimeSeries.dat"
    valueNames = []

    filename = None
    filenameWithoutDir = None
    name = None
    entries = None

    probeType = "TimeSeries"

    def __init__(self, filename):
        self.filename = filename
        self.dirname, self.filenameWithoutDir = os.path.split(self.filename)
        if self.dirname == "":
            self.dirname = "./"

        # Parse the file
        items = Probe.parseFile(self.filename)

        self.altName            = self.filenameWithoutDir.rsplit('_', 1)[0]
        self.name               = items["Name"]
        self.description        = items["Description"]
        
        self.entries = []
        for line in file(self.filename):
            if not line.startswith('#'):
                self.entries.append(LogEvalEntry(line.split()))

    # @staticmethod (this syntax works only for python >= 2.4)
    def readProbes(dirname):
        result = Probe.readProbes(TimeSeriesProbe.fileNameSig, TimeSeriesProbe, dirname)
        return result
    readProbes = staticmethod(readProbes)

# LogEval probe specific part

class LogEvalEntry(object):

    __slots__ = ["x", "y"]

    def __init__(self, listOfValues):
        self.x = float(listOfValues[0])
        self.y = float(listOfValues[1])


class LogEvalProbe(Probe):

    fileNameSig = "_Log.dat"
    valueNames = Probe.valueNames
    entries = None
    readAllValues = True
    filenameEntries = None

    probeType = "LogEval"

    def __init__(self, filename):
        super(LogEvalProbe, self).__init__("LogEval", filename)

        splitFilename = filename.split(".")
        splitFilename[-2] += ".log"
        self.filenameEntries = str(".").join(splitFilename)

        # In the renovated LogEval Probe, the header and the data are in one and the same file
        # TODO: fileNameEntries can be removed when PDataBase/SortingCriterion are abandoned
        if not os.path.exists(self.filenameEntries):
            self.filenameEntries = filename

        self.__entries = []
        self.__entriesRead = False

    def __getEntries(self):
        if not self.readAllValues:
            return []

        if self.__entriesRead == False:
            self.__entries = []
            for line in file(self.filenameEntries):
                if not line.startswith('#'):
                    self.__entries.append(LogEvalEntry(line.split()))
            self.__entriesRead = True
	    
        return self.__entries

    entries = property(__getEntries)

    # @staticmethod (this syntax works only for python >= 2.4)
    def readProbes(dirname):
        return Probe.readProbes(LogEvalProbe.fileNameSig, LogEvalProbe, dirname)
    readProbes = staticmethod(readProbes)


class BatchMeansHistogramEntry:

    def __init__(self, listOfValues):
        self.x = float(listOfValues[1])
        self.cdf = float(listOfValues[0])
        self.pdf = float(listOfValues[3])
        self.relativeError = float(listOfValues[2])
        self.confidence = float(listOfValues[4])
        if len(listOfValues) > 5:
            self.numberOfTrialsPerInterval = int(listOfValues[5])
        else:
            self.numberOfTrialsPerInterval = 0


class BatchMeansProbe(Probe):

    fileNameSig = "_BaM.dat"
    valueNames = ["lowerBorder", "upperBorder", "numberOfIntervals", "intervalSize",
                  "sizeOfGroups", "maximumRelativeError", "evaluatedGroups", "underflows",
                  "overflows", "meanBm", "confidenceOfMeanAbsolute", "confidenceOfMeanPercent",
                  "relativeErrorMean", "varianceBm", "confidenceOfVarianceAbsolute", "confidenceOfVariancePercent",
                  "relativeErrorVariance", "sigma", "firstOrderCorrelationCoefficient"] + Probe.valueNames
    histogram = None

    probeType = "BatchMeans"

    def __init__(self, filename):
        super(BatchMeansProbe, self).__init__("BatchMeans", filename)

        self.lowerBorder                      = self.getValue("lower border")
        self.upperBorder                      = self.getValue("upper border")
        self.numberOfIntervals                = self.getValue("number of intervals")
        self.intervalSize                     = self.getValue("interval size")
        self.sizeOfGroups                     = self.getValue("size of groups")
        self.maximumRelativeError             = self.getValue("maximum relative error [%]")
        self.evaluatedGroups                  = self.getValue("evaluated groups")
        self.underflows                       = self.getValue("Underflows")
        self.overflows                        = self.getValue("Overflows")
        self.meanBm                           = self.getValue("mean (BM version)")
        self.confidenceOfMeanAbsolute         = self.getValue("confidence of mean absolute [+-]")
        self.confidenceOfMeanPercent          = self.getValue("confidence of mean [%]")
        self.relativeErrorMean                = self.getValue("relative error (Bayes Error)")
        self.varianceBm                       = self.getValue("variance (BM version)")
        self.confidenceOfVarianceAbsolute     = self.getValue("confidence of variance absolute [+-]")
        self.confidenceOfVariancePercent      = self.getValue("confidence of variance [%]")
        self.relativeErrorVariance            = self.getValue("relative error")
        self.sigma                            = self.getValue("sigma")
        self.firstOrderCorrelationCoefficient = self.getValue("1st order correlation coefficient")

        # read x, CDF, PDF, relative error, confidence, number of trials
        self.histogram = []
        for line in file(self.absFilename):
            if not line.startswith("#"):
                self.histogram.append(BatchMeansHistogramEntry(line.split()))


    # @staticmethod (this syntax works only for python >= 2.4)
    def readProbes(dirname):
        return Probe.readProbes(BatchMeansProbe.fileNameSig, BatchMeansProbe, dirname)
    readProbes = staticmethod(readProbes)


class LreHistogramEntry(object):

    def __init__(self, listOfValues):
        self.ordinate = float(listOfValues[0])
        self.abscissa = float(listOfValues[1])
        if listOfValues[2] == 'not_available':
            self.relativeError = float('nan')
        else:
            self.relativeError = float(listOfValues[2])
        if listOfValues[3] == 'not_available':
            self.meanLocalCorrelationCoefficient = float('nan')
        else:
            self.meanLocalCorrelationCoefficient = float(listOfValues[3])
        if listOfValues[4] == 'not_available':
            self.deviationFromMeanLocalCC = float('nan')
        else:
            self.deviationFromMeanLocalCC = float(listOfValues[4])
        self.numberOfTrialsPerInterval = int(listOfValues[5])
        if listOfValues[6] == 'not_available':
            self.numberOfTransitionsPerInterval = float('nan')
        else:
            self.numberOfTransitionsPerInterval = int(listOfValues[6])
        self.relativeErrorWithinLimit = listOfValues[7]


class LreProbe(Probe):

    fileNameSigs = ["_LREF.dat",
                    "_LREF_pf.dat",
                    "_LREG.dat",
                    "_LREG_pf.dat"]

    valueNames = ["lreType", "maximumRelativeError", "fMax", "fMin", "scaling",
                  "maximumNumberOfTrialsPerLevel", "rhoN60", "rhoN50",
                  "rhoN40", "rhoN30", "rhoN20", "rhoN10", "rho00",
                  "rhoP25", "rhoP50", "rhoP75", "rhoP90", "rhoP95", "rhoP99",
                  "peakNumberOfSortingElements", "resultIndexOfCurrentLevel", "numberOfLevels",
                  "relativeErrorMean", "relativeErrorVariance", "relativeErrorStandardDeviation",
                  "meanLocalCorrelationCoefficientMean", "meanLocalCorrelationCoefficientVariance",
                  "meanLocalCorrelationCoefficientStandardDeviation", "numberOfTrialsPerIntervalMean",
                  "numberOfTrialsPerIntervalVariance", "numberOfTrialsPerIntervalStandardDeviation",
                  "numberOfTransitionsPerIntervalMean", "numberOfTransitionsPerIntervalVariance",
                  "numberOfTransitionsPerIntervalStandardDeviation"] + Probe.valueNames

    histogram = None

    probeType = "LRE"

    def __init__(self, filename):
        super(LreProbe, self).__init__("LRE", filename)

        self.lreType                                          = self.getValue("Evaluation")
        self.maximumRelativeError                             = self.getValue("Maximum relative error [%]")
        self.fMax                                             = self.getValue("F max")
        self.fMin                                             = self.getValue("F min")
        self.scaling                                          = self.getValue("Scaling")
        self.maximumNumberOfTrialsPerLevel                    = self.getValue("Maximum number of trials per level")
        self.rhoN60                                           = self.getValue("correlated (rho = -0.60)")
        self.rhoN50                                           = self.getValue("correlated (rho = -0.50)")
        self.rhoN40                                           = self.getValue("correlated (rho = -0.40)")
        self.rhoN30                                           = self.getValue("correlated (rho = -0.30)")
        self.rhoN20                                           = self.getValue("correlated (rho = -0.20)")
        self.rhoN10                                           = self.getValue("correlated (rho = -0.10)")
        self.rho00                                            = self.getValue("uncorrelated (rho =  0.00)")
        self.rhoP25                                           = self.getValue("correlated (rho = +0.25)")
        self.rhoP50                                           = self.getValue("correlated (rho = +0.50)")
        self.rhoP75                                           = self.getValue("correlated (rho = +0.75)")
        self.rhoP90                                           = self.getValue("correlated (rho = +0.90)")
        self.rhoP95                                           = self.getValue("correlated (rho = +0.95)")
        self.rhoP99                                           = self.getValue("correlated (rho = +0.99)")
        self.peakNumberOfSortingElements                      = self.getValue("Peak number of sorting mem. elems.")
        self.resultIndexOfCurrentLevel                        = self.getValue("Result memory index of current level")
        self.numberOfLevels                                   = self.getValue("Number of levels")
        self.relativeErrorMean                                = self.getValue("Relative error (Mean)")
        self.relativeErrorVariance                            = self.getValue("Relative error (Variance)")
        self.relativeErrorStandardDeviation                   = self.getValue("Relative error (Standard deviation)")
        self.meanLocalCorrelationCoefficientMean              = self.getValue("Mean local correlation coefficient (Mean)")
        self.meanLocalCorrelationCoefficientVariance          = self.getValue("Mean local correlation coefficient (Variance)")
        self.meanLocalCorrelationCoefficientStandardDeviation = self.getValue("Mean local correlation coefficient (Standard deviation)")
        self.deviationFromMeanLocalCCMean                     = self.getValue("Deviation from mean local c.c.(Mean)")
        self.deviationFromMeanLocalCCVariance                 = self.getValue("Deviation from mean local c.c.(Variance)")
        self.deviationFromMeanLocalCCStandardDeviation        = self.getValue("Deviation from mean local c.c.(Standard deviation)")
        self.numberOfTrialsPerIntervalMean                    = self.getValue("Number of trials per interval (Mean)")
        self.numberOfTrialsPerIntervalVariance                = self.getValue("Number of trials per interval (Variance)")
        self.numberOfTrialsPerIntervalStandardDeviation       = self.getValue("Number of trials per interval (Standard deviation)")
        self.numberOfTransitionsPerIntervalMean               = self.getValue("Number of transitions per interval (Mean)")
        self.numberOfTransitionsPerIntervalVariance           = self.getValue("Number of transitions per interval (Variance)")
        self.numberOfTransitionsPerIntervalStandardDeviation  = self.getValue("Number of transitions per interval (Standard deviation)")

        self.histogram = []
        for line in file(self.absFilename):
            if not line.startswith("#"):
                self.histogram.append(LreHistogramEntry(line.split()))


    # @staticmethod (this syntax works only for python >= 2.4)
    def readProbes(dirname):
        result = {}
        for suffix in LreProbe.fileNameSigs:
            result.update(Probe.readProbes(suffix, LreProbe, dirname))
        return result
    readProbes = staticmethod(readProbes)


class DlreHistogramEntry(LreHistogramEntry):

    def __init__(self, listOfValues):
        super(DlreHistogramEntry, self).__init__(listOfValues)


class DlreProbe(Probe):

    fileNameSigs = ["_DLREF.dat",
                    "_DLREG.dat",
                    "_DLREP.dat"]

    valueNames = ["lreType", "lowerBorder", "upperBorder", "numberOfIntervals",
                  "intervalSize", "maximumNumberOfSamples", "maximumRelativeErrorPercent",
                  "evaluatedLevels", "underflows", "overflows"] + Probe.valueNames

    histogram = None

    probeType = "DLRE"

    def __init__(self, filename):
        super(DlreProbe, self).__init__("DLRE", filename)

        self.dlreType                    = self.getValue("Evaluation")
        self.lowerBorder                 = self.getValue("lower border")
        self.upperBorder                 = self.getValue("upper border")
        self.numberOfIntervals           = self.getValue("number of intervals")
        self.intervalSize                = self.getValue("interval size")
        self.maximumNumberOfSamples      = self.getValue("maximum number of samples")
        self.maximumRelativeErrorPercent = self.getValue("maximum relative error [%]")
        self.evaluatedLevels             = self.getValue("evaluated levels")
        self.underflows                  = self.getValue("Underflows")
        self.overflows                   = self.getValue("Overflows")

        self.histogram = []
        for line in file(self.absFilename):
            if not line.startswith("#"):
                self.histogram.append(DlreHistogramEntry(line.split()))

    # @staticmethod (this syntax works only for python >= 2.4)
    def readProbes(dirname):
        result = {}
        for suffix in DlreProbe.fileNameSigs:
            result.update(Probe.readProbes(suffix, DlreProbe, dirname))
        return result
    readProbes = staticmethod(readProbes)


class TableProbe:
    fileNameSigs = ['_mean.dat',
                    '_max.dat',
                    '_min.dat',
                    '_trials.dat',
                    '_var.dat',
                    ] # there are more than these, but these are the most commonly used ones.
    valueNames = ["minimum", "maximum"]

    tableParser = None
    filename = None
    filenameWithoutDir = None
    name = None

    probeType = "Table"

    def __init__(self, filename):
        self.filename = filename
        self.dirname, self.filenameWithoutDir = os.path.split(self.filename)
        if self.dirname == "":
            self.dirname = "./"

        self.name = self.filenameWithoutDir.rsplit('_', 1)[0]
        self.type = self.filenameWithoutDir.rsplit('_', 1)[1]

        self.tableParser = pywns.TableParser.TableParser(filename)
        self.description               = self.tableParser.getDescription()
        self.minimum                   = self.tableParser.minimum
        self.maximum                   = self.tableParser.maximum
        self.trials                    = self.tableParser.trials

        self.mean                      = "-"
        self.variance                  = "-"
        self.relativeVariance          = "-"
        self.standardDeviation         = "-"
        self.relativeStandardDeviation = "-"
        self.skewness                  = "-"
        self.moment2                   = "-"
        self.moment3                   = "-"

    # @staticmethod (this syntax works only for python >= 2.4)
    def readProbes(dirname):
        result = {}
        for suffix in TableProbe.fileNameSigs:
            result.update(Probe.readProbes(suffix, TableProbe, dirname))
        return result
    readProbes = staticmethod(readProbes)

def readAllProbes(dirname):
    result = {}
    result = PDFProbe.readProbes(dirname)
    result.update(LogEvalProbe.readProbes(dirname))
    result.update(TimeSeriesProbe.readProbes(dirname))
    result.update(MomentsProbe.readProbes(dirname))
    result.update(TableProbe.readProbes(dirname))

    # @todo: update result dict with table probes when simcontrol can handle them
    return result

def getProbeType(filename):
    """This function identifies and returns the type of a probe file"""
    for probeType in [ MomentsProbe, PDFProbe, LogEvalProbe, TimeSeriesProbe ]:
        if probeType.fileNameSig in filename:
            return probeType

    for tableSuffix in TableProbe.fileNameSigs:
        if tableSuffix in filename:
            return TableProbe

    # if nothing was found
    raise TypeError("Could not identify probe type from filename: "+fileName)
