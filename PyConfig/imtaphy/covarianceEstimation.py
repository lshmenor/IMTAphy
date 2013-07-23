# -*- coding: utf-8 -*-
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


class Diagonal:
    nameInFactory = "imtaphy.receiver.covarianceEstimation.Diagonal"
    def __init__(self):
        pass

class EqualDiagonal:
    nameInFactory = "imtaphy.receiver.covarianceEstimation.EqualDiagonal"
    def __init__(self):
        pass
        
        
class Perfect:
    nameInFactory = "imtaphy.receiver.covarianceEstimation.Perfect"
    def __init__(self):
        pass

class GaussianError:
    nameInFactory = "imtaphy.receiver.covarianceEstimation.GaussianError"
    def __init__(self, relativeError_dB):
        self.relativeError = relativeError_dB

class IntraAndInterCellDistinguisher:
    nameInFactory = "imtaphy.receiver.covarianceEstimation.IntraAndInterCellDistinguisher"
    def __init__(self, interCellEstimation, intraCellEstimation = None):
        self.intraCellEstimation = intraCellEstimation
        self.interCellEstimation = interCellEstimation
        
class WishartModel36829:
    nameInFactory = "imtaphy.receiver.covarianceEstimation.WishartModel36829"
    def __init__(self, numberOfSamples):
        self.numberOfSamples = numberOfSamples
        
        
        