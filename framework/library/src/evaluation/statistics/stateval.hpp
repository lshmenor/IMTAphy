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

#ifndef WNS_EVALUATION_STATISTICS_STATEVAL_HPP
#define WNS_EVALUATION_STATISTICS_STATEVAL_HPP

#include <WNS/PyConfigViewCreator.hpp>

namespace wns { namespace evaluation { namespace statistics {
            template <typename T>
            T getMaxError();

            template <>
            inline
            double getMaxError<double>()
            {
                return 0.000000001;
            }

            class StatEvalInterface
            {
            public:
                virtual
                ~StatEvalInterface()
                {
                }

                virtual void
                put(double) = 0;

                virtual void
                reset() = 0;

                virtual const std::string&
                getName() const = 0;

                virtual void
                print(std::ostream&) const = 0;

                /**
                 * @todo dbn: This was introduced to handle LogEval in some way. LogEvals flush
                 * their contents completely to disk and thus cannot be const. We should think
                 * of a generic way to handle both possibilities behind a common interface.
                 */
                virtual void
                printLog(std::ostream&) = 0;
            };

            /**
             * @brief Class StatEval: Base class for statistical evaluation
             */
            class StatEval : public StatEvalInterface
            {
            public:
                /**
                 * @brief Type of evaluation
                 */
                enum functionType
                {
                    df,
                    cdf,
                    pf
                };


                /**
                 * @brief How to format the output of numbers
                 */
                enum formatType
                {
                    fixed,
                    scientific
                };


                StatEval(formatType  format,
                         std::string name,
                         std::string desc);

                StatEval(const wns::pyconfig::View& config);

                virtual ~StatEval();

                /**
                 * @brief Print output
                 */
                virtual
                void print(std::ostream& stream) const;

                virtual void
                printLog(std::ostream& stream);

                /**
                 * @brief Type of statistical evaluation
                 */
                enum statEvalType
                {
                    all = 0,
                    lref, lreg, plref, plreg,
                    moments, pmoments,
                    batchMns, pbatchMns,
                    histogrm, phistogrm,
                    dlref, dlreg, dlrep, pdlref, pdlreg, pdlrep,
                    dlref_nonequi, dlreg_nonequi, dlrep_nonequi,
                    pdlref_nonequi, pdlreg_nonequi, pdlrep_nonequi,
                    pdf, ppdf,
                    logeval, plogeval,
                    probetext, pprobetext, unknown
                };

                /**
                 * @brief put a value to evaluation
                 */
                virtual void
                put(double xI);

                /**
                 * @brief Return mean value
                 */
                virtual double
                mean() const;

                /**
                 * @brief Return variance
                 */
                virtual double
                variance() const;

                /**
                 * @brief Relative variance
                 */
                virtual double
                relativeVariance() const;

                /**
                 * @brief Return coefficient of variation
                 */
                virtual double
                coeffOfVariation() const;

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
                 * @brief Return z3
                 */
                virtual double
                Z3() const;

                /**
                 * @brief Return skewness
                 */
                virtual double
                skewness() const;

                /**
                 * @brief Return standard deviation
                 */
                virtual double
                deviation() const;

                /**
                 * @brief Return relative standard deviation
                 */
                virtual double
                relativeDeviation() const;

                /**
                 * @brief Return number of trials
                 */
                virtual unsigned long int
                trials() const;

                /**
                 * @brief minimal value of trials
                 */
                virtual double
                min() const;

                /**
                 * @brief maximal value of trials
                 */
                virtual double
                max() const;

                /**
                 * @brief Reset evaluation
                 */
                virtual void
                reset();

                /**
                 * @brief Get name of
                 */
                virtual const
                std::string& getName() const;

                /**
                 * @brief Get description
                 */
                virtual const std::string&
                getDesc() const;

                /**
                 * @brief Set the output format: fixed or scientific.
                 */
                virtual void
                setFormat(formatType format);

                /**
                 * @brief Get the output format: fixed or scientific.
                 */
                virtual formatType
                getFormat() const;

                /**
                 * @brief map string to statEvalType
                 */
                static statEvalType
                mapToStatEvalType(std::string type);


                /**
                 * @brief map string to statEvalType
                 */
                static std::string
                mapEvalTypeToString(statEvalType);

            protected:

                /**
                 * @ brief Print the banner containing common statistics
                 */
                void
                printBanner(std::ostream& stream    ,
                            std::string probeTypeDesc,
                            std::string errorString) const;

                /**
                 * @brief Minimum value of all collected trials
                 */
                double minValue_;

                /**
                 * @brief Maximum value of all collected trials
                 */
                double maxValue_;

                /**
                 * @brief number of collected trials
                 */
                unsigned long int numTrials_;

                /**
                 * @brief Sum of collected values
                 */
                double sum_;

                /**
                 * @brief Square sum of collected values
                 */
                double squareSum_;

                /**
                 * @brief Cube sum of collected values
                 */
                double cubeSum_;

                /**
                 * @brief Format of numbers (fixed, scientific)
                 */
                StatEval::formatType format_;

                /**
                 * @brief Name
                 */
                std::string name_;

                /**
                 * @brief Description
                 */
                std::string desc_;

                /**
                 * @brief Comment prefix to be used in output files
                 */
                std::string prefix_;

                /**
                 * @brief Scaling factor for incoming values
                 */
                double scalingFactor_;
            };


            typedef wns::PyConfigViewCreator<StatEvalInterface, StatEvalInterface> Creator;
            typedef wns::StaticFactory<Creator> Factory;

} // statistics
} //evaluation
} // wns

#endif  // WNS_EVALUATION_STATISTICS_STATEVAL_HPP
