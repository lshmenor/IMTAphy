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

from PyQt4 import QtCore, QtGui
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg
import matplotlib.figure
import os

import inspect

class FigureCanvas(FigureCanvasQTAgg):
    """This class implements a QT Widget on which you can draw using the
    MATLAB(R)-style commands provided by matplotlib
    """
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig = matplotlib.figure.Figure(figsize=(width, height), dpi=dpi)
        self.axes = self.fig.add_subplot(111)
        self.axes.hold(False)

        FigureCanvasQTAgg.__init__(self, self.fig)

        FigureCanvasQTAgg.setSizePolicy(self,
                                   QtGui.QSizePolicy.Expanding,
                                   QtGui.QSizePolicy.Expanding)
        FigureCanvasQTAgg.updateGeometry(self)

    def sizeHint(self):
        w, h = self.get_width_height()
        return QtCore.QSize(w, h)

    def minimumSizeHint(self):
        return QtCore.QSize(10, 10)

    def clear(self):
        self.fig.clear()
        self.axes = self.fig.add_subplot(111)
        self.axes.hold(False)

    def registerMotionEventHandler(self, callable):
        #self.axes.set_picker(True)
        self.fig.canvas.mpl_connect('motion_notify_event', callable)

    def registerButtonPressEventHandler(self, callable):
        self.fig.canvas.mpl_connect('button_press_event', callable)

class ViewScenario(QtGui.QDockWidget):

    from ui.Widgets_ViewScenario_ui import Ui_Widgets_ViewScenario
    import scenario.generic
    class ViewScenarioWidget(QtGui.QWidget, Ui_Widgets_ViewScenario):
        
        def __init__(self, configFilename, mainWindow, *args):
            QtGui.QWidget.__init__(self, *args)
            self.configFilename = configFilename
            self.workingDir = os.path.dirname(self.configFilename)
            self.inspector = inspect.ConfigInspector(self.configFilename)
            self.mainWindow = mainWindow
#            self.simulationThread = threads.SimulationThread(self)
            self.progressTimeout = QtCore.QTimer(self)
            self.progressTimeout.setInterval(1000)

#            QtCore.QObject.connect(self.simulationThread, QtCore.SIGNAL("finished()"), self, QtCore.SLOT("on_simulationThread_finished()"))
#            QtCore.QObject.connect(self.progressTimeout, QtCore.SIGNAL("timeout()"), self, QtCore.SLOT("on_progressTimeout_timeout()"))
            self.setupUi(self)
            self.updateFileList()

            self.fillValueLineEdit.setValidator(QtGui.QDoubleValidator(self))

        @QtCore.pyqtSignature("bool")
        def on_redrawButton_clicked(self, checked):
            self.update()            

        def updateFileList(self):
            import Probe
            self.fileList.clear()
#<<<<<<< TREE old LKN code:
#            
#            self.viewScenarioProbes = []
#            dirname = self.workingDir + '/output/'
#            for root, dirs, files in os.walk(dirname):
#                for ff in files:
#                    filename = os.path.join(dirname, ff)
#                    for infix in ['_mean', '_max']: #wrowser.Probe.TableProbe.fileNameSigs:
#                        if infix in filename:
#                            self.viewScenarioProbes.append(ff)
#
#            self.fileList.addItems(sorted(self.viewScenarioProbes))
#=======

            if os.path.exists(self.workingDir + '/output/'):
                self.viewScenarioProbes = Probe.readAllProbes(self.workingDir + '/output/')
            else:
                self.viewScenarioProbes = {}

            doNotShowThese = []
            for k,v in self.viewScenarioProbes.items():
                if v.probeType is not 'Table':
                    doNotShowThese.append(k)
            for k in doNotShowThese:
                self.viewScenarioProbes.pop(k)

            self.fileList.addItems(self.viewScenarioProbes.keys())
