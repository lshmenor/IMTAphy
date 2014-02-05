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

#ifndef WNS_EVALUATION_STATISTICS_PDF_HPP
#define WNS_EVALUATION_STATISTICS_PDF_HPP

#include <WNS/evaluation/statistics/stateval.hpp>

#include <vector>

namespace wns { namespace evaluation { namespace statistics {
            /**
             * @brief Class PDF: Probability Distribution Function. This class
             * can be used to output the distribution function, the complementary
             * distribution function, and the probability function of a given
             * random x-sequence. The user only has to define the left and right
             * border of the x-axis, the given x-values are expected to lie in, and
             * the resolution of this range on the x-axis, i.e. the number of
             * intervals. This algorithm does not consider correlation at all and,
             * thus, cannot be used for controlling the length of a simulation
             * run. This algorithm is especially useful, when you have to examine a
             * fixed set of values or when you want to gain a first notion of the
             * behaviour of a correlated sequence of values.
             */
            class PDF:
                public StatEval
            {
            public:

                /**
                 * @brief scale type of the x-axis
                 */
                enum scaleType
                {
                    logarithmical,
                    linear
                };

                PDF(double minXValue,
                    double maxXValue,
                    unsigned long int resolution,
                    scaleType scaleType,
                    formatType format,
                    std::string name,
                    std::string description);

                PDF(const wns::pyconfig::View& config);

                PDF(const PDF& other);

                virtual ~PDF();

                /**
                 * @brief Normal output
                 */
                virtual void
                print(std::ostream& stream = std::cout) const;

                /**
                 * @brief Input a value to the statistical evaluation
                 */
                virtual void
                put(double value);

                /**
                 * @brief Reset evaluation algorithm to its initial state
                 */
                virtual void
                reset();

            private:

                /**
                 * @brief Return statistical information of the given interval
                 */
                void
                getResult(unsigned long int index,
                          double& abscissa,
                          double& f,
                          double& G,
                          double& P) const;

                /**
                 * @brief Calculate the index in the array for the given Value
                 */
                unsigned long int
                getIndex(double value) const;

                /**
                 * @brief Calculate the abscissa value for the given index
                 */
                double
                getAbscissa(unsigned long int index) const;

                class PercentileError :
                    public std::exception
                {
                };

                class PercentileUnderFlow :
                    public PercentileError
                {
                };

                class PercentileOverFlow :
                    public PercentileError
                {
                };

                /**
                 * @breif Get the (approximated) p-th percentile, throws a
                 * PercentileError if not possible
                 */
                double
                getPercentile(int p) const;

                /**
                 * @brief Print String representation of p-th percentile into stream
                 */
                void printPercentile(int p,
                                     std::ostream& stream = std::cout) const;

                /**
                 * @brief Left border of the x-axis
                 */
                double minXValue_;

                /**
                 * @brief Right border of the x-axis
                 */
                double maxXValue_;

                /**
                 * @brief Resolution of the x-axis
                 */
                unsigned long int resolution_;

                /**
                 * @breif Scale of the x-axis
                 */
                scaleType scaleType_;

                /**
                 * @brief Array containing occurrences of all values
                 */
                std::vector<int> values_;

                int underFlows_;

                int overFlows_;

                /**
                 * @brief Probe name
                 */
                std::string name_;

                /**
                 * @breif Probe description
                 */
                std::string desc;
            };

} // statistics
} // evaluation
} // wns

#endif  // WNS_EVALUATION_STATISTICS_PDF_HPP

