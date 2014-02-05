#!/usr/bin/env python
import numpy
import matplotlib.colors
import matplotlib.cm as cm
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
import sys


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


cdict  =   {'red':   (
                      (0., 0, 0), 
                      (0.35, 0, 0), 
                      (0.66, 1, 1), 
                      (0.89,1, 1),
                      (1, 0.5, 0.5)),
            'green': (
                      (0., 0, 0), 
                      (0.125,0, 0), 
                      (0.375,1, 1), 
                      (0.64,1, 1),
                      (0.91,0,0), 
                      (1, 0, 0)),
            'blue':  (
                      (0., 0.5, 0.5), 
                      (0.11, 1, 1), 
                      (0.34, 1, 1), 
                      (0.65,0, 0),
                      (1, 0, 0))}



try:
    fileToPlot = sys.argv[1]
except:
    print "Usage: plotChannel.py filename"
    print "E.g.:  plotChannel.py output/channelGain_UE10_antennaPair1_max.m"
    sys.exit()


minValue = -30
maxValue = 30
fillValue = minValue - 1

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
    sys.exit()
    
map_parsed = numpy.rec.fromrecords(map_raw, names = 'x,y,z')
trials_parsed = numpy.rec.fromrecords(trials_raw, names = 'x,y,z')
numXEntries = len(numpy.unique(map_parsed['x']))
minX = min(map_parsed['x'])
maxX = max(map_parsed['x'])

numYEntries = len(numpy.unique(map_parsed['y']))
minY = min(map_parsed['y'])
maxY = max(map_parsed['y'])

print "The map is (%dx%d)" % (numYEntries+1, numXEntries+1)
map = numpy.ones((numYEntries, numXEntries)) * fillValue

axes = matplotlib.pyplot.axes()
axes.xaxis.set_label_text("TTIs")
axes.yaxis.set_label_text("PRBs")



i = 0
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
            map[yy][xx] = fillValue #minValue - 0.0001
        i += 1

my_cmap = matplotlib.colors.LinearSegmentedColormap('my_colormap',cdict,256)
im = axes.imshow(map, cmap = my_cmap,
                origin = 'lower',
                 interpolation = "nearest"
                )

cbar = matplotlib.pyplot.colorbar(im, ax=axes)
cbar.set_label("Fast-fading gain [dB]")


matplotlib.pyplot.savefig(filename + ".pdf",
                          dpi = 300,
                          transparent = True,
                          bbox_inches = "tight",
                          pad_inches = 0.02)
#plt.show()

 
