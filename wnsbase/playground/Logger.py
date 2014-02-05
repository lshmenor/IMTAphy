###################################################################################
# OTSPrototype - Prototypische Implementierung eines OTS konformen Protokollstapels
#
# Diese Software unterliegt den Bestimmungen des Konsortialvertrags im Projekt
# Dmotion in Bezug auf Verteilung und Geheimhaltung. Sie darf von den
# Projektpartnern innerhalb Dmotion genutzt werden.
#
# Author : Daniel Bueltmann
# (c) 2006 RWTH-Aachen, Lehrstuhl fuer Kommunikationsnetze
###################################################################################

# Loglevel definitions
Silent = 0
Quiet = 1
Warning = 2
Debug = 3

class ConsoleMasterLogger:

    def __init__(self, mastername):
        self.mastername = mastername

    def write(self, name, message):
        print str(self.mastername).ljust(10) + ":" + str(name).center(10) + " : " + message

class Logger:

    name = None

    loglevel = Debug

    master = None

    def __init__(self, name, loglevel, master=ConsoleMasterLogger("playground.py")):

        self.name = name

        self.loglevel = loglevel

        self.master = master

    def logQuiet(self, message):
        self.log(Quiet, message)

    def logWarning(self, message):
        self.log(Warning, message)

    def logDebug(self, message):
        self.log(Debug, message)

    def log(self, level, message):
        if (level <= self.loglevel):
            self.master.write(self.name, message)
