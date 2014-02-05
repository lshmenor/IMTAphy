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

#include <WNS/markovchain/MarkovBase.hpp>
#include <WNS/events/MultipleTimeout.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/distribution/ForwardRecurrenceTime.hpp>
#include <WNS/distribution/Distribution.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <WNS/distribution/Uniform.hpp>
#include <WNS/container/Matrix.hpp>

#include <boost/numeric/ublas/lu.hpp>

#ifndef WNS_MARKOVCHAIN_MARKOVCONTINUOUSTIME_HPP
#define WNS_MARKOVCHAIN_MARKOVCONTINUOUSTIME_HPP


namespace wns { namespace markovchain {

	/**
	 * @brief Class to represent a Markov Continuous Time Process.
	 * @author Sara Gutierrez de Mesa (sgm@comnets.rwth-aachen.de)
	 * @author Rainer Schoenen (rs@comnets.rwth-aachen.de)
	 */
	template<class T>
	class MarkovContinuousTime :
		public wns::events::MultipleTimeout<int>, // int for the chain number
		public MarkovBase<T>
	{
	public:
		/**
		 * @brief Creates an empty MarkovContinuousTime.
		 * the numberOfChains is required as an argument.
		 */
		MarkovContinuousTime(int _numberOfChains) :
			wns::events::MultipleTimeout<int>(),
			MarkovBase<T>(_numberOfChains),
			transitionScale(1.0),
			startTime(0.0),
			stopTime(0.0), // infinite
			transDistSpec("NegExp(X)")
		{
			MESSAGE_SINGLE(VERBOSE, MarkovBase<T>::logger, "MarkovContinuousTime(C=" << _numberOfChains << ") called");
		}

		/**
		 * @brief Creates a complete MarkovContinuousTime
		 * (full specification given)
		 */
		MarkovContinuousTime(int _numberOfStates,
				     int _numberOfChains,
				     std::vector<T> states,
				     TransitionMatrixType matrix,
				     std::vector<int> startStates,
				     simTimeType startTime,
				     simTimeType stopTime) :
			wns::events::MultipleTimeout<int>(),
			MarkovBase<T>(_numberOfStates, _numberOfChains, states, matrix, startStates),
			transitionScale(1.0),
			startTime(startTime),
			stopTime(stopTime),
			transDistSpec("NegExp(X)")
		{
			transitionScale = 1.0;
			MESSAGE_SINGLE(VERBOSE, MarkovBase<T>::logger, "MarkovContinuousTime(C=" << _numberOfChains << ",...) called");
		}

		/**
		 * @brief destructor MarkovcontinuousTime
		 */
		~MarkovContinuousTime()
		{
			MESSAGE_SINGLE(VERBOSE,MarkovBase<T>::logger, "~MarkovContinuousTime() called");
		}

		/**
		 * @brief setStartTime
		 */
		void
		setStartTime(simTimeType _startTime)
		{
			startTime = _startTime;
		}

		/**
		 * @brief setStopTime
		 */
		void
		setStopTime(simTimeType _stopTime)
		{
			stopTime = _stopTime;
		}

		/**
		 * @brief stringify: converts a double to a string
		 */
		std::string
		stringify(double x) const
		{
			std::ostringstream o;
			o << x;
			return o.str();
		}

		/**
		 * @brief Calculates the next state and the next transition time
		 * for the chain chainNumber
		 */
		double
		calculateNextState(int chainNumber)
		{
			int currentState = MarkovBase<T>::actualStates[chainNumber];
			int nextState;
			double absTransTime;
			double rTime, dTime;

			//MESSAGE_SINGLE(NORMAL, MarkovBase<T>::logger, "calculateNextState(chainNumber=" << chainNumber << "):");
			dTime = 1.0e+100; // for finding the minimum
			nextState = currentState;
			for (int st = 0; st < MarkovBase<T>::numberOfStates; st++){
				if (st != currentState) {
					if (matrixDistribution[currentState][st]) {
						rTime = (*matrixDistribution[currentState][st])(); // random value
						if (rTime < dTime) {
							dTime = rTime;
							nextState = st;
						}
					}
				}
			}
			dTime /= transitionScale;

			MESSAGE_BEGIN(NORMAL, MarkovBase<T>::logger, m, "NextState(chain=" << chainNumber << "): ");
			m << "scheduled transition " << currentState << " -> " << nextState << " after " << dTime << "s";
			MESSAGE_END();

			simTimeType now = wns::simulator::getEventScheduler()->getTime(); // for absolute time
			absTransTime = now + dTime;
			MarkovBase<T>::nextTransitionTime[chainNumber] = absTransTime; // only for information

			if ((stopTime == 0.0) || (absTransTime < stopTime)) {
				MarkovBase<T>::nextStates[chainNumber] = nextState;
			} else {
				MarkovBase<T>::nextStates[chainNumber] = -1; // end the Markov process
				dTime = stopTime - now; // trans time set to end time
			}
			//MESSAGE_SINGLE(NORMAL, log, "stateChangeNotification("<<chainNumber<<") to state "<<currentState);
			// this notifies the method of the derived class:
			stateChangeNotification(chainNumber);
			return dTime; // relative to now
		} // NextState


		/**
		 * @brief set a Python compatible specification string
		 * for the random number distributions
		 * used for flexible transition statistics.
		 *
		 * Input: transitionDistributionSpec containing "X" as a placeholder
		 * e.g.
		 * "Pareto(1.2,X,0.0,1e100)"
		 * "Uniform(1.5*X,0.5*X)"
		 * "Norm(X,X*X)"
		 */
		void
		setTransitionDistributionSpec(std::string distSpec)
		{
			assure(MarkovBase<T>::numberOfStates > 1, "Transition Distribution cannot be set at this stage");
			transDistSpec = distSpec;
			assure(distSpec.find("X",0),"distSpec must contain at least one X");
			// a call to prepareMatrixOfDistributions() must come after this
		}

                /**
		 * @brief: fills in the matrix of distributions with generators
		 * for random values
		 */
		void
		prepareMatrixOfDistributions()
		{
			double lambda;
			MESSAGE_SINGLE(VERBOSE,MarkovBase<T>::logger, "prepareMatrixOfDistributions(): transDistSpec=" << transDistSpec);
			assure(MarkovBase<T>::numberOfStates > 0, "numberOfStates not set");
			const MatrixDistType::SizeType sizes[2] = {MarkovBase<T>::numberOfStates, MarkovBase<T>::numberOfStates};
			matrixDistribution = MatrixDistType(sizes);
			for (int i = 0; i < MarkovBase<T>::numberOfStates; i++) { // row
				double lambdaRowSum = 0.0; // row sum
				for (int j = 0; j < MarkovBase<T>::numberOfStates; j++) { // column
					if (i == j){ // value on diagonal of matrix
						matrixDistribution[i][j] = NULL;
					} else {
						lambda = MarkovBase<T>::transitionsMatrix(i, j);
						if (lambda != 0.0) {
							lambdaRowSum += lambda;
							wns::pyconfig::Parser config;
							config.loadString(
								"import openwns.distribution\n"
								"X = " + stringify(1.0/lambda) + "\n"
								"outcome = openwns.distribution." + transDistSpec + "\n"
								);
							wns::pyconfig::View distConfig(config, "outcome");
							std::string distributionName = distConfig.get<std::string>("__plugin__");
							wns::distribution::Distribution* distribution =
								wns::distribution::DistributionFactory::creator(distributionName)->create(distConfig);
							matrixDistribution[i][j] = distribution;
							// Opnet: LoadDistribution(dist_code, lambda, pdf_args);
							MESSAGE_BEGIN(VERBOSE, MarkovBase<T>::logger, m, "prepare(): ");
							m << "lambda[" << i << "][" << j <<"] = "<< lambda;
							m << ", dist=" << *distribution;
							MESSAGE_END();
						} else {
							matrixDistribution[i][j] = NULL;
						}
					} // off-diagonal
				} // for j
				MarkovBase<T>::transitionsMatrix(i, i) = - lambdaRowSum; // diagonal
			} // for i
		} //prepareMatrixOfDistributions


		/**
		 * @brief: first prepares the matrix of distributions, then
		 * initializates all the structures needed.
		 * Sets the first Timeout
		 */
		void
		startEvents()
		{
			prepareMatrixOfDistributions();

			MESSAGE_SINGLE(NORMAL,MarkovBase<T>::logger, "startEvents(): startTime=" << startTime <<", stopTime=" << stopTime );

			wns::distribution::StandardUniform* randomDist = 
                new wns::distribution::StandardUniform();

			for (int chainNumber = 0; chainNumber<MarkovBase<T>::numberOfChains; chainNumber++){
				int startState = MarkovBase<T>::startStates[chainNumber];
				assure((startState >= -1) && (startState < MarkovBase<T>::numberOfStates), "wrong start state");
				if (startState < 0) { // random (not simple!)
					startState = 0;
					calculateStateProbabilities(); // only calculated once even if called up to C times here
					int nos = MarkovBase<T>::numberOfStates;
					double random = (*randomDist)();
					double probSum = 0.0;
					MESSAGE_SINGLE(NORMAL,MarkovBase<T>::logger, "determining random start state (r="<<random<<")");
					for(int i=0; i<nos; i++) {
						double p=MarkovBase<T>::stateProbabilities[i];
						probSum += p;
						if (random <= probSum) {
							startState = i; break;
						}
					}
				}
				MarkovBase<T>::actualStates[chainNumber] = startState;
				MarkovBase<T>::nextStates[chainNumber]   = startState;
				MarkovBase<T>::nextTransitionTime[chainNumber] = 0.0;
				MESSAGE_BEGIN(NORMAL, MarkovBase<T>::logger, m, "startEvents(chain=" << chainNumber<<"):");
				m << " startState=" << startState;
				MESSAGE_END();
				double myNextEventTimeOffset  = calculateNextState(chainNumber);
				double myFirstEventTimeOffset = wns::distribution::forwardRecurrenceTime(myNextEventTimeOffset);
				MESSAGE_SINGLE(VERBOSE,MarkovBase<T>::logger, "myFirstEventTimeOffset = "<< myFirstEventTimeOffset);
				setTimeout(chainNumber,startTime + myFirstEventTimeOffset); // set the timer to a relative time
				/* startTime>0 can be a problem if the current time is already >0 */
			} // for
			delete randomDist;
		}

		/**
		 * @brief: stateChangeNotification
		 */
		virtual void
		stateChangeNotification(const int &chainNumber)
		{
#ifndef WNS_NO_LOGGING
			int newState =
#endif
				MarkovBase<T>::actualStates[chainNumber];
			MESSAGE_SINGLE(NORMAL, MarkovBase<T>::logger, "MarkovContinuousTime::stateChange(" << chainNumber << ") to state " << newState);
		}

		/**
		 * @brief determine the probability of each state.
		 *
		 * Continuous Time Markov Chain:
		 * In theory, these equations are solved for vectorN:
		 * 1) vectorN * matrixT = 0 (you guess right, matrixT has one linear dependent row)
		 * 2) vectorN * vector1 = 1
		 * This is performed by doing a singular value decomposition (SVD).
		 * Note that the result is only valid if the transition
		 * distribution is NegExp (Markov property).
		 * In some other cases and if p[state] is equal for all states,
		 * the results are ok, too. Especially the meanRate is Ok then.
		 */
		virtual void
		calculateStateProbabilities()
		{
			if (MarkovBase<T>::stateProbabilities[0]>=0.0) { return; }
			// ^ already calculated

			int nos = MarkovBase<T>::numberOfStates;
			for(int row=0; row<nos; row++) {
				double lambdaRowSum=0.0;
				for(int col=0; col<nos; col++)
				    if (col != row)
					    lambdaRowSum += MarkovBase<T>::transitionsMatrix(row, col);
				MarkovBase<T>::transitionsMatrix(row, row) = - lambdaRowSum; // diagonal
			}
			MESSAGE_BEGIN(VERBOSE, MarkovBase<T>::logger, m, "transitionMatrix=\n");
			m << MarkovBase<T>::transitionsMatrix;
			MESSAGE_END();
			
            TransitionMatrixType Qtrans = boost::numeric::ublas::trans(MarkovBase<T>::transitionsMatrix);

			MESSAGE_BEGIN(VERBOSE, MarkovBase<T>::logger, m, "System matrix Q^T=\n");
			m << Qtrans << "\n";
			MESSAGE_END();

            // The "Replace an Equation" Approach (see W.J. Stewart Numeric Solution of Markov Chains 2.3.1.2)

            boost::numeric::ublas::permutation_matrix<std::size_t> pm(Qtrans.size1());
            boost::numeric::ublas::vector<double> x(nos);

            x = boost::numeric::ublas::zero_vector<double>(nos);
            x(nos - 1) = 1.0;

            // Fill the last row of Qtrans with 1.0 => Qtrans1
            for(int col = 0; col < nos; col++)
                Qtrans(nos - 1, col) = 1.0;

            // Solve Qtrans1*result = x
            boost::numeric::ublas::lu_factorize(Qtrans, pm);
            boost::numeric::ublas::lu_substitute(Qtrans, pm, x);
            // Result is in x

            for(int i = 0; i < nos; i++)
                MarkovBase<T>::stateProbabilities[i] = x(i);    

			MESSAGE_BEGIN(NORMAL, MarkovBase<T>::logger, m, "state probabilities: ");
			for(int i=0; i<nos; i++) {
				double p=MarkovBase<T>::stateProbabilities[i];
				m << p << " ";
			}
			MESSAGE_END();
		}

	protected:

		/**
		 * @brief type for the matrix of distributions.
		 */
		typedef	wns::container::Matrix<wns::distribution::Distribution*, 2> MatrixDistType;

		/**
		 * @brief matrix of distributions
		 */
		MatrixDistType matrixDistribution;

		/**
		 * @brief transition scale:
		 * value to tune up (>1) or down (<1) the rate of state changes
		 */
		double transitionScale;

		/**
		 * @brief start time
		 */
		simTimeType startTime; // all chains start at this time

		/**
		 * @brief stop time
		 */
		simTimeType stopTime; // all chains stop at this time

		/**
		 * @brief: flexible specification of the transition distribution
		 */
		std::string transDistSpec; // typically "NegExp(X)"

		/**
		 * @brief onTimeout: changes to the new state, sets the next timeout
		 */
		virtual void
		onTimeout (const int &chainNumber)
		{
			double myNextEventTimeOffset;
			if (MarkovBase<T>::nextStates[chainNumber] < 0) {
				MESSAGE_SINGLE(NORMAL,MarkovBase<T>::logger, "onTimeout(chain "<<chainNumber<<"): Markov process ends.");
			} else {
				MESSAGE_BEGIN(NORMAL, MarkovBase<T>::logger, m, "onTimeout(chain="<<chainNumber<<"): ");
				m << "state change " << MarkovBase<T>::actualStates[chainNumber]
				  << " -> "         << MarkovBase<T>::nextStates[chainNumber];
				MESSAGE_END();

				MarkovBase<T>::actualStates[chainNumber] = MarkovBase<T>::nextStates[chainNumber];
				myNextEventTimeOffset = calculateNextState(chainNumber);
				setTimeout(chainNumber,myNextEventTimeOffset);

				MESSAGE_BEGIN(VERBOSE, MarkovBase<T>::logger, m, "onTimeout(chain="<<chainNumber<<"): ");
				m << "Next state change will occur at t="
				  << wns::simulator::getEventScheduler()->getTime() + myNextEventTimeOffset;
				MESSAGE_END();
			}
		} // onTimeout

		/**
		 * @brief stop the process (for all chains). No more state changes will happen.
		 */
		virtual void
		stopMarkovProcess()
		{
			MESSAGE_SINGLE(NORMAL,MarkovBase<T>::logger, "MarkovProcess stopped.");
			cancelAllTimeouts(); // remove state transition events (from MultipleTimeout.hpp)
		}
	private:
		friend class MarkovContinuousTimeTest;

	}; // MarkovContinuousTime

} }
#endif // WNS_MARKOVCHAIN_MARKOVCONTINUOUSTIME_HPP


/**
 * @file
 */


