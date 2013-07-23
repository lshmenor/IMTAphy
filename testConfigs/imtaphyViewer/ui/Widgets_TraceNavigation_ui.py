# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Widgets_TraceNavigation.ui'
#
# Created: Thu Jan 26 00:59:57 2012
#      by: PyQt4 UI code generator 4.8.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_Widgets_TraceNavigation(object):
    def setupUi(self, Widgets_TraceNavigation):
        Widgets_TraceNavigation.setObjectName(_fromUtf8("Widgets_TraceNavigation"))
        Widgets_TraceNavigation.resize(360, 580)
        Widgets_TraceNavigation.setWindowTitle(QtGui.QApplication.translate("Widgets_TraceNavigation", "Form", None, QtGui.QApplication.UnicodeUTF8))
        Widgets_TraceNavigation.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "Run filter", None, QtGui.QApplication.UnicodeUTF8))
        self.vboxlayout = QtGui.QVBoxLayout(Widgets_TraceNavigation)
        self.vboxlayout.setObjectName(_fromUtf8("vboxlayout"))
        self.gridlayout = QtGui.QGridLayout()
        self.gridlayout.setObjectName(_fromUtf8("gridlayout"))
        self.label = QtGui.QLabel(Widgets_TraceNavigation)
        self.label.setText(QtGui.QApplication.translate("Widgets_TraceNavigation", "Start TTI", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.gridlayout.addWidget(self.label, 0, 0, 1, 1)
        self.label_4 = QtGui.QLabel(Widgets_TraceNavigation)
        self.label_4.setText(QtGui.QApplication.translate("Widgets_TraceNavigation", "No Frames", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridlayout.addWidget(self.label_4, 0, 1, 1, 1)
        self.stepSize = QtGui.QSpinBox(Widgets_TraceNavigation)
        self.stepSize.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "Show number of radio frames", None, QtGui.QApplication.UnicodeUTF8))
        self.stepSize.setMinimum(1)
        self.stepSize.setMaximum(999999)
        self.stepSize.setObjectName(_fromUtf8("stepSize"))
        self.gridlayout.addWidget(self.stepSize, 1, 1, 1, 1)
        self.previous = QtGui.QPushButton(Widgets_TraceNavigation)
        self.previous.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "go back number of radioframes", None, QtGui.QApplication.UnicodeUTF8))
        self.previous.setText(QtGui.QApplication.translate("Widgets_TraceNavigation", "<<", None, QtGui.QApplication.UnicodeUTF8))
        self.previous.setObjectName(_fromUtf8("previous"))
        self.gridlayout.addWidget(self.previous, 1, 2, 1, 1)
        self.next = QtGui.QPushButton(Widgets_TraceNavigation)
        self.next.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "forward number of radioframes", None, QtGui.QApplication.UnicodeUTF8))
        self.next.setText(QtGui.QApplication.translate("Widgets_TraceNavigation", ">>", None, QtGui.QApplication.UnicodeUTF8))
        self.next.setObjectName(_fromUtf8("next"))
        self.gridlayout.addWidget(self.next, 1, 3, 1, 1)
        self.radioframe = QtGui.QSpinBox(Widgets_TraceNavigation)
        self.radioframe.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "Start at radioframe", None, QtGui.QApplication.UnicodeUTF8))
        self.radioframe.setMaximum(999999)
        self.radioframe.setObjectName(_fromUtf8("radioframe"))
        self.gridlayout.addWidget(self.radioframe, 1, 0, 1, 1)
        self.vboxlayout.addLayout(self.gridlayout)
        self.gridlayout1 = QtGui.QGridLayout()
        self.gridlayout1.setObjectName(_fromUtf8("gridlayout1"))
        self.label_2 = QtGui.QLabel(Widgets_TraceNavigation)
        self.label_2.setText(QtGui.QApplication.translate("Widgets_TraceNavigation", "Sender", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridlayout1.addWidget(self.label_2, 0, 0, 1, 1)
        self.label_3 = QtGui.QLabel(Widgets_TraceNavigation)
        self.label_3.setText(QtGui.QApplication.translate("Widgets_TraceNavigation", "Receiver", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridlayout1.addWidget(self.label_3, 0, 1, 1, 1)
        self.receivers = QtGui.QListWidget(Widgets_TraceNavigation)
        self.receivers.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "filter by receiver, multiple selections possible", None, QtGui.QApplication.UnicodeUTF8))
        self.receivers.setSelectionMode(QtGui.QAbstractItemView.MultiSelection)
        self.receivers.setObjectName(_fromUtf8("receivers"))
        self.gridlayout1.addWidget(self.receivers, 1, 1, 1, 1)
        self.senders = QtGui.QListWidget(Widgets_TraceNavigation)
        self.senders.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "filter by sender, multiple selections possible", None, QtGui.QApplication.UnicodeUTF8))
        self.senders.setSelectionMode(QtGui.QAbstractItemView.MultiSelection)
        self.senders.setObjectName(_fromUtf8("senders"))
        self.gridlayout1.addWidget(self.senders, 1, 0, 1, 1)
        self.vboxlayout.addLayout(self.gridlayout1)
        self.hboxlayout = QtGui.QHBoxLayout()
        self.hboxlayout.setObjectName(_fromUtf8("hboxlayout"))
        self.applyFilterCheckbox = QtGui.QCheckBox(Widgets_TraceNavigation)
        self.applyFilterCheckbox.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "Toogle to enable use of custom filter", None, QtGui.QApplication.UnicodeUTF8))
        self.applyFilterCheckbox.setText(QtGui.QApplication.translate("Widgets_TraceNavigation", " Use Filter", None, QtGui.QApplication.UnicodeUTF8))
        self.applyFilterCheckbox.setObjectName(_fromUtf8("applyFilterCheckbox"))
        self.hboxlayout.addWidget(self.applyFilterCheckbox)
        self.snippetCombo = QtGui.QComboBox(Widgets_TraceNavigation)
        self.snippetCombo.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "Filter name", None, QtGui.QApplication.UnicodeUTF8))
        self.snippetCombo.setEditable(True)
        self.snippetCombo.setObjectName(_fromUtf8("snippetCombo"))
        self.hboxlayout.addWidget(self.snippetCombo)
        self.removeFilterButton = QtGui.QToolButton(Widgets_TraceNavigation)
        self.removeFilterButton.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "Remove this filter", None, QtGui.QApplication.UnicodeUTF8))
        self.removeFilterButton.setText(QtGui.QApplication.translate("Widgets_TraceNavigation", "...", None, QtGui.QApplication.UnicodeUTF8))
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(_fromUtf8("remove.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.removeFilterButton.setIcon(icon)
        self.removeFilterButton.setObjectName(_fromUtf8("removeFilterButton"))
        self.hboxlayout.addWidget(self.removeFilterButton)
        self.applyFilterButton = QtGui.QToolButton(Widgets_TraceNavigation)
        self.applyFilterButton.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "Run this filter", None, QtGui.QApplication.UnicodeUTF8))
        self.applyFilterButton.setText(QtGui.QApplication.translate("Widgets_TraceNavigation", "...", None, QtGui.QApplication.UnicodeUTF8))
        icon1 = QtGui.QIcon()
        icon1.addPixmap(QtGui.QPixmap(_fromUtf8("dialog-information.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.applyFilterButton.setIcon(icon1)
        self.applyFilterButton.setObjectName(_fromUtf8("applyFilterButton"))
        self.hboxlayout.addWidget(self.applyFilterButton)
        self.vboxlayout.addLayout(self.hboxlayout)
        self.customFilter = QtGui.QTextEdit(Widgets_TraceNavigation)
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Courier 10 Pitch"))
        font.setUnderline(False)
        self.customFilter.setFont(font)
        self.customFilter.setToolTip(QtGui.QApplication.translate("Widgets_TraceNavigation", "Python code of filter", None, QtGui.QApplication.UnicodeUTF8))
        self.customFilter.setHtml(QtGui.QApplication.translate("Widgets_TraceNavigation", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'Courier 10 Pitch\'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">def filter(key, value):</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">   # Return True if data is to be included</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">   # Look in the console output to see fields</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">   # print value</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">   # Check before accessing helps</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">   #if value.has_key(&quot;Transmission&quot;):</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">      #if value[&quot;Transmission&quot;].has_key(&quot;ReceiverID&quot;):</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">          #return value[&quot;Transmission&quot;][&quot;ReceiverID&quot;] == &quot;UT2&quot;</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">   #return False</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">   return True</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.customFilter.setAcceptRichText(False)
        self.customFilter.setObjectName(_fromUtf8("customFilter"))
        self.vboxlayout.addWidget(self.customFilter)

        self.retranslateUi(Widgets_TraceNavigation)
        QtCore.QMetaObject.connectSlotsByName(Widgets_TraceNavigation)

    def retranslateUi(self, Widgets_TraceNavigation):
        pass

import viewer_rc
