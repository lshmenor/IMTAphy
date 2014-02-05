/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WNS/distribution/Weibull.hpp>

using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Weibull,
    Distribution,
    "Weibull",
    wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Weibull,
    Distribution,
    "Weibull",
    wns::distribution::RNGConfigCreator);

Weibull::Weibull(double scale, double shape, wns::rng::RNGen* rng) :
  Distribution(rng)
{
  assert((scale > 0.0) && (shape > 0.0));
  scale_ = scale;
  shape_ = shape;

  mean_ = scale_ * gamma(1 + (1 / shape_));
  variance_ = pow(scale_, 2) * gamma( 1 + (2 / shape_)) - pow(mean_, 2);
}

Weibull::Weibull(const pyconfig::View& config) :
  Distribution(),
  shape_(config.get<double>("shape")),
  scale_(config.get<double>("scale"))
{
  assert((scale_ > 0) && (shape_ > 0));

  mean_ = scale_ * gamma(1 + (1 / shape_));
  variance_ = pow(scale_, 2) * gamma( 1 + (2 / shape_)) - pow(mean_, 2);
}


Weibull::Weibull(wns::rng::RNGen* rng, const pyconfig::View& config) :
  Distribution(),
  shape_(config.get<double>("shape")),
  scale_(config.get<double>("scale"))
{
  assert((scale_ > 0) && (shape_ > 0));

  mean_ = scale_ * gamma(1 + (1 / shape_));
  variance_ = pow(scale_, 2) * gamma( 1 + (2 / shape_)) - pow(mean_, 2);
}

Weibull::~Weibull()
{
}

/* - Parameters:
      1. Scale (notation: alpha or lambda )
      2. Shape (notation: k or beta)
  - Sources:
      1. Algorithm in this method is described here:
         http://en.wikipedia.org/wiki/Weibull_distribution#Generating_Weibull-distributed_random_variates
      2. http://de.wikipedia.org/wiki/Weibull-Verteilung
      3 .http://en.wikipedia.org/wiki/Gamma_function
*/

double
Weibull::operator()()
{
  /* generates uniform random value between (0,1) */
  uniDis = new wns::distribution::Uniform(0.0, 1.0);
  double uniRandomValue = (*uniDis)();

  double x = scale_ * pow((-1) * log(uniRandomValue), 1 / shape_);

  return x;
}

double
Weibull::getMean() const
{
  return mean_;
}

/*
  This method implements the gamma function
  source: http://www.crbond.com/download/gamma.cpp
*/
double
Weibull::gamma(double x)
{
  int i,k,m;
  double ga,gr,z;
  double r = 1.0;

   static double g[] = {
     1.0,
     0.5772156649015329,
     -0.6558780715202538,
     -0.420026350340952e-1,
     0.1665386113822915,
     -0.421977345555443e-1,
     -0.9621971527877e-2,
     0.7218943246663e-2,
     -0.11651675918591e-2,
     -0.2152416741149e-3,
     0.1280502823882e-3,
     -0.201348547807e-4,
     -0.12504934821e-5,
     0.1133027232e-5,
     -0.2056338417e-6,
     0.6116095e-8,
     0.50020075e-8,
     -0.11812746e-8,
     0.1043427e-9,
     0.77823e-11,
     -0.36968e-11,
     0.51e-12,
     -0.206e-13,
     -0.54e-14,
     0.14e-14};

   if (x > 171.0) {
     /* This value is an overflow flag. */
     return 1e308;
   }

   if (x == (int)x) {
     if (x > 0.0) {
       /* use factorial */
       ga = 1.0;
       for (i=2;i<x;i++) {
	 ga *= i;
       }
     }
     else
       ga = 1e308;
   }
   else {
     if (fabs(x) > 1.0) {
       z = fabs(x);
       m = (int)z;
       r = 1.0;
       for (k=1;k<=m;k++) {
	 r *= (z-k);
       }
       z -= m;
     }
     else
       z = x;
     gr = g[24];
     for (k=23;k>=0;k--) {
       gr = gr*z+g[k];
     }
     ga = 1.0/(gr*z);
     if (fabs(x) > 1.0) {
       ga *= r;
       if (x < 0.0) {
	 ga = -M_PI/(x*ga*sin(M_PI*x));
       }
     }
   }
   return ga;
}


std::string
Weibull::paramString() const
{
	std::ostringstream tmp;
	tmp << "Weibull(shape = " << shape_ << ", scale = " << scale_
	    << ", mean = " << mean_ << ", variance = " << variance_ << ")";
	return tmp.str();
}
