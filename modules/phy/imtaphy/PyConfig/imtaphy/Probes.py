################################################################################
# This file is part of IMTAphy
# _____________________________________________________________________________
#
# Copyright (C) 2010
# Institute of Communication Networks (LKN)
# Department of Electrical Engineering and Information Technology (EE & IT)
# Technische Universitaet Muenchen
# Arcisstr. 21
# 80333 Muenchen - Germany
# http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
#
# _____________________________________________________________________________
#
#   IMTAphy is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   IMTAphy is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
#
#################################################################################

import openwns.evaluation
from openwns.evaluation import *



class ProbeConfig:
    sinrProbeName = None
    widebandLossProbeName = None 
    pathlossProbeName = None
    shadowingProbeName = None
    avgSINRProbeName = None
    medianSINRProbeName = None
    fastFadingProbeName = None
    xBins = None
    yBins = None
    numberOfRounds = None
    

    def __init__(self, scenarioName, xBins, yBins, numberOfRounds):
        self.sinrProbeName = "sinr_" + scenarioName
        self.widebandLossProbeName = "wbl_" + scenarioName
        self.pathlossProbeName = "pathloss_" + scenarioName
        self.shadowingProbeName = "shadowing_" + scenarioName
        self.avgSINRProbeName = "avgSINR_" + scenarioName
        self.medianSINRProbeName = "medianSINR_" + scenarioName
        self.fastFadingProbeName = "fastFading_" + scenarioName
        self.xBins = xBins
        self.yBins = yBins
        self.numberOfRounds = numberOfRounds
        


def installScannerProbes(scenarioConfig, probeConfig, simulator, settlingTime=0, scenarioPlotting=False, withBestServer=False):
        # Create a table generator for the evaluation
    width = scenarioConfig.xMax - scenarioConfig.xMin
    height = scenarioConfig.yMax - scenarioConfig.yMin
    if scenarioPlotting:
        table = Table(axis1 = 'scenario.x', minValue1 = scenarioConfig.xMin,
                maxValue1 = scenarioConfig.xMax, resolution1 = probeConfig.xBins,
                axis2 = 'scenario.y', minValue2 = scenarioConfig.yMin,
                maxValue2 = scenarioConfig.yMax, resolution2 = probeConfig.yBins,
                values = ['max', 'mean', 'trials'], # trials is needed for fillValue
                formats = ['MatlabReadable']
                )

    ############################## wideband Loss probe ##########################
    node = openwns.evaluation.createSourceNode(simulator, probeConfig.widebandLossProbeName)

    if scenarioPlotting:
        node.appendChildren(table)
    node.appendChildren(PDF(name = probeConfig.widebandLossProbeName,
                            description = 'Wideband Loss [dB]',
                            minXValue = -160,
                            maxXValue = -40,
                            resolution = 240))
    ##########################################################################


        ############################## pathloss probe ##########################
    node = openwns.evaluation.createSourceNode(simulator, probeConfig.pathlossProbeName)

    if scenarioPlotting:
        node.appendChildren(table)
    node.appendChildren(PDF(name = probeConfig.pathlossProbeName,
                            description = 'Pathloss [dB]',
                            minXValue = -200,
                            maxXValue = -40,
                            resolution = 160))
    ##########################################################################



    ############################## shadowing probe ##########################
    node = openwns.evaluation.createSourceNode(simulator, probeConfig.shadowingProbeName)

    if scenarioPlotting:
        node.appendChildren(table)
    node.appendChildren(PDF(name = probeConfig.shadowingProbeName,
                            description = 'Shadowing [dB]',
                            minXValue = -30,
                            maxXValue = 30,
                            resolution = 120))
    ##########################################################################


    ############################## fast fading probe ##########################
    node = openwns.evaluation.createSourceNode(simulator, probeConfig.fastFadingProbeName)

    node.appendChildren(Logger())

#       fast fading separately for all MSs and PRBs:
#	msSep = node.appendChildren(Separate(by = 'MSID', forAll = range(len(bsPositions)*len(azimuths) +1, len(bsPositions)*len(azimuths) + len(msPositions)), format = "MSID%d"))
#	prbSep = msSep.appendChildren(Separate(by = 'PRB', forAll = xrange(numPRBs), format = "PRB%d"))

# TODO: enable again, if needed for debugging
#	msSep = node.appendChildren(Separate(by = 'MSID', forAll = range(len(bsPositions)*len(azimuths) +1, len(bsPositions)*len(azimuths) + 4), format = "MSID%d"))
#	prbSep = msSep.appendChildren(Separate(by = 'PRB', forAll = xrange(5), format = "PRB%d"))


#	prbSep.appendChildren(Logger())
#	prbSep.appendChildren(PDF(name = probeConfig.fastFadingProbeName,
#				  description = 'fastFading [linear]',
#				  minXValue = 0,
#				  maxXValue = 100,
#				  resolution = 1000))

    if scenarioPlotting:
        node.appendChildren(table)
    node.appendChildren(PDF(name = probeConfig.fastFadingProbeName,
                            description = 'fastFading [linear]',
                            minXValue = 0,
                            maxXValue = 100,
                            resolution = 1000))
    ##########################################################################


    ############################## SINR probe ##########################
    node = openwns.evaluation.createSourceNode(simulator, probeConfig.sinrProbeName)
    s = node.appendChildren(SettlingTimeGuard(settlingTime))

    s.appendChildren(PDF(name = probeConfig.sinrProbeName,
                            description = 'SINR [dB]',
                            minXValue = -200,
                            maxXValue = 200,
                            resolution = 4000))


    if scenarioPlotting:
        node.getLeafs().appendChildren(table)
    
    msSep = s.appendChildren(Separate(by = 'MSID', forAll = range(1, 200), format = "MSID%d"))
    
    msSep.appendChildren(PDF(name = probeConfig.sinrProbeName,
                            description = 'SINR [dB]',
                            minXValue = -100,
                            maxXValue = 100,
                            resolution = 400))
                            

    if withBestServer:
        # To get best server plots
        try:
            argmax = s.appendChildren(ArgMax("BSID"))
        except:
            pass

    ##########################################################################

    ############################## avg SINR probe ##########################
    node = openwns.evaluation.createSourceNode(simulator, probeConfig.avgSINRProbeName)

    if scenarioPlotting:
        node.appendChildren(table)
    node.appendChildren(PDF(name = probeConfig.avgSINRProbeName,
                            description = 'avg. SINR [dB]',
                            minXValue = -60,
                            maxXValue = 80,
                            resolution = 420))

    if withBestServer:
        # To get best server plots
        try:
            argmax = node.appendChildren(ArgMax("BSID"))
        except:
            pass
    if scenarioPlotting:
        node.getLeafs().appendChildren(table)
    ##########################################################################

    ############################## median SINR probe ##########################
    node = openwns.evaluation.createSourceNode(simulator, probeConfig.medianSINRProbeName)

    if scenarioPlotting:
        node.appendChildren(table)
    node.appendChildren(PDF(name = probeConfig.avgSINRProbeName,
                            description = 'median SINR [dB]',
                            minXValue = -20,
                            maxXValue = 60,
                            resolution = 320))

    if withBestServer:
        # To get best server plots
        try:
            argmax = node.appendChildren(ArgMax("BSID"))
        except:
            pass
    if scenarioPlotting:
        node.getLeafs().appendChildren(table)
    ##########################################################################

