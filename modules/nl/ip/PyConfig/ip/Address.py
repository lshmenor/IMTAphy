###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2007
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 5, D-52074 Aachen, Germany
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

class ipv4Address:
    
    address = long(0)
    
    def __init__(self, _buildFrom):
        if (type(_buildFrom)==type(long()) or (type(_buildFrom) == type(int()))):
            self.address = _buildFrom
        else:
            self.address  = long(0)
            self.address  = long(_buildFrom.split(".")[0]) << 24
            self.address += long(_buildFrom.split(".")[1]) << 16
            self.address += long(_buildFrom.split(".")[2]) << 8
            self.address += long(_buildFrom.split(".")[3])
            
    def __str__(self):
        strRepr =        str((self.address >> 24).__and__(0xFF))
        strRepr += "." + str((self.address >> 16).__and__(0xFF))
        strRepr += "." + str((self.address >> 8).__and__(0xFF))
        strRepr += "." + str((self.address).__and__(0xFF))
        return strRepr
    
    def __add__(self, added):
        return ipv4Address(self.address + int(added))
    
    def __sub__(self, subtracted):
        return ipv4Address(self.address - int(subtracted))
        
        
