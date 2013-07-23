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

from openwns.wrowser.simdb.Parameters import Parameters, Bool, Int, Float, String

class Set(Parameters):
    scenario = String()                 # UMa, UMi, RMa, SMa and InH
    seed = Int()                        # any number
    numBSAntennas = Int()
    numMSAntennas = Int()               #
    scmLinks = String()                 # all or serving

params = Set()

#
# now the Parameters in params get populated with different values. Each time "write" is called the current values fixed.
#

for seed in range(3):
    for scenario in ["UMa", "RMa", "SMa", "UMi", "InH"]:
        for antennas in [12, 21]:
            for links in ["all", "serving", "no"]:
                params.seed = seed
                params.scenario = scenario
                if antennas == 12:
                    params.numMSAntennas = 1
                    params.numBSAntennas = 2
                else:
                    params.numMSAntennas = 2
                    params.numBSAntennas = 1
                params.scmLinks = links
                params.write()

# don't forget to comment the default params class in config.py
