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

#ifndef WNS_EVALUATION_STATISTICS_DLRE_HPP
#define WNS_EVALUATION_STATISTICS_DLRE_HPP

#include <WNS/evaluation/statistics/stateval.hpp>

#include <cstdlib>
#include <climits>
#include <cfloat>
#include <iomanip>

namespace wns { namespace evaluation { namespace statistics {
            /**
             * @brief Class DLRE: Discrete LRE (LRE III), base class
             */
            class DLRE : public StatEval
            {
            public:

                /**
                 * @brief Constructor for non-equi-distant x-values
                 */
                 DLRE(std::vector<double> xValuesArr,
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
                DLRE(double xMin,
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

                DLRE(const wns::pyconfig::View& config);

                ~DLRE();

                /**
                 * @brief Class DLRE::resultLine: one line of evaluation results.
                 */
                class ResultLine
                {
                public:
                    ResultLine();

                    bool
                    operator != (const ResultLine& other) const;

                    /**
                     * @brief F, G or P value (depends on derived class)
                     */
                    double vf_;

                    double x_;

                    double relErr_;

                    /**
                     * @brief Local correlation
                     */
                    double rho_;

                    /**
                     * @brief Relative error of correlation
                     */
                    double sigRho_;

                    /**
                     * @brief Absolute number of hits of x
                     */
                    int nx_;
                };

                /**
                 * @brief Reset measurement results
                 */
                virtual void
                reset();

                /**
                 * @brief Return factor for cond. prob.
                 */
                void
                setBase(double newBase);

                /**
                 * @brief Change error
                 */
                virtual void
                changeError(double newError);

                /**
                 * @brief Return current x level to be completed
                 */
                virtual double
                curXLev();

                /**
                 * @brief Returns the prob. of xt
                 */
                virtual double
                p(double xt);

                /**
                 * @brief return the index of the minimum x value to
                 * be evaluated
                 */
                virtual int
                minIndex() const;

                /**
                 * @brief Return the index of the maximum x value to
                 * be evaluated
                 */
                virtual int
                maxIndex() const;

                /**
                 * @brief Put new value to probe
                 */
                virtual void
                put(double value) = 0;

                /**
                 * @brief Return result line
                 */
                virtual void
                getResultLine(const int index, ResultLine& line) const = 0;

                virtual void
                print(std::ostream& stream = std::cout) const = 0;

            protected:
                enum Phase 
                {
                    initialize,
                    iterate,
                    finish
                }
                phase_;

                /**
                 * @brief Class DLRE::Result: results for one x interval
                 */
                class Result
                {
                public:
                    Result();

                    bool operator != (const Result& other) const;

                    /**
                     * @brief x value for this interval
                     */
                    double x_;

                    /**
                     * @brief Number of samples for this interval
                     */
                    int h_;

                    /**
                     * @brief Sum of samples for all intervals left/right
                     * of this one
                     */
                    int sumh_;

                    /**
                     * @brief Number of transitions for this interval
                     */
                    int c_;
                };

                /**
                 * @brief get index of current x
                 */
                int
                getIndex(double value) const;

                /**
                 * @brief Output function-specific evaluation results
                 */
                void
                printAll(std::ostream& stream,
                         functionType functionType,
                         const double yMin) const;

                /**
                 * @brief Large sample conditions are fulfilled?
                 */
                bool checkLargeSample(int index) const;

                /**
                 * @brief Comparison result returned by getIndex()
                 */
                enum ComparisonResult
                {
                    noIndex = -3,
                    greater = -2,
                    lower = -1
                };

                /**
                 * @brief reason for status = finish
                 */
                enum ReasonType
                {
                    ok,
                    minimum,
                    last
                };

                /**
                 * @brief Array of results
                 */
                Result* results_;

                double relErrMax_;

                /**
                 * @brief Maximal number of trials
                 */
                int maxNrv_;

                /**
                 * @brief Values lower minimum
                 */
                int wastedLeft_;

                /**
                 * @brief Values greater maximum
                 */
                int wastedRight_;

                /**
                 * @brief Run time control (rtc) counter
                 */
                int h_;

                /**
                 * @brief Offset of x-values (step 1)
                 */
                int xOffset_;

                double xMin_;

                double xMax_;

                int indexMin_;

                int indexMax_;

                /**
                 * @brief Equi-distant x values or user defined?
                 */
                bool equiDist_;

                /**
                 * @brief distance between x values
                 */
                double intSize_;

                /**
                 * @brief Index of current x-value
                 */
                int curIndex_;

                /**
                 * @brief predecessor
                 */
                double preRv_;

                /**
                 * @brief Index of predecessor
                 */
                int preIndex_;

                /**
                 * @brief Needed for conditional probabilties
                 */
                double base_;

                /**
                 * @brief Reason for end reached
                 */
                ReasonType reason_;

                /**
                 * @brief Index of current x-level
                 */
                int curLevelIndex_;

                /**
                 * @brief Number of samples after which simulation is checked for
                 * completion
                 */
                int skipInterval_;

                /**
                 * @brief force last large sample criterion (r - av >= 10)
                 */
                bool forceRminusAOK_;

                /**
                 * @brief Number of trials for large sample conditions
                 */
                static const int largeSampleNumTrials_;

                /**
                 * @brief number of sorted values for large sample conditions
                 */
                static const int largeSampleNumSortedValues_;

                /**
                 * @brief Number of transitions for large sample conditions
                 */
                static const int largeSampleNumTransitions_;

            private:
                /**
                 * @brief Initialization with an array of x-values for the bins
                 */
                void initNonEqui(int level,
                                 std::vector<double> xValuesArr,
                                 double preFirst);
                /**
                 * @brief Initialization which creates equidistant x-values for
                 * the bins, which have a width of 'intSize' between 'xMin'
                 * and 'xMax'
                 */
                void initEqui(double xMin,
                              double xMax,
                              double intSize,
                              double preFirst);


                void printLevel(std::ostream& stream,
                                int level,
                                const std::string& errorString,
                                bool discretePointFlag,
                                functionType functionType) const;
            };
        } // statistics
    } // evaluation
} // wns

#endif  // WNS_EVALUATION_STATISTICS_DLRE_HPP
