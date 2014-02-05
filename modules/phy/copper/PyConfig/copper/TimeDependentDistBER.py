###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2009
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

import openwns.distribution
import openwns.pyconfig
from math import pi
from math import exp
from math import log10


class TimeDependentDistBER(openwns.pyconfig.Sealed):
    """This class is providing a time distributed BER in the form:
               BER
               | *                     *
               |  *                   *
               |   *                 *
               |    *               *
               |     *             *
               |      *           *
               |       *         *
               |        *       *
               |         *     *
               |           ***
               |____________|____________ time

    Distance:start        middle       end

    After giving the start,the middle distance and the step the rest from middle to end will be generated automatically.
    The BER is then calculated from the distances between the nodes and is distributed in time
    usind wns.Distribution.TimeDependent(time,wns.Distribution.Uniform).
    The curve can be repeated as many times as needed.
    """
    B=None
    Ps=None
    gs= None
    gr=None
    gamma=None
    f= None
    c= None
    _lambda=None
    d0=None
    k=None
    T=None

    def __init__ (self, dataRate, efficiency =1.0, Ps = 0.1, gs = 1, gr = 1, gamma = 2.4, f = 5.5*1E+9, c = 3.0*1E+8, d0 = 1.0, k = 1.38*1E-23, T = 290):
        self.B = dataRate/efficiency
        self.Ps = Ps
        self.gs = gs
        self.gr = gr
        self.gamma = gamma
        self.f = f
        self.c = c
        self._lambda = c/f
        self.d0 = d0
        self.k = k
        self.T = T

    def getDistribution(self, simulationTime, repeatBERCurve, startDistance, middleDistance, step):
        dist  =  openwns.distribution.TimeDependent()
        start = startDistance
        middle = middleDistance
        distanceList = []
        step = step
        time = 0
        last = None
        for i in xrange(start, middle, -step):
            distanceList.append(i)
            last=i
        for i in xrange(last, start+step, step):
            distanceList.append(i)

        deltaT = (simulationTime/repeatBERCurve) / len(distanceList)
        for k in xrange(repeatBERCurve):
            for j in xrange(len(distanceList)):
                dist.eventList.append(openwns.distribution.Event(time, openwns.distribution.Uniform(1.4*self.getBER(distanceList[j]), 0.6*self.getBER(distanceList[j]))))
                time = time + deltaT
        return dist

    def getBER(self, distance):
        Noise=self.k*self.T*self.B
        Noise_dbm=10*log10(Noise*1000)
        const=self.Ps*self.gs*self.gr*pow((self._lambda/(4*pi*self.d0)),2)
        Pr=const*pow((self.d0/distance),self.gamma)
        SINR=10*log10(Pr*1000)-Noise_dbm
        BER=self.getQ(pow(2*SINR,0.5))
        return BER

    def getQ(self, x):
        Q=((1.0/x*pow(2*pi,0.5))*exp(-(pow(x,2)/2)))
        return Q

    def findDistanceForThreshold(self, distance, threshold, side):
        # side = 1 means bigger than the threshold, side = 0 means smaller than the threshold
        if side == 1:
                if self.getBER(distance) >= threshold:
                    return distance
        if side == 0:
                if self.getBER(distance) < threshold:
                    return distance
        return 0
    def findDistanceForThresholdFromList(self, distanceList, threshold, side):
        # side = 1 means bigger than the threshold, side = 0 means smaller than the threshold
        if side == 1:
            for j in xrange(len(distanceList)):
                if self.getBER(distanceList[j]) >= threshold:
                    return distanceList[j]
        if side == 0:
            for i in xrange(len(distanceList)):
                if self.getBER(distanceList[i])<threshold:
                    return distanceList[i]
