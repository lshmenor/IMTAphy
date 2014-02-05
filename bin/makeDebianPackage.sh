#!/bin/bash
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
# This script will create a Debian package of the sandbox as it is and
# prepare it for installation into /usr/share/wns/
# the source code can be found below /usr/src/wns/

set -e

echo "Preparing build place ..."
# throw away old stuff
rm -rf __packaging_tmp
# copy skeleton
cp -a packaging __packaging_tmp
# delete all arch-ids (should not appear in debian package)
find __packaging_tmp -name ".arch-ids" | xargs rm -rf

echo "Copying '[dbg|opt]/[bin|lib]' and 'default' from sandbox ..."
mkdir -p __packaging_tmp/usr/share/wns/dbg/bin
mkdir -p __packaging_tmp/usr/share/wns/dbg/lib
mkdir -p __packaging_tmp/usr/share/wns/opt/bin
mkdir -p __packaging_tmp/usr/share/wns/opt/lib
mkdir -p __packaging_tmp/usr/share/wns/src

cp -a   sandbox/default __packaging_tmp/usr/share/wns/default
cp -Lrp sandbox/dbg/bin __packaging_tmp/usr/share/wns/dbg/
cp -Lrp sandbox/dbg/lib __packaging_tmp/usr/share/wns/dbg/
cp -Lrp sandbox/opt/bin __packaging_tmp/usr/share/wns/opt/
cp -Lrp sandbox/opt/lib __packaging_tmp/usr/share/wns/opt/
cp -a   framework       __packaging_tmp/usr/share/wns/src/
cp -a   modules         __packaging_tmp/usr/share/wns/src/

echo "Copying examples (tests) ..."
cp -a tests __packaging_tmp/usr/share/wns/examples
# delete all arch-ids (should not appear in debian package)
find __packaging_tmp -name ".arch-ids" | xargs rm -rf
find __packaging_tmp -name "{arch}" | xargs rm -rf
find __packaging_tmp/usr/share/wns/src -name "build" | xargs rm -rf
find __packaging_tmp/usr/share/wns/src -name ".scons*" | xargs rm -rf

# remove some more unwanted stuff
find __packaging_tmp/usr/share/wns/examples -name "output" | xargs rm -rf
find __packaging_tmp/usr/share/wns/examples -name "output_*.py" | xargs rm -rf
find __packaging_tmp/usr/share/wns/examples -name "wns-core" | xargs rm -rf
find __packaging_tmp/usr/share/wns/examples -name "err_log" | xargs rm -rf
find __packaging_tmp/usr/share/wns/examples -name "std_log" | xargs rm -rf

# remove unwanted .pyc files
find __packaging_tmp -name "*.pyc" | xargs rm -rf

# delete all arch-ids (should not appear in debian package)
find __packaging_tmp -name ".arch-ids" | xargs rm -rf

echo "Building package ..."
dpkg -b __packaging_tmp wns.deb

# throw away stuff
rm -rf __packaging_tmp
