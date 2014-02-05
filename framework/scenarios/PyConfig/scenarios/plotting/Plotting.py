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

from pylab import *
import numpy

def loadMap(filename, idresolution, xmax, ymax, what, suffix):
    map_raw = load(filename + what+suffix, comments='%')
    trials_raw = load(filename + '_trials'+suffix, comments='%')

    map = [ [0.0] * (xmax) for tmp in xrange(ymax)]

    for line in xrange(len(map_raw[:,0])):
        xIndex = int ((map_raw[line,0]-1)/idresolution)
        yIndex = int ((map_raw[line,1]-1)/idresolution)

        if trials_raw[line,2] > 0:
            if map_raw[line,2] < -174:
                map[xIndex][yIndex] = -174
            else:
                map[xIndex][yIndex] = map_raw[line,2]
        else:
            map[xIndex][yIndex] = -174

        map = array(map)
    return map

def createMaximumMapOf(filenameList, idresolution, xmax, ymax, what):
    maps = []
    for filename in filenameList:
        print "Loading Map " + str(filename)
        maps.append(loadMap(filename, idresolution, xmax, ymax, what))

    xMax = len(maps[0])
    yMax = len(maps[0][0])

    outputMap = [ [0.0] * (xmax) for tmp in xrange(ymax)]

    for ii in xrange(xMax):
        for jj in xrange(yMax):
            maximum = -174
            for map in maps:
                if map[ii][jj] > maximum:
                    maximum = map[ii][jj]
            outputMap[ii][jj] = maximum

    return array(outputMap)

class MaximumMapCreator:
    def __init__(self, filenameList, idresolution, xmax, ymax, what="_max"):
        self.filenameList = filenameList
        self.idresolution = idresolution
        self.xmax = xmax
        self.ymax = ymax
        self.what = what
        self.basename = self.filenameList[0] + "_MaximumMap"
        self.outputMap = None

    def __call__(self):
        if self.outputMap == None:
            self.outputMap = createMaximumMapOf(self.filenameList,
                                                self.idresolution,
                                                int(self.xmax/self.idresolution),
                                                int(self.ymax/self.idresolution),
                                                self.what)
        return self.outputMap

class SingleMapCreator:
    def __init__(self, filename, idresolution, xmax, ymax, what="_mean", suffix=""):
        self.basename = filename
        self.idresolution = idresolution
        self.xmax = xmax
        self.ymax = ymax
        self.what = what
        self.suffix = suffix

    def __call__(self):
        return loadMap(self.basename,
                       self.idresolution,
                       int(self.xmax/self.idresolution),
                       int(self.ymax/self.idresolution),
                       self.what,
                       self.suffix)


def plotMap(mapCreator):
    clf()
    map = mapCreator()

    cset=imshow(map, cmap=cm.hot)
    colorbar()

    def tickFormatter(x, pos):
        return str(int(x * 10))

    axis('on')

    gca().xaxis.set_major_formatter(FuncFormatter(tickFormatter))
    gca().yaxis.set_major_formatter(FuncFormatter(tickFormatter))
    gca().set_xlim(0, int(mapCreator.xmax/mapCreator.idresolution))
    gca().set_ylim(0, int(mapCreator.ymax/mapCreator.idresolution))
    #hot()

    title('SINR Map of Manhattan Scenario')
    #show()

    savefig(mapCreator.basename + '_Coverage.png')
