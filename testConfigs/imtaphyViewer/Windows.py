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

from PyQt4 import QtCore, QtGui

import Widgets

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg
#import inspect
import pprint
import datetime

import scenario
import scenario.plotterFactory
import scenario.widgets

# Check prerequisites
try:
    import gnomekeyring
    import desktopcouch.records
    couchIsUsable = True
    couchHasUsableKeyring = True
except ImportError:
    couchIsUsable = False
    couchHasUsableKeyring = True
except gnomekeyring.IOError:
    couchIsUsable = False
    couchHasUsableKeyring= False

if couchIsUsable:
    import tracing.frameplotter
    import tracing.model

class Observable(object):

    def __init__(self):
        self.emitter = QtCore.QObject()

    def __setattr__(self, name, value):
        object.__setattr__(self, name, value)
        self.emitter.emit(QtCore.SIGNAL("changed"), name, value, self)
        self.emitter.emit(QtCore.SIGNAL(name + "_changed"), value)

class Observing:

    def observe(self, callable, subject, attribName = ""):
        if attribName != "":
            self.connect(subject.emitter, QtCore.SIGNAL(attribName + "_changed"), callable)
        else:
            self.connect(subject.emitter, QtCore.SIGNAL("changed"), callable)


from ui.Dialogues_OpenCouchDatabase_ui import Ui_CouchDBDialog
class OpenCouchDatabase(QtGui.QDialog, Ui_CouchDBDialog):
    def __init__(self, *args):
        QtGui.QDialog.__init__(self, *args)

        self.setupUi(self)

        self._readDatabases()

        self.connect(self.importButton, QtCore.SIGNAL("clicked()"), self.onImportClicked)

    def _readDatabases(self):
        import desktopcouch.records.server
        import couchdb.client
        port = desktopcouch.find_port()
        s = desktopcouch.records.server.OAuthCapableServer('http://localhost:%s/' % port)
        self.listWidget.clear()
        for dbname in s:
            self.listWidget.addItem(dbname)

    def onImportClicked(self):
        import tracing.model
        fileDialogue = QtGui.QFileDialog(self, "Select a Probe to import", os.getcwd(), "Probe files (*.dat)")
        fileDialogue.setAcceptMode(QtGui.QFileDialog.AcceptOpen)
        fileDialogue.setFileMode(QtGui.QFileDialog.ExistingFile)
        fileDialogue.setViewMode(QtGui.QFileDialog.Detail)
        if fileDialogue.exec_() == QtGui.QDialog.Accepted:
            fileName = fileDialogue.selectedFiles()[0]
        else:
            return

        dbName = "unnamed"
        r = QtGui.QInputDialog.getText(self, "Give a name for the new database", "Datbase name:", QtGui.QLineEdit.Normal, dbName)
        if (r[1] and r[0] != ""):
            dbName = r[0]

        tracing.model.importFile(str(fileName), str(dbName))

        self._readDatabases()

    def getDatabase(self):
        i = self.listWidget.selectedItems()
        if len(i)!=1:
            return None
        return i[0].text()

    def contextMenuEvent(self, event):
        import tracing.model
        items = self.listWidget.selectedItems()
        dbname = items[0].data(0).toString()
        
        menu = QtGui.QMenu(self)
        deleteAction = menu.addAction("Delete")
        action = menu.exec_(self.mapToGlobal(event.pos()))

        if action == deleteAction:
            msg = QtGui.QMessageBox()
            msg.setText("<b>%s</b> will be permanently deleted!" % dbname)
            msg.setInformativeText("Do your really want to delete?")
            msg.setStandardButtons(QtGui.QMessageBox.Yes|QtGui.QMessageBox.Cancel)
            msg.setDefaultButton(QtGui.QMessageBox.Cancel)
            ret = msg.exec_()

            if ret == QtGui.QMessageBox.Yes:
                tracing.model.deleteDB(str(dbname))
                self._readDatabases()




class ProgressStatus(QtGui.QProgressBar):
    def __init__(self, progressLabel,  *args):
        QtGui.QProgressBar.__init__(self, *args)
        self.progressLabel = progressLabel
        self.labelText=""
        self.labelLength=80
        self.setSizePolicy(QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed,QtGui.QSizePolicy.Fixed))
        self.reset()

    def reset(self):
        self.setMinimum(0)
        self.startTime = datetime.datetime.now()
        QtGui.QProgressBar.reset(self)

    def setCurrentAndMaximum(self, current, maximum, additionalText = ""):
        import Time
        
        self.setMaximum(maximum)
        self.setValue(current)
        labelText = self.labelText
        timeDelta = ""
        if len(additionalText) > 0:
            additionalText = additionalText.replace('\n',' ')
            labelText += " " + additionalText
        if maximum > 0 and float(current)/maximum >= 0.01:
            elapsed = datetime.datetime.now() - self.startTime
            total = elapsed * maximum / current
            remaining = total - elapsed
            timeDelta = ".. approx. " + Time.Delta(remaining).asString() + " left"
        if QtGui.QApplication.hasPendingEvents():
            QtGui.QApplication.instance().syncX()
            QtGui.QApplication.instance().processEvents()
            if len(labelText) > self.labelLength:
                labelText = timeDelta
            else:
                labelText += timeDelta 
            self.progressLabel.setText(labelText)
        time.sleep(0.006)




