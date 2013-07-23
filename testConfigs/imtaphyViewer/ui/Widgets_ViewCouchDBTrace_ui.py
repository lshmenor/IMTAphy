# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Widgets_ViewCouchDBTrace.ui'
#
# Created: Tue Jan 24 00:06:06 2012
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_Widgets_ViewCouchDBTrace(object):
    def setupUi(self, Widgets_ViewCouchDBTrace):
        Widgets_ViewCouchDBTrace.setObjectName(_fromUtf8("Widgets_ViewCouchDBTrace"))
        Widgets_ViewCouchDBTrace.resize(400, 232)
        Widgets_ViewCouchDBTrace.setWindowTitle(QtGui.QApplication.translate("Widgets_ViewCouchDBTrace", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.verticalLayout = QtGui.QVBoxLayout(Widgets_ViewCouchDBTrace)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.tableView = QtGui.QTableView(Widgets_ViewCouchDBTrace)
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Andale Mono"))
        font.setPointSize(8)
        self.tableView.setFont(font)
        self.tableView.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
        self.tableView.setSortingEnabled(True)
        self.tableView.setObjectName(_fromUtf8("tableView"))
        self.tableView.horizontalHeader().setMinimumSectionSize(50)
        self.tableView.horizontalHeader().setStretchLastSection(False)
        self.tableView.verticalHeader().setVisible(False)
        self.verticalLayout.addWidget(self.tableView)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.clearButton = QtGui.QPushButton(Widgets_ViewCouchDBTrace)
        self.clearButton.setText(QtGui.QApplication.translate("Widgets_ViewCouchDBTrace", "Clear", None, QtGui.QApplication.UnicodeUTF8))
        self.clearButton.setObjectName(_fromUtf8("clearButton"))
        self.horizontalLayout.addWidget(self.clearButton)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.verticalLayout.addLayout(self.horizontalLayout)

        self.retranslateUi(Widgets_ViewCouchDBTrace)
        QtCore.QMetaObject.connectSlotsByName(Widgets_ViewCouchDBTrace)

    def retranslateUi(self, Widgets_ViewCouchDBTrace):
        pass

