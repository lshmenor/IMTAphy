#!/usr/bin/env python2.6

from SpecialGraphs import *
from BasicGraphs import *
from Importers import *
import boomslang
import sys
import os
import fnmatch
import math

lineWidth = 2

cqiStyles = {
    # 1 - 6 QPSK
    # 7 - 9 QAM16
    # 10 - 15 QAM64
    1: {'lineStyle': '-',
        'color': 'b'},
    2: {'lineStyle': '-',
        'color': 'b'},
    3: {'lineStyle': '-',
        'color': 'b'},
    4: {'lineStyle': '-',
        'color': 'b'},
    5: {'lineStyle': '-',
        'color': 'b'},
    6: {'lineStyle': '-',
        'color': 'b'},
    7: {'lineStyle': '-',
        'color': 'g'},
    8: {'lineStyle': '-',
        'color': 'g'},
    9: {'lineStyle': '-',
        'color': 'g'},
    10: {'lineStyle': '-',
        'color': 'r'},
    11: {'lineStyle': '-',
        'color': 'r'},
    12: {'lineStyle': '-',
        'color': 'r'},
    13: {'lineStyle': '-',
        'color': 'r'},
    14: {'lineStyle': '-',
        'color': 'r'},
    15: {'lineStyle': '-',
        'color': 'r'},
    }


mcsStyles = {
    # 0 - 9 QPSK
    # 10 - 16 QAM16
    # 17 - 28 QAM64
    0: {'lineStyle': '-',
    'color': 'b'},
    1: {'lineStyle': '-',
    'color': 'b'},
    2: {'lineStyle': '-',
    'color': 'b'},
    3: {'lineStyle': '-',
    'color': 'b'},
    4: {'lineStyle': '-',
    'color': 'b'},
    5: {'lineStyle': '-',
    'color': 'b'},
    6: {'lineStyle': '-',
    'color': 'b'},
    7: {'lineStyle': '-',
    'color': 'b'},
    8: {'lineStyle': '-',
    'color': 'b'},
    9: {'lineStyle': '-',
    'color': 'b'},
    10: {'lineStyle': '-',
    'color': 'g'},
    11: {'lineStyle': '-',
    'color': 'g'},
    12: {'lineStyle': '-',
    'color': 'g'},
    13: {'lineStyle': '-',
    'color': 'g'},
    14: {'lineStyle': '-',
    'color': 'g'},
    15: {'lineStyle': '-',
    'color': 'g'},
    16: {'lineStyle': '-',
    'color': 'g'},
    17: {'lineStyle': '-',
    'color': 'r'},
    18: {'lineStyle': '-',
    'color': 'r'},
    19: {'lineStyle': '-',
    'color': 'r'},
    20: {'lineStyle': '-',
    'color': 'r'},
    21: {'lineStyle': '-',
    'color': 'r'},
    22: {'lineStyle': '-',
    'color': 'r'},
    23: {'lineStyle': '-',
    'color': 'r'},
    24: {'lineStyle': '-',
    'color': 'r'},
    25: {'lineStyle': '-',
    'color': 'r'},
    26: {'lineStyle': '-',
    'color': 'r'},
    27: {'lineStyle': '-',
    'color': 'r'},
    28: {'lineStyle': '-',
    'color': 'r'},
        }

def plotMCSBlers():
    plot = Plot()

    plot.setXLimits(-10, 25)
    #    plot.setXLimits(5, 10)
    plot.setYLimits(0, 1)
    plot.setXLabel("SINR")
    plot.setYLabel("BLER")
    plot.logy = True
    
    #(plot.width, plot.height) = (16,9) # getGoldenRatioDimensions(35)

    fileName = "blersMCS"

    for mcs in range(29):
        line = boomslang.Line()
        (line.xValues, line.yValues) = getXYfromFile(fileName+".txt", 0, mcs + 1)
        #    print line.xValues, line.yValues
        if mcs==0:
            line.label = "QPSK"
        elif mcs==10:
            line.label = "16 QAM"
        elif mcs==17:
            line.label = "64 QAM"
            
        line.color = mcsStyles[mcs]['color']
        line.lineStyle = mcsStyles[mcs]['lineStyle']
        line.lineWidth = line.lineWidth * 3
        
        plot.add(line)
        
    plot.save(fileName+".pdf")    

def plotCQIBlers():
    plot = Plot()

    plot.setXLimits(-10, 25)
    #    plot.setXLimits(5, 10)
    plot.setYLimits(0, 1)
    plot.setXLabel("SINR")
    plot.setYLabel("BLER")
    plot.logy = True
    
    #(plot.width, plot.height) = (16,9) # getGoldenRatioDimensions(35)

    fileName = "blersCQI"

    for cqi in range(1,16):
        line = boomslang.Line()
        (line.xValues, line.yValues) = getXYfromFile(fileName+".txt", 0, cqi)
        #    print line.xValues, line.yValues
        if cqi==0:
            line.label = "QPSK"
        elif cqi==10:
            line.label = "16 QAM"
        elif cqi==17:
            line.label = "64 QAM"
            
        line.color = cqiStyles[cqi]['color']
        line.lineStyle = cqiStyles[cqi]['lineStyle']
        line.lineWidth = line.lineWidth * 3
        
        plot.add(line)
        
    plot.save(fileName+".pdf")    