from ui.Dialogues_Warning_ui import Ui_Dialogues_Warning
class Warning(QtGui.QDialog, Ui_Dialogues_Warning):
    def __init__(self, parent, entryname, title, message, *args):
        QtGui.QDialog.__init__(self, *args)
        self.setupUi(self)

        self.setWindowTitle(title)
        self.label.setText(message)
        self.entryname = entryname

        self.exec_()

    def exec_(self):
        if self.disabled == "0":
            r = QtGui.QDialog.exec_(self)








from ui.Windows_Main_ui import Ui_Windows_Main
class Main(QtGui.QMainWindow, Ui_Windows_Main):

    class CancelFlag:
        cancelled = False

    def __init__(self, *args):
        QtGui.QMainWindow.__init__(self, *args)
        self.setupUi(self)
        self.showMaximized()
        self.reader = None

        self.readerStopped = False


        self.workspace = QtGui.QWorkspace(self)
        self.setCentralWidget(self.workspace)

        self.windowMapper = QtCore.QSignalMapper(self)
        self.connect(self.windowMapper, QtCore.SIGNAL("mapped(QWidget *)"),
                     self.workspace, QtCore.SLOT("setActiveWindow(QWidget *)"))

        self.statusbar.setMinimumHeight(self.statusbar.height())
        self.cancelButton = QtGui.QPushButton("Cancel")
        self.progressText = QtGui.QLabel("")
        self.progressIndicator = ProgressStatus(self.progressText)
        self.progressIndicator.setMinimumWidth(100)
        self.actionCloseFigure.setVisible(False)
        self.actionConfigure.setVisible(False)
        self.actionRefresh.setVisible(False)

        global couchIsUsable
        global couchHasUsableKeyring
        if not couchIsUsable and not couchHasUsableKeyring:
            Warning(self, "couchdbnotfound", "Missing CouchDB",
                              "<h4>I cannot find a running CouchDB instance</h4>"
                              "This usually happens if you execute wrowser on a remote machine."
                              "The menu item File > View CouchDB Trace will be disabled")

        elif not couchIsUsable:
            Warning(self, "couchdbnotinstalled", "No CouchDB installed",
                              "<h4>I cannot find a an installation of desktopcouch</h4>"
                              "The couchdb tracing view will only work with desktopcouch, which is preinstalled in Ubuntu Lucid Lynx."
                              "The menu item File > View CouchDB Trace will be disabled")


        self.actionView_CouchDB_Trace.setEnabled(couchIsUsable)


    @QtCore.pyqtSignature("")
    def on_actionView_CouchDB_Trace_triggered(self):
        couchDbDialogue = OpenCouchDatabase(self.workspace)
        self.workspace.addWindow(couchDbDialogue)
        couchDbDialogue.showMaximized()
        if couchDbDialogue.exec_() == QtGui.QDialog.Accepted:
            couchId = couchDbDialogue.getDatabase()
        else:
            return

        db = desktopcouch.records.server.CouchDatabase(str(couchId), create=False)
        self.viewCouchDBCanvas = tracing.frameplotter.FramePlotter(db, self.workspace)
        self.workspace.addWindow(self.viewCouchDBCanvas)
        self.viewCouchDBCanvas.showMaximized()

        self.model = tracing.model.TraceEntryTableModel(self)
    
        self.viewCouchDBTraceWidget = Widgets.ViewCouchDBTrace(self)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.viewCouchDBTraceWidget)
        self.viewCouchDBTraceWidget.internalWidget.tableView.setModel(self.model)
        self.viewCouchDBTraceWidget.internalWidget.tableView.resizeColumnsToContents()
        

        self.viewCouchDBNavigation = Widgets.TraceNavigation(self)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.viewCouchDBNavigation)
        for sender in  self.viewCouchDBCanvas.senders:
            self.viewCouchDBNavigation.internalWidget.senders.addItem(sender)

        for receiver in  self.viewCouchDBCanvas.receivers:
            self.viewCouchDBNavigation.internalWidget.receivers.addItem(receiver)

        self.viewCouchDBCanvas.setSelectors(self.viewCouchDBNavigation.internalWidget.senders,
                                            self.viewCouchDBNavigation.internalWidget.receivers,
                                            self.viewCouchDBNavigation.internalWidget.applyFilterCheckbox,
                                            self.viewCouchDBNavigation.internalWidget.customFilter)

        self.viewCouchDBCanvas.plotRadioFrame()

        self.connect(self.viewCouchDBNavigation, QtCore.SIGNAL("radioFrameChanged(int, int)"), self.viewCouchDBCanvas.on_radioFrameChanged)

        self.connect(self.viewCouchDBCanvas, QtCore.SIGNAL("itemPicked"), self.model.addItem)
        self.connect(self.viewCouchDBTraceWidget.internalWidget.clearButton,
                     QtCore.SIGNAL("clicked()"), self.model.clear)

        self.actionOpenCampaignDatabase.setEnabled(False)
        self.actionView_CouchDB_Trace.setEnabled(False)
        self.actionOpenDSV.setEnabled(False)
        self.actionOpenDirectory.setEnabled(False)
        self.actionView_Scenario.setEnabled(False)
        self.actionCloseDataSource.setEnabled(True)


    def menuSetAllOpen(self,isEnabled):
        global couchIsUsable   
        self.actionView_CouchDB_Trace.setEnabled(isEnabled and couchIsUsable)

    def on_cancelClicked(self):
        pass


    @QtCore.pyqtSignature("")
    def on_actionOpenDirectory_triggered(self):
        self.directoryNavigation = DirectoryNavigation(self.campaigns, self.calledFromDir, self)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.directoryNavigation)

        self.menuSetAllOpen(False)
        self.actionRefresh.setVisible(True)

        self.actionCloseDataSource.setEnabled(True)


    def closeDock(self):
        if hasattr(self, "directoryNavigation"):
            self.directoryNavigation.close()
        if hasattr(self, "viewCouchDBTraceWidget"):
            self.viewCouchDBTraceWidget.close()
        if hasattr(self, "viewCouchDBNavigation"):
            self.viewCouchDBNavigation.close()

    QtCore.pyqtSignature("")
    def on_actionCloseDataSource_triggered(self):
        self.closeDock()

        if hasattr(self, "model"):
            del self.model
        for window in self.workspace.windowList():
            window.close()
        self.campaigns = Observable()

        self.menuSetAllOpen(True)

        self.actionCloseDataSource.setEnabled(False)
        self.actionNewParameter.setEnabled(True)
        self.actionRefresh.setVisible(False)
