#!/usr/bin/python
import sys
import os
sys.path.insert(0,"/usr/local")
class PlotParameters :
  probeName = ['ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF']
  confidence = False
  aggregate = False
  originalPlots = False
  aggrParam = ''
  fileName = 'experiment8_throughput_clients'
  type = 'Param'
  campaignId = '2136'
  xLabel = 'load'
  yLabel = 'mean of Aggregated throughput [Bit/s]'
  confidenceLevel = 0.95
  yLabel = 'mean of Aggregated throughput [Bit/s]'
  parameterName = 'load'
  probeEntry = 'mean'
  useXProbe = False
  useYProbe = True
  filterExpression = 'load in [0.01, 0.025, 0.0375, 0.05, 0.075, 0.1, 0.125, 0.15, 0.175, 0.2, 0.225, 0.25, 0.3, 0.4, 0.5, 0.75, 1.0] and experiment in ["Experiment7", "Experiment8"] and nStations in [2, 10, 50]'
  scaleFactorX = 1 #1/1e6 #bit to MBit
  scaleFactorY = 1 #1/1e6 #bit to MBit
  doClip = True
  minX = 0.0 * scaleFactorX 
  maxX = 1.0 * scaleFactorX 
  minY = 0.0 * scaleFactorY 
  maxY = 1200000.0 * scaleFactorY 
  moveX = 0
  moveY = 0
  grid = (True, False, True, False)
  scale = ('linear', None, 'linear', None)
  marker = '.'
  legend = False
  legendPosition = 'best' #alternatives: upper right, upper left, lower left, lower right, right, center left, center right, lower center, upper center, center or (x,y) with x,y in [0-1]
  showTitle = False
  figureTitle = 'Parameter Figure '
  color = True
  legendLabelMapping = {
    "ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment7; nStations: 2":"ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment7; nStations: 2" , #graph 0
    "ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment7; nStations: 10":"ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment7; nStations: 10" , #graph 1
    "ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment7; nStations: 50":"ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment7; nStations: 50" , #graph 2
    "ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment8; nStations: 2":"ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment8; nStations: 2" , #graph 3
    "ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment8; nStations: 10":"ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment8; nStations: 10" , #graph 4
    "ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment8; nStations: 50":"ip.endToEnd.window.aggregated.bitThroughput_DLL.StationType_client_PDF; experiment: Experiment8; nStations: 50" , #graph 5
  }
  plotOrder = [0, 1, 2, 3, 4, 5]
  #plotOrder = [0,2,1,3] # plot first graph 0 , then graph 2 , ...
  color_styles = ['b-', 'g-', 'r-', 'c-', 'm-','b--', 'r--', 'g--', 'c--', 'm--']
  bw_markers = ['+','.','*','x','o','v','^','<','>','s','p','*','h','H','D','d',',','|']
  #additional plots are defined as done here
  additional_plots = [
    #{'x': [1,400], 'y':[200,200], 'label':'a horizontal line' , 'style':'b--'},
    #{'x': [1,400], 'y':[1*.98,400*0.98], 'label':'98 % line' , 'style':'g--'}
  ]
  outputFormats = [ 'png', 'pdf']

import openwns.wrowser.FigurePlotter
if __name__ == '__main__': openwns.wrowser.FigurePlotter.loadCampaignAndPlotGraphs(PlotParameters)

