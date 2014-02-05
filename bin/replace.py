#!/usr/bin/python
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
##############################################################################
import sys
import os
import re
import getopt
import shutil
import difflib

searchString = None
replaceString = None
directory = None
doNothing = False
undo = False
clean = False
query = False
regularExpression = False
backupExtension = "replace_bak"
fileName = None

def help():
    print """
Usage: replace.py [options] filename
   or: replace.py [options] -d directoryname
    -s "search string"\tstring to search for
    -S filename\tFile that contains the (multiline) string to search for
    -r "replace string"\treplace "search string" with this string
    -R filename\tFile that contains the (multiline) string to replace the search string
    -x \t\t\tuse regular expression during the search
    -c \t\t\tremove all backup files
    -d "directory"\tpeform replace in this dir
    -e extension\tBackup extension (default \""""+backupExtension+"""\")
    -n \t\t\tdry run (print output)
    -q \t\t\tquery before replacing
    -u \t\t\tundo all changes
    -h | -v | -?\tprint this help
    """
    sys.exit()

def undoAll():
    if directory==None:
        help()
    for (dirpath, dirnames, filenames) in os.walk(directory):
        for i in [f for f in filenames if re.match("^.*\."+backupExtension+"$", f)]:
            shutil.move(os.path.join(dirpath ,i), os.path.join(dirpath, i.rstrip(backupExtension)[:-1]))

def removeAllBackupFiles():
    if directory==None:
        help()
    for (dirpath, dirnames, filenames) in os.walk(directory):
        for i in [f for f in filenames if re.match("^.*\."+backupExtension+"$", f)]:
            os.remove(os.path.join(dirpath, i))

def replaceInFile(oldFile, query):

    if doNothing:
        fileContent = file(oldFile).read()
        if(regularExpression):
            expr = re.compile(searchString, re.DOTALL)
            newFileContent = expr.sub(replaceString, fileContent)
        else:
            newFileContent = fileContent.replace(searchString, replaceString)
        if not newFileContent == fileContent:
            return(difflib.unified_diff(fileContent.split("\n"), newFileContent.split("\n")))
        else:
            return None

    backupFile = oldFile+"."+backupExtension
    if os.path.isfile(backupFile):
        answer = raw_input("An backup file is already existing! Overwrite? (Y/n)")
        if answer.lower() == 'y':
            pass
        elif answer.lower() == 'n':
            print "Not touching. No changes will be applied!!!"
            return None
        else:
            print "Please answer 'y' or 'n'"
            replaceInFile(oldFile)

    shutil.move(oldFile, backupFile)
    fileContent = file(backupFile).read()

    if(regularExpression):
        expr = re.compile(searchString, re.DOTALL)
        newFileContent = expr.sub(replaceString, fileContent)
    else:
        newFileContent = fileContent.replace(searchString, replaceString)

    newFile = file(oldFile, 'w')
    newFile.write(newFileContent)
    newFile.close()

    if not newFileContent == fileContent:
        diffs = difflib.unified_diff(fileContent.split("\n"), newFileContent.split("\n"))
        if query == True:
            print "About to make the following change:"
            for diff in diffs:
                print diff
            answer = raw_input("Do you want to commit this change? (y = yes / a = abort without any changes / n = no change (default))")
            if answer.lower() == 'y':
                pass
            elif answer.lower() == 'a':
                print "Aborting ..."
                shutil.move(backupFile, oldFile)
                import sys
                sys.exit(1)
            else:
                print "Not touching. No changes will be applied!!!"
                return None
        shutil.copymode(backupFile, oldFile)
        return diffs
    shutil.copymode(backupFile, oldFile)

    return None

def replaceDir(query):
    if directory==None or searchString==None or replaceString==None or doNothing==None:
        help()

    for (dirpath, dirnames, filenames) in os.walk(directory):
            for i in [f for f in filenames if re.match("^.*(\.hpp|\.cpp|\.h|\.py|Conscript)$", f) and not f == 'bversion.hpp' and not os.path.islink(os.path.join(dirpath, f))]:
                oldFile = os.path.join(dirpath, i)
                replace(oldFile, query)

def replace(fName, query):
    if fName==None or searchString==None or replaceString==None or doNothing==None:
        help()
    print "Replacing in: " + fName
    changes = replaceInFile(fName, query)
    if not changes == None:
        print "Replaced following in " + fName + ":"
        for c in changes:
            print c
    else:
        if not doNothing:
            os.remove(fName+'.'+backupExtension)
        print "Replaced nothing in " + fName + ":"


try:
    optlist, args = getopt.getopt(sys.argv[1:], 'd:s:r:S:R:xncuhvq?')
except getopt.GetoptError, x:
    print "\nError: "+str(x)
    help()

for opt, value in optlist:
    if opt == '-s':
        searchString = value
    elif opt == '-r':
        replaceString = value
    elif opt == '-S':
        searchString = file(value).read()
    elif opt == '-R':
        replaceString = file(value).read()
    elif opt == '-x':
        regularExpression = True
    elif opt == '-e':
        backupExtension = value
    elif opt == '-c':
        clean = True
    elif opt == '-d':
        directory = value
    elif opt == '-n':
        doNothing = True
    elif opt == '-q':
        query = True
    elif opt == '-u':
        undo=True
    elif opt == '-h' or opt == '-?' or opt == '-v':
        help()

if undo == True:
    undoAll()
elif clean == True:
    removeAllBackupFiles()
elif len(args) > 0:
    for fn in args:
        replace(fn, query)
else:
    replaceDir(query)