#>>>>>>> MERGE-SOURCE

        def update(self):
            if self.fileList.currentItem() is not None:
                fileToPlot = str(self.fileList.currentItem().text())
                path = os.path.join(self.workingDir, 'output', fileToPlot)
                fillValue = float(self.fillValueLineEdit.text())
                includeContour = self.contourPlotCheckBox.isChecked()
                try:
                    minValue = float(self.minValue.text())
                except:
                    minValue = float("-inf")
                try:
                    maxValue = float(self.maxValue.text())
                except:
                    maxValue = float("+inf")
                self.mainWindow.updateScenarioView(path, fillValue, minValue, maxValue, includeContour)

        # @QtCore.pyqtSignature("bool")
        # def on_scanWinnerButton_clicked(self, checked):
        #     if not self.simulationThread.isRunning():
        #         self.scanWinnerButton.setEnabled(False)
        #         self.scanWinnerButton.setText("Scanning ( 0.00 % )")
        #         self.simulationThread.start()
        #         self.progressTimeout.start()
        #     else:
        #         QtGui.QMessageBox.critical(self,
        #                                    "An error occured",
        #                                    "There is already a simulation running.")

        # @QtCore.pyqtSignature("")
        # def on_simulationThread_finished(self):
        #     self.scanWinnerButton.setEnabled(True)
        #     self.progressTimeout.stop()
        #     self.scanWinnerButton.setText("Scan")

        #     if self.simulationThread.success:
        #         self.updateFileList()
        #         self.update()
        #     else:
        #         QtGui.QMessageBox.critical(self,
        #                                    "An error occured",
        #                                    "An error occured when executing the simulator")

        @QtCore.pyqtSignature("")
        def on_progressTimeout_timeout(self):
            if os.path.exists('output/progress'):
                progressFile = open('output/progress')
                progress = float(progressFile.read()) * 100
                progressFile.close()
                self.scanWinnerButton.setText("Scan in progress ( %.2f %%)" % progress)

        # def on_p1PickerButton_clicked(self):
        #     if self.p1PickerButton.isChecked():
        #         self.p2PickerButton.setChecked(False)

        # def on_p2PickerButton_clicked(self):
        #     if self.p2PickerButton.isChecked():
        #         self.p1PickerButton.setChecked(False)

        # def on_motionEvent(self, event):
        #     if self.p1PickerButton.isChecked():
        #         if event.xdata is not None and event.ydata is not None:
        #             self.x1LineEdit.setText(str("%.2f" % event.xdata))
        #             self.y1LineEdit.setText(str("%.2f" % event.ydata))

        #     if self.p2PickerButton.isChecked():
        #         if event.xdata is not None and event.ydata is not None:
        #             self.x2LineEdit.setText(str("%.2f" % event.xdata))
        #             self.y2LineEdit.setText(str("%.2f" % event.ydata))

        # def on_buttonPressEvent(self, event):
        #     self.p1PickerButton.setChecked(False)
        #     self.p2PickerButton.setChecked(False)

        @QtCore.pyqtSignature("bool")
        def on_mapCutPlotPushbutton_clicked(self, checked):
            if self.fileList.currentItem() is not None:
                fileToPlot = str(self.fileList.currentItem().text())
                path = os.path.join(self.workingDir, 'output', fileToPlot)
                fillValue = float(self.fillValueLineEdit.text())
                x1 = float(self.x1LineEdit.text())
                y1 = float(self.y1LineEdit.text())
                x2 = float(self.x2LineEdit.text())
                y2 = float(self.y2LineEdit.text())
                self.mainWindow.updateCutPlot(path, fillValue, x1, y1, x2, y2)
            else:
                QtGui.QMessageBox.critical(self,
                                           "An error occured",
                                           "You need to select a map data source first on the Map Plotting tab")

    def __init__(self, configFilename, parent, *args):
        QtGui.QDockWidget.__init__(self, "View Scenario", parent, *args)
        self.internalWidget = self.__class__.ViewScenarioWidget(configFilename, parent, self)
        self.setWidget(self.internalWidget)
