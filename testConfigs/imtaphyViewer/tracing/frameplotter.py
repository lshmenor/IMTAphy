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

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg
import matplotlib.figure
import matplotlib.patches
from PyQt4 import QtCore, QtGui

class FramePlotter(FigureCanvasQTAgg):

    """This class implements a QT Widget on which you can draw using the
    MATLAB(R)-style commands provided by matplotlib
    """
    def __init__(self, db, parent=None, width=5, height=4, dpi=100):
        self.figure = matplotlib.figure.Figure(figsize=(width, height), dpi=dpi)
        self.axes = self.figure.add_subplot(111)
        self.axes.hold(False)

        FigureCanvasQTAgg.__init__(self, self.figure)

        FigureCanvasQTAgg.setSizePolicy(self,
                                        QtGui.QSizePolicy.Expanding,
                                        QtGui.QSizePolicy.Expanding)
        FigureCanvasQTAgg.updateGeometry(self)

        self.db = db
        # Stale OK disables regeneration of index
        # Just keep in mind that this application is read-only
        # If you modify the db, you need to rebuild the index!!!
        self.data = self.db.execute_view("orderByStartTime", design_doc="wrowser")#, stale='ok')
        self.sendersView = self.db.execute_view("senders", design_doc="wrowser", group=True)#, stale='ok')
        self.receiversView = self.db.execute_view("receivers", design_doc="wrowser", group=True)#, stale='ok')
        
        self.senders = []
        self.receivers = []
        self.customFilter = None
        self.customFilterCheckbox = None

        for row in self.sendersView:
            self.senders.append(row.key)

        for row in self.receiversView:
            self.receivers.append(row.key)

            
        self.radioFrameDuration = 0.010
        self.subFrameDuration = 0.001
        self.tolerance = self.subFrameDuration / 14
        self.startFrame = 0
        self.numFrames = 1
        self.activeWindows = []

        self._makeConnects()

    def _makeConnects(self):
        self.figure.gca().get_figure().canvas.mpl_connect('pick_event', self.onPicked)

    def onPicked(self, event):
        self.emit(QtCore.SIGNAL("itemPicked"), event.artist.data)

    def on_radioFrameChanged(self, start, numFrames):
        self.figure.clear()
        self.startFrame = start
        self.numFrames = numFrames
        self.plotRadioFrame()
        self.figure.canvas.draw()

    def plotRadioFrame(self):
        startTime = (self.subFrameDuration * self.startFrame) - self.tolerance
        stopTime = startTime + self.radioFrameDuration * self.numFrames + 2 * self.tolerance

        selectedSenders = self.getSelectedSenders()
        selectedReceivers = self.getSelectedReceivers()

        multiEntries = {}
        maxPRB = 0
        
        for entry in self.data[startTime:stopTime]:
            # do not plot all transmissions by default -> takes too long
            
            #if len(selectedSenders) > 0:
            # Only then we filter by sender
            if not entry.value["Transmission"]["Sender"] in selectedSenders:
                    continue

            if len(selectedReceivers) > 0:
            # Only then we filter by sender
                if not entry.value["Transmission"]["Receiver"] in selectedReceivers:
                    continue


            if not self.applyCustomFilter(entry):
                continue

            v = entry.value

            # there can be multiple entries for the same time/frequency grid
            # in that case, divide the display into as many time domain bins
            if not (v["Transmission"]["Start"], v["Transmission"]["PRB"]) in multiEntries:
                multiEntries[v["Transmission"]["Start"], v["Transmission"]["PRB"]] = []
            multiEntries[v["Transmission"]["Start"], v["Transmission"]["PRB"]].append(v)
            
            entries = multiEntries[v["Transmission"]["Start"], v["Transmission"]["PRB"]]

            entries.sort()
            for i in range(len(entries)):
                v = entries[i]
                theColor = self._getColor(v)

                duration = 1.0 #v["Transmission"]["Stop"] - v["Transmission"]["Start"]

                if v["Transmission"]["PRB"] > maxPRB:
                    maxPRB = v["Transmission"]["PRB"]
                rect=matplotlib.patches.Rectangle((v["Transmission"]["Start"]*1000 + float(i)*duration /float(len(entries)),
                                               v["Transmission"]["PRB"]),
                                              duration  / float(len(entries)),
                                              1,
                                              facecolor=theColor[0], edgecolor=theColor[1], picker=True)
                rect.data=entry
                self.figure.gca().add_patch(rect)
        
        if len(self.figure.axes)>0:
            maxPRB = ((maxPRB // 25) + 1) * 25 
            ax = self.figure.axes[0]
            ax.set_xlim(1000.0 * (startTime + self.tolerance), 
                        1000.0 * (stopTime - self.tolerance))
            ax.set_ylim(0, maxPRB)
            
            ax.yaxis.grid(True, 'minor', linestyle=':', linewidth=0.5)
#            ax.yaxis.grid(True, 'major', linestyle=':')
            ax.xaxis.grid(True, 'minor', linestyle=':', linewidth=0.5)
            ax.xaxis.grid(True, 'major', linestyle='-', linewidth=1)
            ax.xaxis.set_major_locator(matplotlib.ticker.MultipleLocator(10)) # for the 10ms radio frames
            ax.xaxis.set_minor_locator(matplotlib.ticker.MultipleLocator(1)) # for the 1ms TTIs            
            ax.yaxis.set_major_locator(matplotlib.ticker.MultipleLocator(5* (maxPRB // 25))) 
            ax.yaxis.set_minor_locator(matplotlib.ticker.MultipleLocator(1)) # for each PRB
            ax.set_title("Radioframe starting at subframe %d" % self.startFrame)
            ax.set_xlabel("TTI (1 ms)")
            ax.set_ylabel("PRB (180 kHz)")

    def _getColor(self, v):
        # returns (facecolor, edgecolor)
        import re
        if v["Transmission"]["Sender"].startswith("eNB"):
            m=re.match('^\D*(\d*)$', v["Transmission"]["Receiver"])
        else:
            m=re.match('^\D*(\d*)$', v["Transmission"]["Sender"])

        userID = int(m.group(1))
        
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
            20: "#101010",
            21: "#202020",
            22: "#303030",
            23: "#404050",
            24: "#505050",
            25: "#606060",
            26: "#707070",
            27: "#808080",
            28: "#909090",
            29: "#a0a0a0",
            30: "#b0b0b0",
            31: "#c0c0c0",
            31: "#d0d0d0",
            32: "#e0e0e0",
        }

        wrapped = userID % 1000

        r = 0.3 + (wrapped % 10) * 0.07
        g = 0.3 + ((wrapped / 10) % 10) * 0.07
        b = 0.3 + ((wrapped / 100) % 10) * 0.07
        facecolor = (r,g,b)

        facecolor = TUMNumberedColorsHex[userID % 32 + 1] # avoid 0 which is black
        
        edgecolor = (0.0, 0.0, 0.0)
        if v.has_key("Transmission") and v["Transmission"].has_key("HARQ.NDI"):
            if not v["Transmission"]["HARQ.NDI"]:
                edgecolor = (0.8, 0.0, 0.0)

        return (facecolor, edgecolor)

    def setSelectors(self, senderSelector, receiverSelector, customFilterCheckbox, customFilter):
        self.senderSelector = senderSelector
        self.receiverSelector = receiverSelector
        self.customFilterCheckbox = customFilterCheckbox
        self.customFilter = customFilter

    def getSelectedSenders(self):
        selectedSenders = []
        if self.senderSelector is not None:
            items = self.senderSelector.selectedItems()
            for it in items:
                if it.isSelected():
                    selectedSenders.append(str(it.data(0).toString()))
        return selectedSenders

    def getSelectedReceivers(self):
        selectedReceivers = []
        if self.receiverSelector is not None:
            items = self.receiverSelector.selectedItems()
            for it in items:
                if it.isSelected():
                    selectedReceivers.append(str(it.data(0).toString()))
        return selectedReceivers

    def applyCustomFilter(self, entry):
        
        if self.customFilter is None:
            return True

        if self.customFilterCheckbox.checkState() == QtCore.Qt.Unchecked:
            return True

        code = str(self.customFilter.document().toPlainText())

        g ={}
        l={}
        
        try:
            exec code in g,l
        except:
            import sys
            m = ""
            info = sys.exc_info()
            m += "!!!Unexpected Error of type %s!!!" % info[0]
            m += str(info[1])

            if info[2] is not None:
                import traceback
                for entry in traceback.format_tb(info[2]):
                    entry = entry.rstrip("\n")
                    for line in entry.split("\n"):
                        m += line
            print m
  
            f = self.customFilter.document().defaultFont()
            f.setUnderline(True)
            self.customFilter.document().setDefaultFont(f)
            return True

        try:
            toBeCalled = l["filter"]
            r = toBeCalled(entry.key, entry.value)

            f = self.customFilter.document().defaultFont()
            f.setUnderline(False)
            self.customFilter.document().setDefaultFont(f)      
            return r
        except:
            import sys
            m = ""
            info = sys.exc_info()
            m += "!!!Unexpected Error of type %s!!!" % info[0]
            m += str(info[1])

            if info[2] is not None:
                import traceback
                for entry in traceback.format_tb(info[2]):
                    entry = entry.rstrip("\n")
                    for line in entry.split("\n"):
                        m += line
            print m
            #QtGui.QMessageBox.critical(self,
            #                           "Error executing your filter",
            #                           m)

            f = self.customFilter.document().defaultFont()
            f.setUnderline(True)
            self.customFilter.document().setDefaultFont(f)
            return True

        f = self.customFilter.document().defaultFont()
        f.setUnderline(False)
        self.customFilter.document().setDefaultFont(f)

        return True
