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

import math

class Vector:
    """ This represents a geometric shift in space (3D). Unit is meters [m] """
    __slots__ = ["x", "y", "z"]

    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = x
        self.y = y
        self.z = z

    def toString(self,precision=0):
        format = '%%.%df %%.%df %%.%df'%(precision,precision,precision)
        return (format % (self.x, self.y, self.z))
    def __str__(self):
        return self.toString()

    def positionDifference(pos1,pos2):
        """ Difference of two position points
            vector = Vector.positionDifference(position-center)
        """
        return Vector(pos1.x-pos2.x,pos1.y-pos2.y,pos1.z-pos2.z)

    def __mul__(self, other):
        """ vector2 = vector1 * 10.0 """
        return Vector(self.x*other,self.y*other,self.z*other)
    def __div__(self, other):
        """ vector2 = vector1 / 10.0 """
        other = float(other)
        return Vector(self.x/other,self.y/other,self.z/other)
    def __eq__(self, other):
        """ vector2 == vector1 ? """
        if not other.__dict__.has_key('x'): # be sure other is also a Vector
            return False
        return self.x==other.x and self.y==other.y and self.z==other.z

    def angle2D(self):
        a = math.atan2(self.y, self.x) # in [-pi..pi]
        return a
    def length(self):
        return math.sqrt(self.x*self.x+self.y*self.y+self.z*self.z)
    def length2D(self):
        return math.sqrt(self.x*self.x+self.y*self.y)
    def turn2D(self,angle): # 0 <= angle <= 2.0*math.pi
        """ turn right by given angle. Return result """
        angle = angle + self.angle2D()
        length = self.length()
        return Vector(length*math.cos(angle),length*math.sin(angle),self.z)
    def toPosition(self):
        return Position(self.x,self.y,self.z)


class Position:
    """ This represents a geometric position in space (3D). Unit is meters [m] """
    __slots__ = ["x", "y", "z"]
    label = None

    def __init__(self, x=0.0, y=0.0, z=0.0, label=None):
        self.x = x
        self.y = y
        self.z = z
        self.label = label

    def toString(self,precision=2):
        format = '%%.%df %%.%df %%.%df'%(precision,precision,precision)
        return (format % (self.x, self.y, self.z))
    def __str__(self):
        return self.toString()

    def toLabeledString(self):
        return ('%s(%d,%d,%d)' % (self.label, self.x, self.y, self.z))

    def getLabel(self):
        return label

    def setLabel(self,label):
        self.label = label

    def __add__(self,other):
        return Position(self.x+other.x,self.y+other.y,self.z+other.z,self.label)
    def __sub__(self,other):
        return Vector(self.x-other.x,self.y-other.y,self.z-other.z)
    def __eq__(self, other):
        """ vector2 == vector1 ? """
        if not other.__dict__.has_key('x'): # be sure other is also a Position
            return False
        return self.x==other.x and self.y==other.y and self.z==other.z

def PositionFromVector(vector):
    return Position(vector.x,vector.y,vector.z)

class Line:
    """ This represents a line between two endpoints (2D). Unit is meters [m] """
    __slots__ = ["x1", "y1", "x2", "y2"]

    def __init__(self, x1=0, y1=0, x2=0, y2=0):
        self.x1 = x1
        self.y1 = y1
        self.x2 = x2
        self.y2 = y2

    def getPos1(self):
        return Position(self.x1,self.y1,0)
    def getPos2(self):
        return Position(self.x2,self.y2,0)

    def toString(self):
        return ('(%d,%d)->(%d,%d)' % (self.x1, self.y1, self.x2, self.y2))
    def __str__(self):
        return self.toString()

class BoundingBox:
    """ Box with x,y,z dimension """
    __slots__ = ["xmin","xmax", "ymin","ymax", "zmin","zmax"]

    def __init__(self, xmin = 1e9, xmax = -1e9, ymin = 1e9, ymax = -1e9, zmin = 1e9, zmax = -1e9):
        self.xmin = xmin
        self.ymin = ymin
        self.zmin = zmin
        self.xmax = xmax
        self.ymax = ymax
        self.zmax = zmax

    def extendToIncludePosition(self,point):
        if (point.x<self.xmin):
            self.xmin=point.x
        if (point.y<self.ymin):
            self.ymin=point.y
        if (point.z<self.zmin):
            self.zmin=point.z
        if (point.x>self.xmax):
            self.xmax=point.x
        if (point.y>self.ymax):
            self.ymax=point.y
        if (point.z>self.zmax):
            self.zmax=point.z

    def extendToIncludeLine(self,line):
        self.extendToIncludePosition(line.getPos1())
        self.extendToIncludePosition(line.getPos2())

    def roundToNextNiceNumber(self,factor=1.0):
        """ factor can be 1,10,100 e.g. to round to next decade """
        self.xmin=math.floor(self.xmin/factor)*factor
        self.ymin=math.floor(self.ymin/factor)*factor
        self.zmin=math.floor(self.zmin/factor)*factor
        self.xmax=math.ceil(self.xmax/factor)*factor
        self.ymax=math.ceil(self.ymax/factor)*factor
        self.zmax=math.ceil(self.zmax/factor)*factor
