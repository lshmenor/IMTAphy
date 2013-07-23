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

import imtaphy.covarianceEstimation

class Filter:
    nameInFactory = None
    def __init__(self):
        pass

class No(Filter):
    nameInFactory = "imtaphy.receiver.filter.NoFilter"
    def __init__(self):
        pass
        
class MRCFilter(Filter):
    nameInFactory = "imtaphy.receiver.filter.MRC"
    def __init__(self):
        pass
    
class MMSEFilter(Filter):
    nameInFactory = "imtaphy.receiver.filter.MMSE"
    
    def __init__(self, maxRank = 0):
        self.maxRank = maxRank


    
class LinearReceiver:
    nameInReceiverFactory = "imtaphy.receiver.LinearReceiver"
    
    def __init__(self, logger, filter, noiseFigure = "7 dB", channelEstimation = None, covarianceEstimation = imtaphy.covarianceEstimation.Diagonal()):
        self.noiseFigure = noiseFigure
        self.logger = logger
        self.filter = filter
        self.covarianceEstimation = covarianceEstimation
        self.channelEstimation = channelEstimation # None means perfect channel estimation

class ProbingReceiver(LinearReceiver):
    nameInReceiverFactory = "imtaphy.receiver.ProbingReceiver"
    
    def __init__(self, logger, filter, noiseFigure = "7 dB", channelEstimation = None, covarianceEstimation = imtaphy.covarianceEstimation.Diagonal()):
        self.noiseFigure = noiseFigure
        self.logger = logger
        self.filter = filter
        self.covarianceEstimation = covarianceEstimation
        self.channelEstimation = channelEstimation # None means perfect channel estimation
