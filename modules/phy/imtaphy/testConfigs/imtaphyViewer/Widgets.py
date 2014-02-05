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


class TraceNavigation(QtGui.QDockWidget):

    from ui.Widgets_TraceNavigation_ui import Ui_Widgets_TraceNavigation
    class TraceNavigationWidget(QtGui.QWidget, Ui_Widgets_TraceNavigation):
        
        def __init__(self, mainWindow, *args):
            QtGui.QWidget.__init__(self, *args)
            self.setupUi(self)

    def __init__(self, parent, *args):
        QtGui.QDockWidget.__init__(self, "Navigation", parent, *args)
        self.internalWidget = self.__class__.TraceNavigationWidget(parent, self)
        self.setWidget(self.internalWidget)

        import tracing.model
        self.snippetModel = tracing.model.CodeSnippetModel(self)
        #self.snippetModel = QtGui.QStringListModel(["Hahn", "Leee"], self)
        combo = self.internalWidget.snippetCombo

        self.connect(combo.model(), QtCore.SIGNAL("dataChanged(const QModelIndex&, const QModelIndex&)"), self.on_SnippetDataChanged)
        self.connect(combo, QtCore.SIGNAL("currentIndexChanged(int)"), self.on_SnippetSelectionChanged)

        combo.setModel(self.snippetModel)

        combo.setCurrentIndex(combo.count())

        self.connect(self.internalWidget.next, QtCore.SIGNAL("clicked()"), self.on_NextClicked)
        self.connect(self.internalWidget.previous, QtCore.SIGNAL("clicked()"), self.on_PreviousClicked)

        self.connect(self.internalWidget.radioframe, QtCore.SIGNAL("valueChanged(int)"), self.on_radioFrameChanged)
        self.connect(self.internalWidget.stepSize, QtCore.SIGNAL("valueChanged(int)"), self.on_stepSizeChanged)

        self.connect(self.internalWidget.senders, QtCore.SIGNAL("itemSelectionChanged()"), self.on_selectionChanged)
        self.connect(self.internalWidget.receivers, QtCore.SIGNAL("itemSelectionChanged()"), self.on_selectionChanged)
        self.connect(self.internalWidget.applyFilterButton, QtCore.SIGNAL("clicked()"), self.on_selectionChanged)
        self.connect(self.internalWidget.applyFilterCheckbox, QtCore.SIGNAL("stateChanged(int)"), self.on_selectionChanged)
        self.connect(self.internalWidget.removeFilterButton, QtCore.SIGNAL("clicked()"), self.on_removeFilter)
        self.connect(self.internalWidget.applyFilterButton, QtCore.SIGNAL("clicked()"), self.on_saveFilter)
        self.connect(self.internalWidget.applyFilterCheckbox, QtCore.SIGNAL("stateChanged(int)"), self.on_saveFilter)

        self.timer = None

    @QtCore.pyqtSignature("const QModelIndex&, const QModelIndex&")
    def on_SnippetDataChanged(self, start, stop):
        combo = self.internalWidget.snippetCombo
        code = self.internalWidget.customFilter

        i = combo.currentIndex()
        i = combo.model().index(i,1)
        t = combo.model().data(i, QtCore.Qt.DisplayRole)
        code.setPlainText(t.toString())

    def on_SnippetSelectionChanged(self, pos):
        combo = self.internalWidget.snippetCombo
        code = self.internalWidget.customFilter

        i = combo.model().index(pos,1)
        t = combo.model().data(i, QtCore.Qt.DisplayRole)
        code.setPlainText(t.toString())
        
    def on_saveFilter(self):
        combo = self.internalWidget.snippetCombo
        code = self.internalWidget.customFilter

        i = combo.currentIndex()
        i = combo.model().index(i,1)
        t = combo.model().setData(i, QtCore.QVariant(code.toPlainText()))
    
    def on_removeFilter(self):
        combo = self.internalWidget.snippetCombo
        code = self.internalWidget.customFilter

        i = combo.currentIndex()

        if i > -1:
            i = combo.model().remove(i)
        
    @QtCore.pyqtSignature("")
    def on_NextClicked(self):
        rf = self.internalWidget.radioframe.value()
        rf += 10 * self.internalWidget.stepSize.value()
        self.internalWidget.radioframe.setValue(rf)

    @QtCore.pyqtSignature("")
    def on_PreviousClicked(self):
        rf = self.internalWidget.radioframe.value()
        rf -= 10 * self.internalWidget.stepSize.value()

        if rf < 0:
            rf = 0

        self.internalWidget.radioframe.setValue(rf)

    @QtCore.pyqtSignature("int")
    def on_radioFrameChanged(self, value):
        if self.timer is None:
            self.timer = QtCore.QTimer(self)
            self.timer.setSingleShot(True)
            self.connect(self.timer, QtCore.SIGNAL("timeout()"), self.on_changeTimerExpired)
            
            self.timer.start(500)

        else:
            if self.timer.isActive():
                self.timer.stop()

            self.timer.start(500)

    @QtCore.pyqtSignature("int")
    def on_stepSizeChanged(self, value):
        if self.timer is None:
            self.timer = QtCore.QTimer(self)
            self.timer.setSingleShot(True)
            self.connect(self.timer, QtCore.SIGNAL("timeout()"), self.on_changeTimerExpired)
            
            self.timer.start(500)

        else:
            if self.timer.isActive():
                self.timer.stop()

            self.timer.start(500)

    @QtCore.pyqtSignature("")
    def on_changeTimerExpired(self):
        self.emit(QtCore.SIGNAL("radioFrameChanged(int, int)"), self.internalWidget.radioframe.value(), self.internalWidget.stepSize.value())

    @QtCore.pyqtSignature("")
    def on_selectionChanged(self):
        self.on_radioFrameChanged(self.internalWidget.radioframe.value())

class ViewCouchDBTrace(QtGui.QDockWidget):

    from ui.Widgets_ViewCouchDBTrace_ui import Ui_Widgets_ViewCouchDBTrace
    class ViewCouchDBTraceWidget(QtGui.QWidget, Ui_Widgets_ViewCouchDBTrace):
        
        def __init__(self, mainWindow, *args):
            QtGui.QWidget.__init__(self, *args)
            self.setupUi(self)

    def __init__(self, parent, *args):
        QtGui.QDockWidget.__init__(self, "View CouchDB Trace", parent, *args)
        self.internalWidget = self.__class__.ViewCouchDBTraceWidget(parent, self)
        self.setWidget(self.internalWidget)

