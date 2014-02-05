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
import json
import desktopcouch
import desktopcouch.records
import desktopcouch.records.record

def importFile(filename, dbname):
    progress = QtGui.QProgressDialog("Importing data", "Stop", 0 ,3)
    progress.setWindowModality(QtCore.Qt.WindowModal)
    progress.setCancelButton(None)
    progress.setMinimumDuration(0)

    progress.setValue(1)
    f = open(filename)
    c = f.read()
    f.close()

    progress.setValue(2)

    parsed = json.loads(c)["content"]

    progress.setValue(3)

    
    progress = QtGui.QProgressDialog("Writing %d entries to database" % len(parsed), "Stop", 0, int(len(parsed)/500) +1)
    progress.setWindowModality(QtCore.Qt.WindowModal)
    progress.setCancelButton(None)
    progress.setMinimumDuration(0)

    db = desktopcouch.records.server.CouchDatabase(dbname.lower(), create=True)
    
    db.add_view("orderByStartTime",
"""
function(doc) {
  if (doc.Transmission)
  {
      emit(doc.Transmission.Start, doc);
  }
}
""", "", "wrowser")

    db.add_view("senders",
"""
function(doc) {
  emit(doc.Transmission.Sender, null);
}
""", 
"""
function(keys, values) {
   return true;
}
""",
"wrowser")

    db.add_view("receivers",
"""
function(doc) {
  emit(doc.Transmission.Receiver, null);
}
""", 
"""
function(keys, values) {
   return true;
}
""",
"wrowser")



    i = 1
    recordsToAdd = []
    for entry in parsed:
        r = desktopcouch.records.record.Record(record_type="http://openwns.org/couchdb/phytrace")
        r["Transmission"] = entry["Transmission"]
        #if entry.has_key("SINREst"):
            #r["SINREst"] = entry["SINREst"]
        #if entry.has_key("SchedulingTimeSlot"):
            #r["SchedulingTimeSlot"] = entry["SchedulingTimeSlot"]
        recordsToAdd.append(r)
        if len(recordsToAdd) > 500:
            db.put_records_batch(recordsToAdd)
            recordsToAdd = []
            progress.setValue(i)
            i = i + 1

    progress.setValue(i+1)
    db.put_records_batch(recordsToAdd)


def deleteDB(dbname):
    import desktopcouch.records.server
    import couchdb.client
    port = desktopcouch.find_port()
    db = desktopcouch.records.server.OAuthCapableServer('http://localhost:%s/' % port)

    del db[dbname.lower()]

class TraceEntryTableModel(QtCore.QAbstractTableModel):

    def __init__(self, parent = None):
        QtCore.QAbstractTableModel.__init__(self, parent)
        self.headerNames = []
        self.headerNames.append("Sender")
        self.headerNames.append("Receiver")
        self.headerNames.append("TTI")
        self.headerNames.append("TBid")
        self.headerNames.append("PRB")
        self.headerNames.append("SINR") 
        self.headerNames.append("Eff.SINR")
        self.headerNames.append("Est.SINR")
        self.headerNames.append("MCS")
        self.headerNames.append("CR")
        self.headerNames.append("BLER")
        self.headerNames.append("Decodable")
        self.headerNames.append("HARQ.PID")
        self.headerNames.append("HARQ.NDI")
        self.headerNames.append("HARQ.attempt")
        #self.headerNames.append("SpatialTB")
        #self.headerNames.append("Layer")
        
        self.theData=[]

    def rowCount(self, parent = QtCore.QModelIndex()):
        if parent.isValid():
            return 0
        else:
            return len(self.theData)

    def columnCount(self, parent = QtCore.QModelIndex()):
        if parent.isValid():
            return 0
        else:
            return len(self.headerNames)

    def headerData(self, section, orientation, role):
        if role == QtCore.Qt.DisplayRole:
            return QtCore.QVariant(self.headerNames[section])
        return QtCore.QVariant()

    def data(self, index, role = QtCore.Qt.DisplayRole):
        if not index.isValid() or index.row() >= self.rowCount():
            return QtCore.QVariant()

        if role == QtCore.Qt.DisplayRole:
            key = self.headerNames[index.column()]
            if self.theData[index.row()].has_key(key):
                    return QtCore.QVariant(self.theData[index.row()][key])

        return QtCore.QVariant()


    def addItem(self, item):
        i = {}

        for e in self.theData:
            if e["_data"] == item:
                # No duplicates
                return
   
        # here we display stuff after the user has clicked into the grid
        # some table entries are pre-defined above but all additional
        # entries in the tracefile will be added as well

        i["Transmission.Start"] = item.value["Transmission"]["Start"]
        i["Transmission.Stop"] = item.value["Transmission"]["Stop"]
        i["TTI"] = int(round(item.value["Transmission"]["Stop"] * 1000.0))

        for key in item.value["Transmission"].keys():
            if key == "Start" or key == "Stop":
                continue
            i[key] = item.value["Transmission"][key]
            if not key in self.headerNames:
                self.headerNames.append(key)
        
        i["_data"] = item

        self.theData.append(i)
        self.reset()

    def clear(self):
        self.theData = []
        self.reset()
    
    def sort(self, column, order):
        if order == 0:
            reverse = False
        else:
            reverse = True

        key = self.headerNames[column]

        self.theData = sorted(self.theData, key=lambda i:i[key], reverse=reverse)
        self.reset()

