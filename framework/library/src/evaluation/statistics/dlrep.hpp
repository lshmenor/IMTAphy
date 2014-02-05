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

#ifndef WNS_EVALUATION_STATISTICS_DLREP_HPP
#define WNS_EVALUATION_STATISTICS_DLREP_HPP

#include <WNS/evaluation/statistics/dlre.hpp>

namespace wns { namespace evaluation { namespace statistics {
            /**
             * @brief Class DLREP: Discrete LRE (LRE III), probability function
             */
            class DLREP : public DLRE
            {
            public:

                /**
                 * @brief Constructor for non-equi-distant x-values
                 */
                DLREP(std::vector<double> xValuesArr,
                      int level,
                      double error,
                      double preFirst,
                      std::string name,
                      std::string description,
                      bool forceRMinusAOk,
                      int maxNrv,
                      int skipInterval,
                      formatType format);

                /**
                 * @brief Constructor for equi-distant x-values
                 */
                DLREP(double xMin,
                      double xMax,
                      double intSize,
                      double error,
                      double preFirst,
                      std::string name,
                      std::string description,
                      bool forceRMinusAOk,
                      int maxNrv,
                      int skipInterval,
                      formatType format);

                /**
                 * @brief PyConfig Constructor
                 */
                DLREP(const wns::pyconfig::View& config);

                // Destructor
                ~DLREP();

                /** @brief Print output */
                virtual void
                print(std::ostream& aStreamRef = std::cout) const;

                /** @brief put new value to probe */
                void
                put(double value);

                /** @brief Result line */
                virtual void
                getResultLine(const int index, ResultLine& line) const;

                /** @brief Change max error */
                virtual void
                changeError(double newError);

            private:

                // run time control
                DLRE::Phase rtc();
            };

} // end namespace statistics
} // end namespace evaluation
} // end namespace wns
#endif  // WNS_EVALUATION_STATISTICS_DLREP_HPP
