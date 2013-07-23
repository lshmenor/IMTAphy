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

import os
import subprocess
import sys

class SimulatorConfigNotFound:
    pass

        
    
class ConfigInspector:

    def __init__(self, configurationFile):
        currentdir = os.getcwd()

        self.configurationFile = configurationFile

        cwd = os.getcwd()
        print cwd

        folders=[]
        path = cwd
        while 1:
            path,folder=os.path.split(path)
            if folder!="":
                folders.insert(0,folder)
            else:
                if path!="/":
                    folders.insert(0, path)
                break
        
        for i in reversed(range(len(folders))):
            testPath = '/'
            for j in range(i+1):
                testPath = os.path.join(testPath, folders[j])
                
            sandboxPath = os.path.join(testPath, "sandbox", "dbg", "lib", "PyConfig")
            if os.path.exists(sandboxPath):
                break
            sandboxPath = os.path.join(testPath, "sandbox", "opt", "lib", "PyConfig")
            if os.path.exists(sandboxPath):
                break   

        if not os.path.exists(sandboxPath):
            sys.exit("No sandbox found here or in any directory above the current path, exiting. Start from within SDK!")
        
        self.config = {}
        exec("import sys", self.config)
        filepath = os.path.dirname(str(configurationFile))
        exec("sys.path.append('%s')" % filepath, self.config)
        exec("sys.path.insert(0,'%s')" % sandboxPath, self.config)


        file = open(str(configurationFile), "r")
        content = file.read()
        file.close()

        os.chdir(filepath)
        exec(content,self.config)
        
        os.chdir(currentdir)
  
        self.simulator = self.getSimulator()
  
    def getSimulator(self):
        import openwns.simulator
        for k,v in self.config.items():
            if isinstance(v, openwns.simulator.OpenWNS):
                return v    
        raise SimulatorConfigNotFound


        
    def getNodes(self):
        sim = self.simulator
        return sim.simulationModel.nodes

    def hasMobility(self, node):
        try:
            import rise.Mobility
        except:
            return False    
        for c in node.components:
            if isinstance(c, rise.Mobility.Component):
                return True
        return False

    def hasStationPhy(self, node):
        import imtaphy.Station
        for c in node.components:
            if isinstance(c, imtaphy.Station.StationPhy):
                return True
        return False

    def getStationPhy(self, node):
        import imtaphy.Station
        for c in node.components:
            if isinstance(c, imtaphy.Station.StationPhy):
                return c
        

    def getPosition(self, node):
        if self.hasMobility(node):
            return self.getMobility(node).coords
        if self.hasStationPhy(node):
            return self.getStationPhy(node).position
           

    def getMobility(self, node):
        import rise.Mobility
        assert self.hasMobility(node), "No mobility found"

        for c in node.components:
            if isinstance(c, rise.Mobility.Component):
                return c.mobility



    def getNodeType(self, node):
        
        classname = node.__class__.__name__

        return classname

    def getNodeTypeId(self, node):
        """Returns the node type
        0 : Base Station
        1 : User Terminal
        2 : Relay Node
        3 : Unknown
        """

        classname = self.getNodeType(node)

#        print classname

        if classname in ['eNB', 'BS', 'BaseStation', 'Station']:
            return 0

        if classname in ['UE', 'MS', 'UserTerminal']:
            return 1

        if classname in ['RN', 'RelayNode']:
            return 2

        if classname in ['ScannerStation']:
            return 4

        return 3
    
        for node in self.getNodes():
            if self.getNodeTypeId(node) == 0:
                try:
                    length = node.phy.ofdmaStation.receiver[0].propagation.maxId()
                    for a in range(length):
                        for b in range(length):
                            shadowing = node.phy.ofdmaStation.receiver[0].propagation.getPair(a,b).shadowing
                            if isinstance(shadowing, rise.scenario.Shadowing.Objects):
                                return shadowing.obstructionList                      
                except: # Exception as inst: #only in python 2.6!?
                    #print type(inst), inst
                    pass
        return []


    def getSize(self):
        try:
            bbox = self.config["scenario"]
            xMax = bbox.xmax
            yMax = bbox.ymax
            xMin = bbox.xmin
            yMin = bbox.ymin
            return (xMin, yMin, xMax, yMax) 
        except:
            print "No imtaphy bounding box defined"
    
        try:
            xMax = self.config["xMax"]
            yMax = self.config["yMax"]
            xMin = self.config["xMin"]
            yMin = self.config["yMin"]
            return (xMin, yMin, xMax, yMax) 
        except:
            print "Reading sizes from nodes"
            pass

        xMax = None
        xMin = None
        yMax = None
        yMin = None

        if self.hasMobility(n):
            m = self.getMobility(n)
            if xMax is None or xMax < m.coords.x:
                xMax = m.coords.x
  
            if xMin is None or xMin > m.coords.x:
                xMin = m.coords.x
  
            if yMax is None or yMax < m.coords.y:
                yMax = m.coords.y
  
            if yMin is None or yMin > m.coords.y:
                yMin = m.coords.y

        width = xMax - xMin
        height = yMax - yMin

        dw = width * 0.05
        dh = height * 0.05

        if dw < 50:
            dw = 50
        if dh < 50:
            dh = 50

        scenXMin = max(0, xMin - dw)
        scenYMin = max(0, yMin - dh)
        scenXMax = max(0, xMax + dw)
        scenYMax = max(0, yMax + dh)

        return (scenXMin, scenYMin, scenXMax, scenYMax)