class CodeSnippetModel(QtCore.QAbstractItemModel):

    def __init__(self, parent=None):
        QtCore.QAbstractItemModel.__init__(self, parent)

        import os

        self.dir = os.path.join(os.environ['HOME'], '.wns', 'traceFilters')

        if not os.path.exists(self.dir):
            os.mkdir(self.dir)

        self.snippets = []

        self.load()

        if len(self.snippets) == 0:

            self.snippets.append(["Example", """
def filter(key, value):
   # The function named filter will be called from wrowser
   # do not rename it or change the number of mandatory parameters
   # Return True if you want to include this element in the frame viewer
   # Return False otherwise

   # Look in the console output to see fields
   print key
   print value

   # Check if element exists before accessing it
   if value.has_key("Transmission"):
      if value["Transmission"].has_key("SenderID"):
          return value["Transmission"]["SenderID"] == "BS2"
   return False
"""])
            self.save()

    def load(self):
        import os
        oldpath = os.getcwd()
        os.chdir(self.dir)
        import glob
        files = glob.glob("*.py")
        for filename in files:
            key = filename.rstrip("y")
            key = key.rstrip("p")
            key = key.rstrip(".")

            f = open(filename)
            c = f.read()
            f.close()
            self.snippets.append([key, c])

        os.chdir(oldpath)

    def save(self):
        import os
        oldpath = os.getcwd()
        os.chdir(self.dir)

        for (k,v) in self.snippets:
            f = open(k+".py","w")
            f.write(v)
            f.close()

        os.chdir(oldpath)

    def delete(self, key):
        import os
        oldpath = os.getcwd()
        os.chdir(self.dir)

        os.remove(str(key)+".py")

        os.chdir(oldpath)
        
        
        
    def rowCount(self, parent = QtCore.QModelIndex()):
        if parent.isValid():
            return 0
        else:
            return len(self.snippets)

    def columnCount(self, parent = QtCore.QModelIndex()):
        if parent.isValid():
            return 0
        else:
            return 2

    def data(self, index, role = QtCore.Qt.DisplayRole):
        if not index.isValid() or index.row() >= self.rowCount():
            return QtCore.QVariant()

        if role == QtCore.Qt.DisplayRole or role == QtCore.Qt.EditRole:
            return QtCore.QVariant(self.snippets[index.row()][index.column()])

        return QtCore.QVariant()

    def index(self, row, column, parent = QtCore.QModelIndex()):
        return self.createIndex(row, column, 0)

    def parent(self, index):
        return QtCore.QModelIndex()

    def setData(self, index, data, role = QtCore.Qt.EditRole):
        if role == QtCore.Qt.EditRole:
            self.snippets[index.row()][index.column()] = data.toString()
            self.save()
            self.emit(QtCore.SIGNAL("dataChanged(const QModelIndex&,const QModelIndex&)"), index, index)
            return True
        return False

    def flags(self, index):
        if not index.isValid():
            return QtCore.Qt.ItemIsEnabled

        return QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable | QtCore.Qt.ItemIsEditable

    def insertRows(self, row, count, parent):
        self.beginInsertRows(QtCore.QModelIndex(), row, row+count)

        for i in xrange(count):
            self.snippets.insert(row+i, ["", "def filter(key, value):\n   return True"])

        self.endInsertRows()
        self.emit(QtCore.SIGNAL("modelReset()"))
        return True

    def remove(self, rowindex):

        msgBox = QtGui.QMessageBox()
        msgBox.setText("Remove %s" % self.snippets[rowindex][0])
        msgBox.setInformativeText("This will permanently delete the filter. Make sure you have a copy.")
        msgBox.setStandardButtons(QtGui.QMessageBox.Ok | QtGui.QMessageBox.Cancel);
        msgBox.setDefaultButton(QtGui.QMessageBox.Cancel);
        ret = msgBox.exec_();
        
        if ret == QtGui.QMessageBox.Ok:
            e = self.snippets.pop(rowindex)
            self.delete(e[0])
            self.emit(QtCore.SIGNAL("modelReset()"))
