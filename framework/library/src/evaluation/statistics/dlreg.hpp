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

#ifndef WNS_EVALUATION_STATISTICS_DLREG_HPP
#define WNS_EVALUATION_STATISTICS_DLREG_HPP


#include <WNS/evaluation/statistics/dlre.hpp>

namespace wns { namespace evaluation { namespace statistics {
            /**
             * @brief Class DLREG: Discrete LRE (LRE III) for complementary
             * distribution function
             */

            class DLREG : public DLRE
            {
            public:

                /**
                 * @brief Constructor for non-equi-distant x-values
                 */
                DLREG(std::vector<double> xValuesArr,
                      int level,
                      double error,
                      double preFirst,
                      std::string name,
                      std::string description,
                      bool forceRMinusAOk,
                      double gMin,
                      int maxNrv,
                      int skipInterval,
                      formatType format);

                /**
                 * @brief  Constructor for equi-distant x-values
                 */
                DLREG(double xMin,
                      double xMax,
                      double intSize,
                      double error,
                      double preFirst,
                      std::string name,
                      std::string description,
                      bool forceRMinusAOk,
                      double gMin,
                      int maxNrv,
                      int skipInterval,
                      formatType aFormat);

                DLREG(const wns::pyconfig::View& config);

                ~DLREG();

                /**
                 * @brief Print output
                 */
                virtual void
                print(std::ostream& stream = std::cout) const;

                /**
                 * @brief Put new value to probe
                 */
                void
                put(double value);

                /**
                 * @brief Return current g-level
                 */
                virtual double
                curGLev() const;

                /**
                 * @brief Return the g-level of x(t)
                 */
                virtual double
                g(double xt) const;

                /**
                 * @brief Result line
                 */
                virtual void
                getResultLine(const int index, ResultLine& line) const;

                /**
                 * @brief Change max. error
                 */
                virtual void
                changeError(double newError);

            private:

                // runtime control
                DLRE::Phase rtc();

                // minimum g level
                double  gMin_;

            };

} // end namespace statistics
} // end namespace evaluation
} // end namespace wns
#endif  // WNS_EVALUATION_STATISTICS_DLREG_HPP
