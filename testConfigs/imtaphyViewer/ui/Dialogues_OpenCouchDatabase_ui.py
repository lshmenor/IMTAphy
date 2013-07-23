# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Dialogues_OpenCouchDatabase.ui'
#
# Created: Mon Jan 23 00:11:55 2012
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_CouchDBDialog(object):
    def setupUi(self, CouchDBDialog):
        CouchDBDialog.setObjectName(_fromUtf8("CouchDBDialog"))
        CouchDBDialog.resize(391, 300)
        CouchDBDialog.setWindowTitle(QtGui.QApplication.translate("CouchDBDialog", "Dialog", None, QtGui.QApplication.UnicodeUTF8))
        self.verticalLayout = QtGui.QVBoxLayout(CouchDBDialog)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.label = QtGui.QLabel(CouchDBDialog)
        self.label.setText(QtGui.QApplication.translate("CouchDBDialog", "Open existing database or import new data", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.verticalLayout.addWidget(self.label)
        self.listWidget = QtGui.QListWidget(CouchDBDialog)
        self.listWidget.setObjectName(_fromUtf8("listWidget"))
        self.verticalLayout.addWidget(self.listWidget)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.importButton = QtGui.QPushButton(CouchDBDialog)
        self.importButton.setText(QtGui.QApplication.translate("CouchDBDialog", "Import...", None, QtGui.QApplication.UnicodeUTF8))
        self.importButton.setObjectName(_fromUtf8("importButton"))
        self.horizontalLayout.addWidget(self.importButton)
        self.buttonBox = QtGui.QDialogButtonBox(CouchDBDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.horizontalLayout.addWidget(self.buttonBox)
        self.verticalLayout.addLayout(self.horizontalLayout)

        self.retranslateUi(CouchDBDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), CouchDBDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), CouchDBDialog.reject)
        QtCore.QObject.connect(self.listWidget, QtCore.SIGNAL(_fromUtf8("activated(QModelIndex)")), CouchDBDialog.accept)
        QtCore.QMetaObject.connectSlotsByName(CouchDBDialog)

    def retranslateUi(self, CouchDBDialog):
        self.listWidget.setSortingEnabled(True)