def plotMCSThroughput():
    plot = Plot()

    plot.setXLimits(-10, 25)
    #    plot.setXLimits(5, 10)
    plot.setYLimits(0, 4.5)
    plot.setXLabel("SINR")
    plot.setYLabel("Throughput")

    #(plot.width, plot.height) = (16,9) # getGoldenRatioDimensions(35)

    fileName = "throughputsMCS"

    for mcs in range(29):
        line = boomslang.Line()
        (line.xValues, line.yValues) = getXYfromFile(fileName+".txt", 0, mcs + 1)
        #    print line.xValues, line.yValues
        if mcs==0:
            line.label = "QPSK"
        elif mcs==10:
            line.label = "16 QAM"
        elif mcs==17:
            line.label = "64 QAM"
            
        line.color = mcsStyles[mcs]['color']
        line.lineStyle = mcsStyles[mcs]['lineStyle']
        line.lineWidth = line.lineWidth * 3
        
        plot.add(line)
        
    plot.save(fileName+".pdf")


def plotMCSEfficiency():
    plot = Plot()

    plot.setXLimits(-10, 25)
    #    plot.setXLimits(5, 10)
    plot.setYLimits(0, 6)
    plot.setXLabel("SINR")
    plot.setYLabel("Spectral Efficiency [bits / s / Hz]")

    #(plot.width, plot.height) = (16,9) # getGoldenRatioDimensions(35)

    fileName = "efficienciesMCS"

    for mcs in range(29):
        line = boomslang.Line()
        (line.xValues, line.yValues) = getXYfromFile(fileName+".txt", 0, mcs + 1)
        #    print line.xValues, line.yValues
        if mcs==0:
            line.label = "QPSK"
        elif mcs==10:
            line.label = "16 QAM"
        elif mcs==17:
            line.label = "64 QAM"
            
        line.color = mcsStyles[mcs]['color']
        line.lineStyle = mcsStyles[mcs]['lineStyle']
        line.lineWidth = line.lineWidth * 3
        
        plot.add(line)

    shannon = boomslang.Line()
    shannon.xValues = [x / 10 - 10 for x in range(350)]
    shannon.yValues = [math.log(math.pow(10.0, x/10.0) + 1.0, 2.0) for x in shannon.xValues]
    shannon.label = "Shannon Capacity"
    plot.add(shannon)

    # shannon60 = boomslang.Line()
    # shannon60.xValues = [x / 10 - 10 for x in range(350)]
    # shannon60.yValues = [0.6 * math.log(math.pow(10.0, x/10.0) + 1.0, 2.0) for x in shannon60.xValues]
    # shannon60.label = "60% of Shannon Capacity"
    # plot.add(shannon60)

        
    plot.save(fileName+".pdf")


def plotCQIThroughput():
    plot = Plot()

    plot.setXLimits(-10, 25)
    #    plot.setXLimits(5, 10)
    plot.setYLimits(0, 4.5)
    plot.setXLabel("SINR")
    plot.setYLabel("Throughput")

    #(plot.width, plot.height) = (16,9) # getGoldenRatioDimensions(35)

    fileName = "throughputsCQI"

    for cqi in range(1, 16): #1..15
        line = boomslang.Line()
        (line.xValues, line.yValues) = getXYfromFile(fileName+".txt", 0, cqi)
        #    print line.xValues, line.yValues
        if cqi==1:
            line.label = "QPSK"
        elif cqi==7:
            line.label = "16 QAM"
        elif cqi==10:
            line.label = "64 QAM"
            
        line.color = cqiStyles[cqi]['color']
        line.lineStyle = cqiStyles[cqi]['lineStyle']
        line.lineWidth = line.lineWidth * 3
        
        plot.add(line)
        
    plot.save(fileName+".pdf")

def plotCQIEfficiency():
    plot = Plot()

    plot.setXLimits(-10, 25)
    #    plot.setXLimits(5, 10)
    plot.setYLimits(0, 6.0)
    plot.setXLabel("SINR")
    plot.setYLabel("Spectral Efficiency [bits / s / Hz]")

    #(plot.width, plot.height) = (16,9) # getGoldenRatioDimensions(35)

    fileName = "efficienciesCQI"

    for cqi in range(1, 16): #1..15
        line = boomslang.Line()
        (line.xValues, line.yValues) = getXYfromFile(fileName+".txt", 0, cqi)
        #    print line.xValues, line.yValues
        if cqi==1:
            line.label = "QPSK"
        elif cqi==7:
            line.label = "16 QAM"
        elif cqi==10:
            line.label = "64 QAM"
            
        line.color = cqiStyles[cqi]['color']
        line.lineStyle = cqiStyles[cqi]['lineStyle']
        line.lineWidth = line.lineWidth * 3
        
        plot.add(line)

    shannon = boomslang.Line()
    shannon.xValues = [x / 10 - 10 for x in range(350)]
    shannon.yValues = [math.log(math.pow(10.0, x/10.0) + 1.0, 2.0) for x in shannon.xValues]
    shannon.label = "Shannon Capacity"
    plot.add(shannon)
    
        
    plot.save(fileName+".pdf")


# TODO: this is basically always the same, at least the throughput/efficiency plots should be done by a sinlge method
#plotMCSThroughput()
#plotCQIThroughput()
plotMCSEfficiency()
plotCQIEfficiency()
plotMCSBlers()
plotCQIBlers()
