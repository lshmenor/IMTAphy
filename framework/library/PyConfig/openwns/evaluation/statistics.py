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

from openwns.pyconfig import Sealed, attrsetter

class StatEval(Sealed):
    """
    StatEval is the Baseclass of all Evaluation types
    it includes the typename which is set by the deriving
    class. The follwing parameters can be set for each
    StatEval type:

    name         : The Name of the Probe

    description  : optional description of the Probe

    typename     : deprecated, but still needed for backward compatibility
                   TODO: remove it

    skipInterval : For the (D)LRE types this defines the number
                   of probe writes after which the relative error
                   is evaluated
                   For all other types this defines how many values
                   are skipped after each write to a probe.
                   TODO: move this into the classes that really need it

    numIntervals : This is only relevant for the BatchMeans and Histogram
                   types.
                   TODO: move this into the classes that really need it

    format       : For each probe type the number format can be set
                   individually. Use 'fixed' or 'scientific'
    prefix       : Character to be prepended to comment lines in the output file

    scalingFactor: Scales all measurements by this factor
    """
    name = None

    description = None

    typename = None

    skipInterval = None

    numIntervals = None

    format = None

    prefix = None

    appendFlag = None

    scalingFactor = None

    nameInFactory = "openwns.evaluation.statistics.StatEval"

    def __init__(self, typename, skipInterval=0, numIntervals=0, format="fixed", prefix = "#", name="no name available", description = "no description available", scalingFactor = 1.0):
        self.name = name
        self.description = description
        self.typename=typename
        self.skipInterval=skipInterval
        self.numIntervals=numIntervals
        self.format = format
        self.prefix = prefix
        self.scalingFactor = scalingFactor

    def getTypeName(self):
        return(self.typename)

    def getSkipInterval(self):
        return(self.skipInterval)

    def getNumIntervals(self):
        return(self.numIntervals)

    def getFormat(self):
        return(self.format)

class MomentsEval(StatEval):
    """
    Configuration for the Moments Evaluation Object.

    As opposed to the normal StatEval, this Evaluation Object can take
    weighted measurements.

    Parameters:      None
    """
    nameInFactory = "openwns.evaluation.statistics.Moments"

    def __init__(self, **kwargs):
        StatEval.__init__(self,"Moments")
        # No special settings for Moments probe
        attrsetter(self, kwargs)

    def getIniFile(self):
        return("")

class PDFEval(StatEval):
    """
    Configuration for the PDF Evaluation Object.

    The PDF StatEval stores (and prints on request) the
    - Cumulative Distribution Function (CDF,F(x))
    - Complementary Cumulative Distribution Function(CCDF, G(x))
    - Probability Density Function (PDF, p(x))

    Parameters:

    minXValue  : left border of the x axis. Underflows are recorded
    maxXValue  : right border of the x axis. Overflows are recorded
    resolution : number of bins into which the x axis will be divided
    """
    nameInFactory = "openwns.evaluation.statistics.PDF"

    xScaleType = None
    minXValue = None
    maxXValue = None
    resolution = None

    def __init__(self, **kwargs):
        super(PDFEval, self).__init__("PDF")
        self.minXValue = 0.0
        self.maxXValue = 1.0
        self.resolution = 100
        self.xScaleType = "linear"
        attrsetter(self, kwargs)

    def getIniFile(self):
        rs = ";; Settings for PDF\n"
        rs += "xScaleType = " + self.xScaleType + "\n"
        rs += "minXValue = " + str(self.minXValue) + "\n"
        rs += "maxXValue = " + str(self.maxXValue) + "\n"
        rs += "resolution = " + str(self.resolution) + "\n"
        return(rs)

