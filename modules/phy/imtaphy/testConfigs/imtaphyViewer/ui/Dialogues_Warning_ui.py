# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Dialogues_Warning.ui'
#
# Created: Mon Jan 23 00:44:53 2012
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_Dialogues_Warning(object):
    def setupUi(self, Dialogues_Warning):
        Dialogues_Warning.setObjectName(_fromUtf8("Dialogues_Warning"))
        Dialogues_Warning.resize(400, 251)
        Dialogues_Warning.setWindowTitle(QtGui.QApplication.translate("Dialogues_Warning", "Dialog", None, QtGui.QApplication.UnicodeUTF8))
        self.verticalLayout = QtGui.QVBoxLayout(Dialogues_Warning)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.label = QtGui.QLabel(Dialogues_Warning)
        self.label.setText(QtGui.QApplication.translate("Dialogues_Warning", "This is a warning with a long text to see if wrap around kdkdk", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.label.setWordWrap(True)
        self.label.setObjectName(_fromUtf8("label"))
        self.verticalLayout.addWidget(self.label)
        self.checkBox = QtGui.QCheckBox(Dialogues_Warning)
        self.checkBox.setText(QtGui.QApplication.translate("Dialogues_Warning", "Do not show this warning again", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBox.setObjectName(_fromUtf8("checkBox"))
        self.verticalLayout.addWidget(self.checkBox)
        self.buttonBox = QtGui.QDialogButtonBox(Dialogues_Warning)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setCenterButtons(True)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(Dialogues_Warning)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), Dialogues_Warning.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), Dialogues_Warning.reject)
        QtCore.QMetaObject.connectSlotsByName(Dialogues_Warning)

    def retranslateUi(self, Dialogues_Warning):
        pass

