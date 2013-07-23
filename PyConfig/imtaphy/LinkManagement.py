################################################################################
# This file is part of IMTAphy
# _____________________________________________________________________________
#
# Copyright (C) 2010
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

import math
import openwns.geometry.position
import imtaphy.Logger

class LinkManager:
    classifier = None
    scmLinkCriterion = None
    handoverMargin = None
    wraparoundShiftVectors = None
    logger = None
    useSCMforRSRP = None
    
    # implemented scmLinkCriteria:
    # all: all links are SCM links
    # none: no links are SCM links (default)
    # serving: only links to serving BS are SCM

    def __init__(self, classifier, scmLinkCriterion = "all", handoverMargin = "1 dB", shiftVectors = [], logger = None, useSCMforRSRP = False):
        self.classifier = classifier
        self.scmLinkCriterion = scmLinkCriterion
        self.handoverMargin = handoverMargin
        self.wraparoundShiftVectors = shiftVectors
        self.useSCMforRSRP = useSCMforRSRP
        if logger is None:
            self.logger = logger = imtaphy.Logger.Logger("LinkManager")
        else:
            self.logger = logger


class StaticClassifier:
    scenario = None
    propagation = None
    outdoorPropagation = None
    userLocation = None
    nameInChannelFactory = "imtaphy.linkclassify.Static"

    def __init__(self, scenario, propagation, outdoorPropagation, userLocation):
        self.scenario = scenario
        self.propagation = propagation
        self.outdoorPropagation = outdoorPropagation
        self.userLocation = userLocation

# some helper classes derived from StaticClassifer to configure static links directly:


class ITUClassifier:
    scenarioType = None
    use2Ddistance = None
    onlyOutdoorDistanceUMi = None
    nameInChannelFactory = "imtaphy.linkclassify.ITU"

    def __init__(self, scenario = "UMa", use2Ddistance = True, onlyOutdoorDistanceUMi = True):
        self.scenarioType = scenario
        self.use2Ddistance = use2Ddistance
        self.onlyOutdoorDistanceUMi = onlyOutdoorDistanceUMi

# some helper classes derived from ITUClassifer to configure static links directly:

class InH(ITUClassifier):
    def __init__(self):
        ITUClassifier.__init__(self)
        self.scenarioType = "InH"
class UMi(ITUClassifier):
    def __init__(self):
        ITUClassifier.__init__(self)
        self.scenarioType = "UMi"
class SMa(ITUClassifier):
    def __init__(self):
        ITUClassifier.__init__(self)
        self.scenarioType = "SMa"
class UMa(ITUClassifier):
    def __init__(self):
        ITUClassifier.__init__(self)
        self.scenarioType = "UMa"
class RMa(ITUClassifier):
    def __init__(self):
        ITUClassifier.__init__(self)
        self.scenarioType = "RMa"


def computeShiftVectors(isd, numCircles):
    shiftVectors = []

    # law of cosines: c^2 = a^2 + b^2 -2*a*b*cos(gamma) with gamma opposite c
    # also: cos(alpha) = (a^2 - b^2 - c^2) / (-2*b*c)
    # we look at the center site and the shifted copy towards the upper right
    # the center site is the left corner of a triangle, the triangle's side a is
    # the connection along a multiple of inter-site-distances to the upper right
    # the side b goes from there straight up to the center of the shifted position
    # c is the triangle's side that connects the center with the center of the
    # the shifted cells. gamma (opposite c) is 120 deg and a and b are multiple
    # of inter-site disntances. The start angle for creating the positions via
    # (cos, sin) in the loop below is the angle between a horizontal line and a.
    # This equals to 90 - alpha which is the angle on top at the corner of the
    # shifted position

    if (numCircles == 1):
        a = 2.0 * isd
        b = 1.0 * isd
    elif (numCircles == 2):
        a = 3.0 * isd
        b = 2.0 * isd
    else:
        return []

    wrapAroundRadius = math.sqrt(a*a + b*b - 2.0*a*b*math.cos(120.0 * math.pi / 180.0))
    wrapAroundAngle = math.pi * 90.0 / 180.0 - math.acos((a*a - b*b - wrapAroundRadius*wrapAroundRadius) /
                                                         (-2.0 * b * wrapAroundRadius))

    for i in range(6):
        angle = wrapAroundAngle + i * 60.0 * math.pi / 180.0
        vector = openwns.geometry.position.Vector(x = wrapAroundRadius * math.cos(angle),
                                                  y = wrapAroundRadius * math.sin(angle),
                                                  z = 0)
        shiftVectors.append(vector)
    return shiftVectors