class DLREEval(StatEval):
    """
    Configuration for the DLRE Statistical Evaluation Object

    Discrete LRE (LRE III), See Habilitation Thesis C. Goerg, ComNets 1997

    Parameters:

    distances          :   'equi' or 'nonequi', in the first case, you have to specify
       'equi' -->
             xMin          :  left border of x axis
             xMax          :  right border of x axis
             intervalWidth :  width of each bin on the x axis -> number of bins = (xMax-xMin)/intervalWidth
       'nonequi' -->
             xValues       :  array of x Values to calculate the F/G/p for

    forceRminusAOK     :   boolean flag whether or not to force the last
                           large sample criterion (r - av >= 10)
    minLevel           :   evaluation is finished if the F/G level of the current bin is below this value
                           i.e. (1-minLevel) of all trials have been included in the evaluation
                           This does (obviously) not apply in the p-Version of the algorithm
    initValue          :   This tells the algorithm in which of the x-Bins to start. (pab: I think the
                           algorithm converges quicker if you set this to the expected value of
                           your random variable)
    maxError           :   The maximum (aka the Limit of the) relative Error
    maxRoundingError   :   maximum deviation from the specified discrete x-values that is tolerated
                           (you COULD consider it as the width of the bins)
    maxNumTrials       :   after how many trials should the evaluation stop. You can say 'infinity'
                           here

    skipInterval       :   number of samples after which simulation is checked for completion

                           If skipInterval is set to 0 (this is the default), the current
                           level is checked for completion after each input.
                           Use skipInterval to speed up your simulation, i.e. if your
                           simulation generates 10,000 values per second then set
                           skipInterval to a reasonably high value, e.g. 10,000.

    active             :   Flag determining whether this StatEval contributes
                           To determine whether the desired stochastical security
                           level of the simulation has been reached.
    """
    nameInFactory = None

    active = False
    forceRminusAOK = False
    minLevel = 0.00001
    maxError = 0.05
    initValue = 0.0
    maxRoundingError = 1e-9
    # equidistant Settings
    xMin = 0.0
    xMax = 1.0
    intervalWidth = 0.01
    maxNumTrials = "infinity"
    # nonEquiDistantSettings
    xValues = None # type: list
    # distances mode
    distances = "equi"

    def __init__(self, mode="f", distances="equi", **kwargs):
        """
        Constructor parameters:
        mode        :   if set to 'F', the output is the CDF
                        if set to 'G', the output is the CCDF
                        if set to 'P', the output is the PDF
        distances   :   can be set to 'equi' or 'nonequi', see
                        class description
        """
        self.distances = distances
        if(mode.lower()=="f"):
            super(DLREEval, self).__init__("DLREF")
            self.nameInFactory = "openwns.evaluation.statistics.DLREF"
        elif(mode.lower()=="g"):
            super(DLREEval, self).__init__("DLREG")
            self.nameInFactory = "openwns.evaluation.statistics.DLREG"
        elif(mode.lower()=="p"):
            super(DLREEval, self).__init__("DLREP")
            self.nameInFactory = "openwns.evaluation.statistics.DLREP"
        else:
            raise Exception("You configured an unknown DLRE mode '"+mode+"'")

        self.active = False
        self.forceRminusAOK = False
        self.minLevel = 0.00001
        self.maxError = 0.05
        self.initValue = 0.0
        self.maxRoundingError = 1e-9
        self.maxNumTrials = "infinity"
        if (distances=="equi"):
            # equidistant Settings
            self.xMin = 0.0
            self.xMax = 1.0
            self.intervalWidth = 0.001
            self.xValues = None
        elif (distances=="nonequi"):
            # nonEquiDistantSettings
            self.xMin = None
            self.xMax = None
            self.intervalWidth = None
            self.xValues = []
        else:
            raise "Unknown DLRE distances setting '%s'" % distances
        attrsetter(self, kwargs)

    def addDistance(self, aIntervalBorder):
        self.xValues.append(aIntervalBorder)

    def getIniFile(self):
        rs = ";; Settings for DLRE\n"
        rs += "active = " + str(self.active).lower() + "\n"
        rs += "forceRminusAOK = " + str(self.forceRminusAOK).lower() + "\n"
        rs += "minLevel = " + str(self.minLevel) + "\n"
        rs += "maxError = " + str(self.maxError) + "\n"
        rs += "initValue = " + str(self.initValue) + "\n"
        rs += "maxRoundingError = " + str(self.maxRoundingError) + "\n"
        rs += "maxNumTrials = " + str(self.maxNumTrials) + "\n"
        if(self.distances == "equi"):
            rs += "xMin = " + str(self.xMin) + "\n"
            rs += "xMax = " + str(self.xMax) + "\n"
            rs += "intervalWidth = " + str(self.intervalWidth) + "\n"
        else:
            rs += "xValues = "
            for i in self.xValues.sort():
                rs += str(i) + " "
            rs += "\n"
        return(rs)

class TextEval(StatEval):
    """
    Configuration for the Text Evaluation Object.

    As opposed to all other StatEvals, this one does not
    accept numerical values.

    It is comparable to the LogEval but can store a time-series of
    strings, not of numbers

    Parameters:

    prependSimTime   : boolean flag, whether to prepend the simtime
                       when the string was logged in front of every
                       output line or not
    width            : Overall width of the simTime field
    precision        : number of decimal fields of simTime field
    appendFlag       : deprecated, currently used by PDataBase to determine
                       whether to append values to an already existing file
                       when writing the probe.
    skipInterval     : this defines how many values are skipped after each write
                       to a probe.
    """
    nameInFactory = "wns.probe.stateval.TextEval"
    prependSimTime = True
    width = 12
    precision = 7

    def __init__(self, **kwargs):
        self.prependSimTime = True
        self.width = 12
        self.precision = 7
        #self.appendFlag = True # only way for LogEval probes
        self.appendFlag = False # only way for PDFEval probes
        StatEval.__init__(self,"PProbeText")
        attrsetter(self, kwargs)

    def getIniFile(self):
        rs = ";;Settings for ProbeText\n"
        rs += "prependSimTime = " + repr(self.prependSimTime).lower() + "\n"
        rs += "width = " + str(self.width) + "\n"
        rs += "precision = " + str(self.precision) + "\n"
        rs += "appendFlag = " + repr(self.appendFlag).lower() + "\n"
        return(rs)

