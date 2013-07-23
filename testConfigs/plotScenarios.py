#!/usr/bin/env python

import math
import numpy
import matplotlib
matplotlib.use('PDF') # to avoid the matplotlib GUI from being opened
import pylab

fontSize = 8

fileType = ".pdf"

golden_mean = (math.sqrt(5)-1.0)/2.0  # Aesthetic ratio  
width = 6.5/2.54# width in inches   
height = width*golden_mean   # height in inches


matplotlib.rcParams['xtick.labelsize'] = fontSize
matplotlib.rcParams['ytick.labelsize'] = fontSize
matplotlib.rcParams['legend.fontsize'] = fontSize
matplotlib.rcParams['axes.labelsize'] = fontSize
matplotlib.rcParams['text.usetex'] = True
matplotlib.rcParams['ps.useafm'] = True
matplotlib.rcParams['ps.usedistiller'] = 'xpdf'
matplotlib.rcParams['font.family'] = "serif"
matplotlib.rcParams["font.serif"] = "Times, Palatino, New Century Schoolbook, Bookman, Computer Modern Roman"
matplotlib.rcParams["font.sans-serif"] = "Helvetica, Avant Garde, Computer Modern Sans serif"
matplotlib.rcParams["font.cursive"] = "Zapf Chancery"
matplotlib.rcParams["font.monospace"] = "Courier, Computer Modern Typewriter"
matplotlib.rcParams["lines.linewidth"] = 1.2
matplotlib.rcParams["patch.linewidth"] = 0.35  # Patches are graphical objects that fill 2D space, like polygons or circles. 
matplotlib.rcParams["axes.linewidth"] = 0.5
matplotlib.rcParams["grid.linewidth"] = 0.35


# defines a custom colormap that has a different color for the 0 entry
# which basically is the color for non-existing values. 
# it is the first line for each red/green/blue and can be set to black (0,0,0)
# or white (0,1,1)
# http://www.scipy.org/Cookbook/Matplotlib/Show_colormaps

#        based on original jet color map from matplotlib's _cm.py
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

my_cmap = matplotlib.colors.LinearSegmentedColormap('my_colormap',cdict,256)




def plotMapWithTrials(filename, colorMap, scaleLabel, interpolation, fillValue, minValue, maxValue, gridTicks, colorNorm = None, zoom = None):
    # http://docs.scipy.org/doc/numpy/reference/generated/numpy.loadtxt.html
    map_raw = numpy.loadtxt(filename + "_max.m", comments = '%')
    trials_raw = numpy.loadtxt(filename + "_trials.m", comments = '%')

    # create a recarray from a list of records in text form
    map_parsed = numpy.rec.fromrecords(map_raw, names = 'x,y,z')
    trials_parsed = numpy.rec.fromrecords(trials_raw, names = 'x,y,z')

    numXEntries = len(numpy.unique(map_parsed['x']))
    minX = min(map_parsed['x'])
    maxX = max(map_parsed['x'])

    numYEntries = len(numpy.unique(map_parsed['y']))
    minY = min(map_parsed['y'])
    maxY = max(map_parsed['y'])

    if not zoom is None:
        assert (zoom.minX >= minX)
        minX = zoom.minX
        assert (zoom.maxX <= maxX)
        maxX = zoom.maxX
        assert (zoom.minY >= minY)
        minY = zoom.minY
        assert (zoom.maxY <= maxY)
        maxY = zoom.maxY
        minXKey = map_parsed['x'][0]
        maxXKey = map_parsed['x'][numYEntries*numXEntries - 1]
        minYKey = map_parsed['y'][0]
        maxYKey = map_parsed['y'][numYEntries - 1]
        xBins = (maxXKey - minXKey) / (numXEntries - 1)
        yBins = (maxYKey - minYKey) / (numYEntries - 1)
        minXIndex = (zoom.minX - minXKey) / xBins
        minYIndex = (zoom.minY - minYKey) / yBins
        maxXIndex = (zoom.maxX - minXKey) / xBins
        maxYIndex = (zoom.maxY - minYKey) / yBins


    print "The map is (%dx%d)" % (numYEntries+1, numXEntries+1)
    map = numpy.ones((numYEntries, numXEntries)) * fillValue

    i = 0
    assert (numXEntries * numYEntries  == len(map_parsed['x']))
    for xx in xrange(numXEntries):
        for yy in xrange(numYEntries):
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

    if not zoom is None:
        zoomMap =  map[minYIndex:maxYIndex,minXIndex:maxXIndex]
        map = zoomMap


    fig = matplotlib.pyplot.figure(figsize=(width,height))

    #http://matplotlib.sourceforge.net/api/pyplot_api.html#matplotlib.pyplot.imshow
    axes = matplotlib.pyplot.axes()

    axes.xaxis.set_major_locator(matplotlib.ticker.MultipleLocator(gridTicks))
    axes.yaxis.set_major_locator(matplotlib.ticker.MultipleLocator(gridTicks))
    axes.xaxis.set_label_text(r"$x$ in m")
    axes.yaxis.set_label_text(r"$y$ in m")

    im = axes.imshow(map, cmap = colorMap, #pylab.cm.gray, my_cmap
                     origin = 'lower',
                     interpolation = interpolation,
                     norm = colorNorm,
                     extent= (minX, maxX,
                              minY, maxY)
    )

    cbar = matplotlib.pyplot.colorbar(im, ax=axes)
    cbar.set_label(scaleLabel)

    matplotlib.pyplot.savefig(filename + fileType,
                              dpi = 300,
                              transparent = True,
                              bbox_inches = "tight",
                              pad_inches = 0.01)



