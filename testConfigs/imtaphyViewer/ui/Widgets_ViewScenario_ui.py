# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Widgets_ViewScenario.ui'
#
# Created: Tue Jan 24 03:14:25 2012
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_Widgets_ViewScenario(object):
    def setupUi(self, Widgets_ViewScenario):
        Widgets_ViewScenario.setObjectName(_fromUtf8("Widgets_ViewScenario"))
        Widgets_ViewScenario.resize(410, 605)
        Widgets_ViewScenario.setWindowTitle(QtGui.QApplication.translate("Widgets_ViewScenario", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.vboxlayout = QtGui.QVBoxLayout(Widgets_ViewScenario)
        self.vboxlayout.setObjectName(_fromUtf8("vboxlayout"))
        self.toolBox = QtGui.QToolBox(Widgets_ViewScenario)
        self.toolBox.setObjectName(_fromUtf8("toolBox"))
        self.resultPage = QtGui.QWidget()
        self.resultPage.setGeometry(QtCore.QRect(0, 0, 392, 556))
        self.resultPage.setObjectName(_fromUtf8("resultPage"))
        self.verticalLayout = QtGui.QVBoxLayout(self.resultPage)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.resultFileLabel = QtGui.QLabel(self.resultPage)
        self.resultFileLabel.setText(QtGui.QApplication.translate("Widgets_ViewScenario", "Select a file to plot", None, QtGui.QApplication.UnicodeUTF8))
        self.resultFileLabel.setObjectName(_fromUtf8("resultFileLabel"))
        self.verticalLayout.addWidget(self.resultFileLabel)
        self.fileList = QtGui.QListWidget(self.resultPage)
        self.fileList.setObjectName(_fromUtf8("fileList"))
        self.verticalLayout.addWidget(self.fileList)
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label = QtGui.QLabel(self.resultPage)
        self.label.setToolTip(QtGui.QApplication.translate("Widgets_ViewScenario", "If simulator output does not contain a value at a given point, this value is used", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Widgets_ViewScenario", "Fill Value", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.fillValueLineEdit = QtGui.QLineEdit(self.resultPage)
        self.fillValueLineEdit.setToolTip(QtGui.QApplication.translate("Widgets_ViewScenario", "If simulator output does not contain a value at a given point, this value is used", None, QtGui.QApplication.UnicodeUTF8))
        self.fillValueLineEdit.setText(QtGui.QApplication.translate("Widgets_ViewScenario", "0", None, QtGui.QApplication.UnicodeUTF8))
        self.fillValueLineEdit.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.fillValueLineEdit.setObjectName(_fromUtf8("fillValueLineEdit"))
        self.gridLayout.addWidget(self.fillValueLineEdit, 0, 2, 1, 1)
        self.label_11 = QtGui.QLabel(self.resultPage)
        self.label_11.setToolTip(QtGui.QApplication.translate("Widgets_ViewScenario", "If simulator output does not contain a value at a given point, this value is used", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setText(QtGui.QApplication.translate("Widgets_ViewScenario", "minValue", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setObjectName(_fromUtf8("label_11"))
        self.gridLayout.addWidget(self.label_11, 1, 0, 1, 2)
        self.minValue = QtGui.QLineEdit(self.resultPage)
        self.minValue.setToolTip(QtGui.QApplication.translate("Widgets_ViewScenario", "Use this as the minimum value (deep blue) for the plot, all lower values will be mapped to this.", None, QtGui.QApplication.UnicodeUTF8))
        self.minValue.setText(_fromUtf8(""))
        self.minValue.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.minValue.setObjectName(_fromUtf8("minValue"))
        self.gridLayout.addWidget(self.minValue, 1, 2, 1, 1)
        self.label_12 = QtGui.QLabel(self.resultPage)
        self.label_12.setToolTip(QtGui.QApplication.translate("Widgets_ViewScenario", "If simulator output does not contain a value at a given point, this value is used", None, QtGui.QApplication.UnicodeUTF8))
        self.label_12.setText(QtGui.QApplication.translate("Widgets_ViewScenario", "maxValue", None, QtGui.QApplication.UnicodeUTF8))
        self.label_12.setObjectName(_fromUtf8("label_12"))
        self.gridLayout.addWidget(self.label_12, 2, 0, 1, 1)
        self.maxValue = QtGui.QLineEdit(self.resultPage)
        self.maxValue.setToolTip(QtGui.QApplication.translate("Widgets_ViewScenario", "Use this as the maximum value (dark red) for the plot, all higher values will be mapped to this.", None, QtGui.QApplication.UnicodeUTF8))
        self.maxValue.setText(_fromUtf8(""))
        self.maxValue.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.maxValue.setObjectName(_fromUtf8("maxValue"))
        self.gridLayout.addWidget(self.maxValue, 2, 2, 1, 1)
        self.label_4 = QtGui.QLabel(self.resultPage)
        self.label_4.setText(QtGui.QApplication.translate("Widgets_ViewScenario", "Draw contour plot", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.contourPlotCheckBox = QtGui.QCheckBox(self.resultPage)
        self.contourPlotCheckBox.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.contourPlotCheckBox.setText(_fromUtf8(""))
        self.contourPlotCheckBox.setObjectName(_fromUtf8("contourPlotCheckBox"))
        self.gridLayout.addWidget(self.contourPlotCheckBox, 3, 1, 1, 2)
        self.verticalLayout.addLayout(self.gridLayout)
        spacerItem = QtGui.QSpacerItem(261, 171, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem)
        self.redrawButton = QtGui.QPushButton(self.resultPage)
        self.redrawButton.setText(QtGui.QApplication.translate("Widgets_ViewScenario", "Redraw", None, QtGui.QApplication.UnicodeUTF8))
        self.redrawButton.setObjectName(_fromUtf8("redrawButton"))
        self.verticalLayout.addWidget(self.redrawButton)
        self.toolBox.addItem(self.resultPage, _fromUtf8(""))
        self.vboxlayout.addWidget(self.toolBox)

        self.retranslateUi(Widgets_ViewScenario)
        self.toolBox.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(Widgets_ViewScenario)

    def retranslateUi(self, Widgets_ViewScenario):
        self.toolBox.setItemText(self.toolBox.indexOf(self.resultPage), QtGui.QApplication.translate("Widgets_ViewScenario", "Map Plotting", None, QtGui.QApplication.UnicodeUTF8))