#        self.menuNew.setEnabled(False)
        windowTitleElements = self.windowTitle().split(' ')
        self.setWindowTitle(windowTitleElements[0]+" "+windowTitleElements[1])


    def showProgressBar(self, callBack):
        self.statusbar.addWidget(self.cancelButton)
        self.statusbar.addWidget(self.progressIndicator)
        self.statusbar.addWidget(self.progressText)
        self.cancelCallback = callBack
        self.cancelButton.show()
        self.progressIndicator.show()
        self.progressText.show()
        self.cancelButton.connect(self.cancelButton,QtCore.SIGNAL("clicked()"), callBack)

    def hideProgressBar(self):
        self.statusbar.removeWidget(self.cancelButton)
        self.statusbar.removeWidget(self.progressIndicator)
        self.statusbar.removeWidget(self.progressText)
        self.progressIndicator.reset()
        self.progressIndicator.setValue(0)
        self.progressText.clear()
        self.cancelButton.disconnect(self.cancelButton,QtCore.SIGNAL("clicked()"), self.cancelCallback)

    @QtCore.pyqtSignature("")
    def on_actionView_Scenario_triggered(self):

        self.viewScenarioFilename = str(QtGui.QFileDialog.getOpenFileName(
                self.workspace,
                "Open File",
                os.getcwd(),
                "Config Files (*.py)"))
        if self.viewScenarioFilename == '' :  return 
        try:
            self.p = scenario.plotterFactory.create(self.viewScenarioFilename)
        except scenario.plotterFactory.InvalidConfig:
            QtGui.QMessageBox.critical(self,
                                       "No scenario found",
                                       "Could not find any scenario in this file.\n\nMake sure you have an instance of openwns.simulator.OpenWNS in the global namespace of your configuration file")
            self.p = None

        if self.p is not None:
            self.viewScenarioCanvas = scenario.widgets.FigureCanvas(self.workspace)
            self.workspace.addWindow(self.viewScenarioCanvas)
            self.p.plotScenario(self.viewScenarioCanvas, '', 0.0, 0.0, 0.0, False)
            self.viewScenarioCanvas.showMaximized()

            self.viewScenarioWidget = scenario.widgets.ViewScenario(self.viewScenarioFilename, self)
#            self.viewScenarioCanvas.registerMotionEventHandler(self.viewScenarioWidget.internalWidget.on_motionEvent)
#            self.viewScenarioCanvas.registerButtonPressEventHandler(self.viewScenarioWidget.internalWidget.on_buttonPressEvent)

            self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.viewScenarioWidget)

            self.menuSetAllOpen(False)

            self.actionCloseDataSource.setEnabled(True)

    def updateScenarioView(self, fileToPlot, fillValue, minValue, maxValue, includeContour):
        if self.viewScenarioCanvas is not None:
            #self.p = scenario.plotterFactory.create(self.viewScenarioFilename)

            self.viewScenarioCanvas.clear()

            self.p.plotScenario(self.viewScenarioCanvas, fileToPlot, fillValue, minValue, maxValue, includeContour)