TUMNamedColorsHex = {
    'TUM-Blau': "#0065bd",
    'weiss': "#ffffff",
    'schwarz': "#000000",
    'akzent1': "#a2ad00",
    'akzent2': "#dad7cb",
    'akzent3': "#e37222",
    'komplex1': "#69085a",
    'komplex2': "#0f1b5f",
    'komplex3': "#003359",
    'komplex4': "#005293",
    'komplex5': "#0073cf",
    'komplex6': "#64a0c8",
    'komplex7': "#98c6ea",
    'komplex8': "#00778a",
    'komplex9': "#007c30",
    'komplex10': "#679a1d",
    'komplex11': "#a2ad00",
    'komplex12': "#dad7cb",
    'komplex13': "#ffdc00",
    'komplex14': "#f9ba00",
    'komplex15': "#e37222",
    'komplex16': "#d64c13",
    'komplex17': "#c4071b",
    'komplex18': "#9c0d16",
    }

TUMNumberedColorsHex = {
    0: "#000000",
    1: "#0065bd",
    2: "#69085a",
    3: "#0f1b5f",
    15: "#003359",
    16: "#005293",
    17: "#0073cf",
    18: "#64a0c8",
    19: "#98c6ea",
    4: "#00778a",
    5: "#007c30",
    6: "#679a1d",
    7: "#a2ad00",
    8: "#dad7cb",
    9: "#ffdc00",
    10: "#f9ba00",
    11: "#e37222",
    12: "#d64c13",
    13: "#c4071b",
    14: "#9c0d16",


    }

import random

tumColors = []
for i in range(20):
    tumColors.append(TUMNumberedColorsHex[i])
tumColors.append(TUMNamedColorsHex['TUM-Blau'])
random.seed(2011)
random.shuffle(tumColors)

tumColors.append(tumColors[20])


cm = matplotlib.colors.ListedColormap(tumColors)

plotMapWithTrials("output/avgSINR_DL",
                  colorMap = my_cmap,
                  scaleLabel = "Geometry in dB", 
                  interpolation = 'nearest',
                  fillValue = 0,
                  minValue = float("-inf"),
                  maxValue = float("inf"),
                  gridTicks = 250,
                  zoom = None
                 )



plotMapWithTrials("output/servingBS_DL",
                  colorMap = cm,
                  scaleLabel = "Serving Cell", 
                  interpolation = 'nearest',
                  fillValue = 0,
                  minValue = float("-inf"),
                  maxValue = float("inf"),
                  gridTicks = 250,
                  colorNorm = matplotlib.colors.BoundaryNorm(range(1,23), cm.N),
                  zoom = None 
                 )


