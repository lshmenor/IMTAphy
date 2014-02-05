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

#ifndef WNS_EVALUATION_STATISTICS_DLREF_HPP
#define WNS_EVALUATION_STATISTICS_DLREF_HPP

#include <WNS/evaluation/statistics/dlre.hpp>

namespace wns { namespace evaluation { namespace statistics {
            /**
             * @brief Class DLREF: Discrete LRE (LRE III) for distribution function
             */
            class DLREF : public DLRE
            {
            public:

                /**
                 * @brief Constructor for non-equi-distant x-values
                 */
                DLREF(std::vector<double> xValues,
                      int level,
                      double error,
                      double preFirst,
                      std::string name,
                      std::string description,
                      bool forceRMinusAOk,
                      double fMin,
                      int maxNrv,
                      int skipInterval,
                      formatType format);

                /**
                 * @brief Constructor for equi-distant x-values
                 */
                DLREF(double xMin,
                      double xMax,
                      double intSize,
                      double error,
                      double preFirst,
                      std::string name,
                      std::string description,
                      bool forceRMinusAOk,
                      double fMin,
                      int maxNrv,
                      int skipInterval,
                      formatType format);

                DLREF(const wns::pyconfig::View& config);

                ~DLREF();

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
                 * @brief Return current f-level
                 */
                virtual double
                curFLev();

                /**
                 * @brief Return the f-level of x(t)
                 */
                virtual double
                f(double xt);

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
                DLRE::Phase
                rtc();

                // minimum f level
                double fMin_;
            };
        }
    }
}

#endif  // WNS_EVALUATION_STATISTICS_DLREF_HPP
