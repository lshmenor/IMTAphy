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

import imtaphy.Logger
import imtaphy.Spectrum
import openwns.geometry.position

class Channel:
    logger = imtaphy.Logger.Logger("TheChannel")
    pathlossModel = None
    fastFadingModel = None
    linkManager = None
    spectrum = None
    
    def __init__(self, pathlossModel, spatialChannelModel, linkManager, spectrum = imtaphy.Spectrum.Spectrum()):
        self.pathlossModel = pathlossModel
        self.spatialChannelModel = spatialChannelModel
        self.linkManager = linkManager
        self.spectrum = spectrum
