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

import pyconfig

def asDistribution(it):
    if isinstance(it, Distribution):
        return it

    return Fixed(it)

class Distribution(object):
    def __init__(self, **kw):
        pyconfig.attrsetter(self, kw)

    def __add__(self, value):
        value = asDistribution(value)
        return ADD(self, value)

    def __sub__(self, value):
        value = asDistribution(value)
        return SUB(self, value)

    def __mul__(self, value):
        value = asDistribution(value)
        return MUL(self, value)

    def __div__(self, value):
        value = asDistribution(value)
        return DIV(self, value)


    def above(self, value):
        return ABOVE(self, value)

    def below(self, value):
        return BELOW(self, value)


class Fixed(Distribution):
    __plugin__ = 'Fixed'

    value = None
    mean = None
    def __init__(self, value, **kw):
        self.value = value
        super(Fixed, self).__init__(**kw)
        self.mean = value

class Normal(Distribution):
    __plugin__ = 'Norm'

    mean = None
    variance = None
    def __init__(self, mean, variance, **kw):
        self.mean = mean
        self.variance = variance
        super(Normal, self).__init__(**kw)

class NegExp(Distribution):
    __plugin__ = 'NegExp'

    mean = None
    def __init__(self, mean, **kw):
        self.mean = mean
        super(NegExp, self).__init__(**kw)

class Uniform(Distribution):
    __plugin__ = 'Uniform'

    low  = None
    high = None
    mean = None
    def __init__(self, high = 1.0, low = 0.0, **kw):
        if low > high:
            low, high = high, low
        self.low = low
        self.high = high
        super(Uniform, self).__init__(**kw)
        self.mean = (self.low+self.high)/2.0
        
class DiscreteUniform(Distribution):
    __plugin__ = 'DiscreteUniform'

    low  = None
    high = None
   
    def __init__(self, high = 1, low = 0, **kw):
        if low > high:
            low, high = high, low
        self.low = low
        self.high = high
        super(DiscreteUniform, self).__init__(**kw)
                
class StandardUniform(Uniform):

    def __init__(self, **kw):
        super(StandardUniform, self).__init__(1.0, 0.0, **kw)
        __plugin__ = "StandardUniform"


class Binomial(Distribution):
    __plugin__ = 'Binomial'

    numberOfTrials = None
    probability = None
    mean = None
    def __init__(self, N, p, **kw):
        self.numberOfTrials = N
        self.probability = p
        super(Binomial, self).__init__(**kw)
        self.mean = self.probability * self.numberOfTrials

class Geometric(Distribution):
    __plugin__ = 'Geometric'

    mean = None
    def __init__(self, mean, **kw):
        self.mean = mean
        super(Geometric, self).__init__(**kw)

class Erlang(Distribution):
    __plugin__ = 'Erlang'

    rate = None
    shape = None
    def __init__(self, rate, shape, **kw):
        self.rate = rate
        self.shape = shape
        super(Erlang, self).__init__(**kw)

class Poisson(Distribution):
    __plugin__ = 'Poisson'

    mean = None
    def __init__(self, mean, **kw):
        self.mean = mean
        super(Poisson, self).__init__(**kw)
        
class Rice(Distribution):
    __plugin__ = 'Rice'

    losFactor = None
    variance = None
    
    def __init__(self, losFactor, variance, **kw):
        self.losFactor = losFactor
        self.variance = variance
        super(Rice, self).__init__(**kw)

class Pareto(Distribution):
    __plugin__ = 'Pareto'

    shapeA = None
    scaleB = None # proportional to mean output
    xMin = None
    xMax = None # limit the (heavy tailed) output
    mean = None
    def __init__(self, shapeA, scaleB, xMin=0.0, xMax=1e100, **kw):
        self.shapeA = shapeA
        self.scaleB = scaleB
        self.xMin = xMin
        self.xMax = xMax
        super(Pareto, self).__init__(**kw)
        self.mean = self.shapeA*self.scaleB / (self.shapeA - 1.0)

class LogNorm(Distribution):
    __plugin__ = 'LogNorm'

    mean = None
    std = None
    def __init__(self, mean, std, **kw):
        self.mean = mean
        self.std = std
        super(LogNorm, self).__init__(**kw)

class Weibull(Distribution):
    __plugin__ = 'Weibull'

    scale = None
    shape = None
    def __init__(self, shape, scale, **kw):
        self.scale = scale
        self.shape = shape
        super(Weibull, self).__init__(**kw)

class Cauchy(Distribution):
    __plugin__ = 'Cauchy'

    median = None
    sigma = None
    def __init__(self, median, sigma, **kw):
        self.median = median
        self.sigma = sigma
        super(Cauchy, self).__init__(**kw)

class TimeDependent(Distribution):
    __plugin__ = 'wns.distribution.TimeDependent'

    eventList = None

    def __init__(self):
        super(TimeDependent, self).__init__()
        self.eventList = []


class Event(object):
    activationTime = None
    distribution = None

    def __init__(self, activationTime, distribution):
        self.activationTime = activationTime
        self.distribution = distribution


# binary operator
class Binary(Distribution):
    first = None
    second = None
    def __init__(self, first, second):
        self.first = first
        self.second = second

class ADD(Binary):
    __plugin__ = 'ADD'

class MUL(Binary):
    __plugin__ = 'MUL'

class SUB(Binary):
    __plugin__ = 'SUB'

class DIV(Binary):
    __plugin__ = 'DIV'

class WithFloat(Distribution):
    subject = None
    arg = None
    def __init__(self, subject, arg):
        assert type(arg) == type(1.0)
        self.subject = subject
        self.arg = arg

class ABOVE(WithFloat):
    __plugin__ = 'ABOVE'

class BELOW(WithFloat):
    __plugin__ = 'BELOW'

class CDFTable(Distribution):
    # find more Distributions derived from CDFTable() in
    # distribution/CDFTables.py

    __plugin__ = 'CDFTable'

    # list of (value, CDF-value)-pairs
    cdfTable = None

class ExampleCDFTable(CDFTable):
    # (value, CDF-value)
    cdfTable = ((40, 0.5),
                (1500, 1.0))

