#!/usr/bin/python
import sys
import os
sys.path.insert(0,"/usr/local")
import imp
import openwns.wrowser.FigurePlotter
import os

for fileName in os.listdir('.'):
    if fileName.endswith('.py') and fileName!='plotAll.py' :
        print "file:",fileName
        try:
            module = imp.load_module('PlotParameters', file(fileName), '.', ('.py', 'r', imp.PY_SOURCE))
            #module.PlotParameters.color = False #parameter is modified for all plots
            print "going to plot the figure"
            openwns.wrowser.FigurePlotter.loadCampaignAndPlotGraphs(module.PlotParameters)
        except ImportError :
            print "this file does not contain the class 'PlotParameters'"
