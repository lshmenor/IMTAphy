################################################################################
# This file is part of IMTAphy
# _____________________________________________________________________________
#
# Copyright (C) 2012
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
#
# based on code with this license:
#
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

import math

import random
import pylab
import numpy
import matplotlib
from numpy import *

try:
    import openwns.simulator
except:
    pass

class GenericPlotter:

    def __init__(self, inspector):
        self.inspector = inspector

    def loadImage(self, fileToPlot, fillValue, minValue, maxValue):
        baseFilename = '_'.join(fileToPlot.split("_")[:-1])
        what= '_' + fileToPlot.split("_")[-1]

        try:
            filename = baseFilename + what
            trialFilename = baseFilename + '_trials.m'
            print "Loading %s" % filename
            map_raw = numpy.loadtxt(filename, comments='%')
        
            print "Loading %s" % trialFilename
            trials_raw = numpy.loadtxt(trialFilename, comments='%')
        except IOError:
            print "IOError"
            return None

        map_parsed = rec.fromrecords(map_raw, names = 'x,y,z')
        trials_parsed = rec.fromrecords(trials_raw, names = 'x,y,z')
        self.numXEntries = len(unique(map_parsed['x']))
        self.minX = min(map_parsed['x'])
        self.maxX = max(map_parsed['x'])

        self.numYEntries = len(unique(map_parsed['y']))
        self.minY = min(map_parsed['y'])
        self.maxY = max(map_parsed['y'])

        print "The map is (%dx%d)" % (self.numYEntries+1, self.numXEntries+1)
        map = ones((self.numYEntries, self.numXEntries)) * fillValue

        i = 0
        print "x entries:", self.numXEntries, "y entries:", self.numYEntries 
        assert (self.numXEntries * self.numYEntries  == len(map_parsed['x']))
        for xx in xrange(self.numXEntries):
            for yy in xrange(self.numYEntries):
                if trials_parsed['z'][i] > 0:
                    # limit to max/min value if set to values different than 0
                    if minValue != float("-inf"): 
                        map_parsed['z'][i] = max(map_parsed['z'][i], minValue)
                    if maxValue != float("+inf"):
                        map_parsed['z'][i] = min(map_parsed['z'][i], maxValue)
                    map[yy][xx] = map_parsed['z'][i]
                else:
                    map[yy][xx] = minValue - 0.0001
                i += 1

        return map
        
    def plotScenario(self, canvas, fileToPlot, fillValue, minValue, maxValue, includeContour):
        """this method should be implemented in any class that derives
from ScenarioFrame to plot special scenarios"""
        axes = canvas.axes
        axes.hold(True)

        scenarioSize = self.inspector.getSize()


        axes.grid(True)

        groupcolors = { 1 : 'r', 2:'b', 3:'g', 4:'y', 5:'m', 6:'c' }

        shapes = { 0 : '^', 1 : 'o', 2 : 's', 3: 'x', 4: ''}

        map = self.loadImage(fileToPlot, fillValue, minValue, maxValue)
        

        if map is not None: # place markers into middle of bins
            (rows, cols) = map.shape
            xOffset = (scenarioSize[2]-scenarioSize[0]) / cols /2
            yOffset = (scenarioSize[3]-scenarioSize[1]) / rows /2
        else:
            xOffset = 0
            yOffset = 0

        # TODO: check if this offset thing is necessary
        xOffset = 0
        yOffset = 0
            
            
        for n in self.inspector.getNodes():
            if self.inspector.hasMobility(n) or self.inspector.hasStationPhy(n):
                pos = self.inspector.getPosition(n)
                type = self.inspector.getNodeTypeId(n)
                color = groupcolors[type + 1]

               

                if type == 0: # eNB
                    scalingX = (scenarioSize[2]-scenarioSize[0]) / 30
                    scalingY = (scenarioSize[3]-scenarioSize[1]) / 30
                    xr = scalingX * math.sin(n.components[1].antenna.azimuth) - scalingX / 2
                    yr = scalingY * math.cos(n.components[1].antenna.azimuth)
#                    xr=random.randint(-50,50)
#                    yr=random.randint(-50,50)
                    axes.text(pos.x + xr, pos.y + yr, n.name)
                    axes.plot([pos.x + xOffset], 
                          [pos.y + yOffset], color+shapes[type])
                else:
                    if map is None: #only print UE markers when not plotting map below
                        axes.plot([pos.x + xOffset], 
                                  [pos.y + yOffset], color+shapes[type])

        # defines a custom colormap that has a different color for the 0 entry
        # which basically is the color for non-existing values. 
        # it is the first line for each red/green/blue and can be set to black (0,0,0)
        # or white (0,1,1)
        # http://www.scipy.org/Cookbook/Matplotlib/Show_colormaps

        # based on original jet color map from matplotlib's _cm.py
        cdict  =   {'red':   ((0.0, 1.0, 1.0),
                              (0., 0, 0), 
                              (0.35, 0, 0), 
                              (0.66, 1, 1), 
                              (0.89,1, 1),
                              (1, 0.5, 0.5)),
                    'green': ((0.0, 1.0, 1.0),
                              (0., 0, 0), 
                              (0.125,0, 0), 
                              (0.375,1, 1), 
                              (0.64,1, 1),
                              (0.91,0,0), 
                              (1, 0, 0)),
                    'blue':  ((0.0, 1.0, 1.0),
                              (0., 0.5, 0.5), 
                              (0.11, 1, 1), 
                              (0.34, 1, 1), 
                              (0.65,0, 0),
                              (1, 0, 0))}

# this is a simpler example that does not lot as nice as the original color jet map
#        cdict = {'red': ((0.0, 1.0, 1.0),
#                         (0.0, 0.0, 0.0),
#                         (1.0, 1.0, 1.0)),
#
#                 'green': ((0.0, 1.0, 1.0),
#                           (0.0, 0.0, 0.0),
#                           (0.5, 1.0, 1.0),
#                           (1.0, 0.0, 0.0)),
#                 'blue': ((0.0, 1.0, 1.0),
#                          (0.0, 1.0, 1.0),
#                          (1.0, 0.0, 0.0))}

        my_cmap = matplotlib.colors.LinearSegmentedColormap('my_colormap',cdict,256)




        if map is not None:
            im = axes.imshow(map, cmap = my_cmap,
                             origin = 'lower',
                             interpolation = "nearest",
                             extent= (scenarioSize[0], scenarioSize[2],
                                      scenarioSize[1], scenarioSize[3])
                             )
            canvas.fig.colorbar(im)

            if includeContour:
                cs = axes.contour(map, cmap = my_cmap,
                                  extent= (scenarioSize[0], scenarioSize[2],
                                           scenarioSize[1], scenarioSize[3])
                                  )
                axes.clabel(cs)

        axes.set_xlim(scenarioSize[0], scenarioSize[2])
        axes.set_ylim(scenarioSize[1], scenarioSize[3])

        canvas.draw()
