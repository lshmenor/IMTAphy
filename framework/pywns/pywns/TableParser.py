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

class TableParser:
    fileName = None
    header = None
    lines = None
    firstRowContains = None
    firstRowIdName = None
    secondRowContains = None
    secondRowIdName = None
    description = None
    minimum = None
    maximum = None
    trials = None

    xvalues = None
    yvalues = None
    valueMap = None

    xcol = None
    ycol = None
    zcol = 2

    def __init__(self, fileName, firstRowContains = None, firstRowIdName = None,
                 secondRowContains = None, secondRowIdName = None, description = None,
                 minimum = None, maximum = None, lines = None):
        if firstRowContains is None:
            self.header = ''
            self.lines = []
            self.fileName = fileName
            self.maximum = 0
            self.minimum = 0
            self.trials = 0
            firstLine = True
            infile = file(self.fileName)
            for line in infile:
                if line.startswith("%"):
                    self.header += line
                    if line.count("This table contains the"):
                        self.description = line[2:].strip()
                    elif line.count("Dim "):
                        if line.count("Dim 1:"):
                            self.firstRowIdName = line.split(':')[1].strip().split('\'')[1]
                            self.firstRowContains = 'r'
                            if(self.secondRowIdName is None):
                                # fill second row description, will be overwritten if dim == 2
                                self.secondRowIdName = 'na'
                                self.secondRowContains = 'c'
                        elif line.count("Dim 2:"):
                            self.secondRowIdName = line.split(':')[1].strip().split('\'')[1]
                            self.secondRowContains = 'c'
                        else:
                            # more than two dimensions -> not directly supported...
                            # shift value column by one
                            self.zcol += 1
                else:
                    if len(line.strip()) > 0:
                        self.trials+=1
                        lineValues = [ float(col) for col in line.strip().split() ]
                        if(firstLine):
                            self.maximum = lineValues[-1]
                            self.minimum = lineValues[-1]
                            firstLine = False
                        self.maximum = max(lineValues[-1], self.maximum)
                        self.minimum = min(lineValues[-1], self.minimum)
                        self.lines.append(lineValues)

            infile.close()
        else:
            self.header = ''
            self.lines = lines
            self.fileName = fileName
            self.firstRowContains = firstRowContains
            self.firstRowIdName = firstRowIdName
            self.secondRowContains = secondRowContains
            self.secondRowIdName = secondRowIdName
            self.description = description
            self.minimum = minimum
            self.maximum = maximum

        self.xcol = 0
        self.ycol = 1
        self.xvalues = [ v for v in set([ lineValues[0] for lineValues in self.lines ])]
        self.yvalues = [ v for v in set([ lineValues[1] for lineValues in self.lines ])]

        self.xvalues.sort()
        self.yvalues.sort()

    def getArray(self):
        self.valueMap = {}
        # store measured values
        for line in self.lines:
            self.valueMap[(line[self.xcol],line[self.ycol])] = line[self.zcol]

        valueArray = []

        lineNo = 0
        for yv in self.yvalues:
            valueArray.append([]) # create new line
            for xv in self.xvalues:
                valueArray[lineNo].append( self.valueMap[(xv,yv)] )
            lineNo +=1
        return valueArray

    def getXValues(self):
        return self.xvalues

    def getYValues(self):
        return self.yvalues

    def getRowIdName(self):
        if self.ycol==0:
            return self.firstRowIdName
        else:
            return self.secondRowIdName

    def getColumnIdName(self):
        if self.xcol==0:
            return self.firstRowIdName
        else:
            return self.secondRowIdName

    def getDescription(self):
        return self.description
