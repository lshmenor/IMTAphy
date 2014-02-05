/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#ifndef WNS_EVALUATION_STATISTICS_MOMENTS_HPP
#define WNS_EVALUATION_STATISTICS_MOMENTS_HPP

#include <WNS/evaluation/statistics/stateval.hpp>

namespace wns { namespace evaluation { namespace statistics {

            /**
             * @brief Class Moments: Simple stat. evaluation: mean, variance,
             * skewness etc.
             */
            class Moments: public StatEval
            {
            public:

                Moments();

                Moments(std::string name,
                        std::string description,
                        formatType  format);

                Moments(const wns::pyconfig::View& config);

                virtual ~Moments();

                virtual void
                print(std::ostream& stream = std::cout) const;

                /**
                 * @brief Input of value xI, weighted with wI
                 */
                virtual void
                put(double xI, double wI);

                /**
                 * @brief Input of value xI
                 */
                virtual void
                put(double xI);

                /**
                 * @brief Return mean value
                 *
                 *        __ n              1   __ n
                 * E{x} = \        x * p  = _ * \        x
                 *        /_ i = 1      i   n   /_ i = 1
                 *
                 */
                virtual double
                mean() const;

                /**
                 * @brief Return variance
                 *
                 *  2      2     2      1   __ n      2    2
                 * c  = E{x } - E {x} = - * \        x  - E {x}
                 *                      n   /_ i = 1
                 */
                virtual double
                variance() const;

                /**
                 * @brief Return size of 95%-confidence interval of mean
                 */
                virtual double
                getConfidenceInterval95Mean() const;

                /**
                 * @brief Return size of 99%-confidence interval of mean
                 */
                virtual double
                getConfidenceInterval99Mean() const;

                /**
                 * @brief Return moment m2
                 */
                virtual double
                M2() const;


                /**
                 * @brief Return moment m3
                 */
                virtual double
                M3() const;

                /**
                 * @brief Reset evaluation
                 */
                virtual void
                reset();

            protected:

                /**
                 * @brief Sum of weights
                 */
                double wSum_;

            private:

                double
                getConfidenceIntervalMean(double x) const;
            };

} // statistics
} // evaluation
} // wns

#endif  // WNS_EVALUATION_STATISTICS_MOMENTS_HPP


