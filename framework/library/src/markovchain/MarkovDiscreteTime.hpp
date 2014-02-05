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
#include <WNS/events/CanTimeout.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/module/Base.hpp>

#include <WNS/distribution/Uniform.hpp>

#include <boost/numeric/ublas/lu.hpp>

#ifndef WNS_MARKOVCHAIN_MARKOVDISCRETETIME_HPP
#define WNS_MARKOVCHAIN_MARKOVDISCRETETIME_HPP


namespace wns { namespace markovchain {

	/**
	 * @brief Class to represent a Markov Discrete Time Process.
	 * @author Sara Gutierrez de Mesa(sgm@comnets.rwth-aachen.de)
	 */

	template<class T>
	class MarkovDiscreteTime :
		public wns::events::CanTimeout,
		public MarkovBase<T>
	{
	public:
		/**
		 * @brief Creates an empty MarkovDiscreteTime.
		 * parameter numberOfChains is required.
		 */
		MarkovDiscreteTime(int _numberOfChains):
			wns::events::CanTimeout(),
			MarkovBase<T>(_numberOfChains),
			slotTime(1.0),
			startTime(0.0),
			stopTime(0.0)
		{
			MESSAGE_SINGLE(VERBOSE,MarkovBase<T>::logger, "MarkovDiscreteTime() called");
		} // constructor

		/**
		 * @brief Creates a complete MarkovDiscreteTime
		 */
		MarkovDiscreteTime(int _numberOfStates,
				   int _numberOfChains,
				   std::vector<T> states,
				   TransitionMatrixType matrix,
				   std::vector<int> startStates,
				   simTimeType startTime,
				   simTimeType slotTime,
				   simTimeType stopTime):
			wns::events::CanTimeout(),
			MarkovBase<T>(_numberOfStates, _numberOfChains, states, matrix, startStates),
			slotTime(slotTime),
			startTime(startTime),
			stopTime(stopTime)
		{
		} // constructor

		/**
		 * @brief destructor
		 */
	        ~MarkovDiscreteTime()
		{
		}

		/**
		 * @brief setSlotTime
		 */
		void
		setSlotTime(simTimeType _slotTime)
		{
			slotTime = _slotTime;
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
		 *  for the chain chainNumber
		 */
		double
		calculateNextState(int chainNumber)
		{
			int currentState = MarkovBase<T>::actualStates[chainNumber];
			int nextState;
			wns::distribution::StandardUniform* randomDist;

			// generate a random value
			randomDist = new wns::distribution::StandardUniform();
			double random = (*randomDist)();

			nextState = currentState;
			double probSum = 0.0;
			for (int st = 0; st < MarkovBase<T>::numberOfStates; st++){
				double prob = MarkovBase<T>::transitionsMatrix(currentState, st);
				probSum += prob;
				if (random <= probSum) {
					nextState = st;
					break;
				}
			} // forall (possible next) states
			if (currentState != nextState) {
				MESSAGE_BEGIN(NORMAL, MarkovBase<T>::logger, m, "NextState(chain=" << chainNumber << "): ");
				m << "scheduled transition " << currentState << " -> " << nextState << " after " << slotTime << "s";
				MESSAGE_END();
				MarkovBase<T>::nextStates[chainNumber] = nextState;
			}

			delete randomDist;
			return slotTime;
		} // calculateNextState

		/**
		 * @brief Checks if the matrix of probabilities given is correct.
		 */
		bool
		checkSum()
		{
			for (int i = 0;i < MarkovBase<T>::numberOfStates; i++) {
				double checkSum = 0.0;
				for (int j = 0; j < MarkovBase<T>::numberOfStates; j++)
					checkSum += MarkovBase<T>::transitionsMatrix(i, j);
				assure(fabs(checkSum - 1.0) < 1e-3, "wrong matrix row sum");
				if (fabs(checkSum - 1.0) >= 1e-3) {
					MESSAGE_SINGLE(NORMAL,MarkovBase<T>::logger, "wrong matrix row sum");
					return false;
				}
			} // for i
			return true;
		} // checkSum()


		/**
		 * @brief: first checks if the matrix is ok, then
		 * Sets the first Timeout
		 */
		void
		startEvents()
		{
			checkSum();
			MESSAGE_SINGLE(NORMAL,MarkovBase<T>::logger,
				       "startEvents(): startTime=" << startTime <<
				       ", stopTime=" << stopTime );

			wns::distribution::StandardUniform* randomDist = 
                new wns::distribution::StandardUniform();

			for (int chainNumber = 0; chainNumber<MarkovBase<T>::numberOfChains; chainNumber++) {
				int startState = MarkovBase<T>::startStates[chainNumber];
				assure((startState >= -1) && (startState < MarkovBase<T>::numberOfStates), "wrong start state");
				if (startState < 0) { // random (not simple!)
					startState = 0;
					calculateStateProbabilities(); // only calculated once even if called up to C times here
					double random = (*randomDist)();
					double probSum = 0.0;
					MESSAGE_SINGLE(NORMAL,MarkovBase<T>::logger, "determining random start state (r="<<random<<")");
					int nos = MarkovBase<T>::numberOfStates;
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
				MESSAGE_BEGIN(NORMAL, MarkovBase<T>::logger, m, "startEvents(chain=" << chainNumber<<"):");
				m << " startState=" << startState;
				MESSAGE_END();

				calculateNextState(chainNumber);

				// this notifies the method of the derived class:
				stateChangeNotification(chainNumber);
			} // forall chains
			setTimeout(startTime + slotTime); // set the timer to a relative time
			/* startTime>0 can be a problem if the current time is already >0 */
			delete randomDist;
		} // startEvents()

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
		} // stateChangeNotification()

		/**
		 * @brief stop the process (for all chains). No more state changes will happen.
		 */
		virtual void
		stopMarkovProcess()
		{
			MESSAGE_SINGLE(NORMAL,MarkovBase<T>::logger, "MarkovProcess stopped.");
			if (hasTimeoutSet()) cancelTimeout();
			// remove state transition events (from CanTimeout.hpp)
		} // stopMarkovProcess()

	private:

		friend class MarkovDiscreteTimeTest;

	protected:

		/**
		 * @brief determine the probability of each state.
		 *
		 * Discrete Time Markov Chain:
		 * In theory, these equations are solved for vectorN:
		 * 1) vectorN = vectorN * matrixT
		 *   <=>  0 = vectorN * (matrixT - I)
		 * 2) vectorN * vector1 = 1
		 * This is performed by doing a LU decomposition.
		 */
		virtual void
		calculateStateProbabilities()
		{
			if (MarkovBase<T>::stateProbabilities[0]>=0.0) { return; }
			// ^ already calculated

			int nos = MarkovBase<T>::numberOfStates;
			MESSAGE_BEGIN(VERBOSE, MarkovBase<T>::logger, m, "transitionMatrix=\n");
			m << MarkovBase<T>::transitionsMatrix;
			MESSAGE_END();

            TransitionMatrixType Ptrans = boost::numeric::ublas::trans(MarkovBase<T>::transitionsMatrix);

            MESSAGE_BEGIN(VERBOSE, MarkovBase<T>::logger, m, "System matrix P^T=\n");
            m << Ptrans << "\n";
            MESSAGE_END();

            TransitionMatrixType ident = boost::numeric::ublas::identity_matrix<double>(nos, nos);
            TransitionMatrixType PtransMinus = TransitionMatrixType(Ptrans - ident);

            MESSAGE_BEGIN(VERBOSE, MarkovBase<T>::logger, m, "Matrix (P^T - I)=\n");
            m << PtransMinus << "\n";
            MESSAGE_END();

            // The "Replace an Equation" Approach (see W.J. Stewart Numeric Solution of Markov Chains 2.3.1.2)

            boost::numeric::ublas::permutation_matrix<std::size_t> pm(PtransMinus.size1());
            boost::numeric::ublas::vector<double> x(nos);

            x = boost::numeric::ublas::zero_vector<double>(nos);
            x(nos - 1) = 1.0;

            // Fill the last row of PtransMinus with 1.0 => PtransMinus1
            for(int col = 0; col < nos; col++)
                PtransMinus(nos - 1, col) = 1.0;

            // Solve Qtrans1*result = x
            boost::numeric::ublas::lu_factorize(PtransMinus, pm);
            boost::numeric::ublas::lu_substitute(PtransMinus, pm, x);
            // Result is in x

            for(int i = 0; i < nos; i++)
                MarkovBase<T>::stateProbabilities[i] = x(i);

			MESSAGE_BEGIN(NORMAL, MarkovBase<T>::logger, m, "state probabilities: ");
			for(int i=0; i<nos; i++) {
				double p = MarkovBase<T>::stateProbabilities[i];
				m << p << " ";
			}
			MESSAGE_END();
		}

		/**
		 * @brief: Slot Time
		 */
		simTimeType slotTime;

		/**
		 * @brief: Start Time
		 */
		simTimeType startTime;

		/**
		 * @brief: Stop Time
		 */
		simTimeType stopTime;

		/**
		 * @brief onTimeout:changes to the new state, sets the next Timeout
		 */
		virtual void
		onTimeout ()
		{
			for (int chainNumber = 0; chainNumber < MarkovBase<T>::numberOfChains; chainNumber++){
				if (MarkovBase<T>::actualStates[chainNumber] != MarkovBase<T>::nextStates[chainNumber]){
					MESSAGE_BEGIN(NORMAL, MarkovBase<T>::logger, m, "onTimeout(chain "<<chainNumber<<"): ");
					m << "state change " << MarkovBase<T>::actualStates[chainNumber]
					  << " -> "         << MarkovBase<T>::nextStates[chainNumber];
					MESSAGE_END();

					MarkovBase<T>::actualStates[chainNumber] = MarkovBase<T>::nextStates[chainNumber];
					stateChangeNotification(chainNumber);
				} else { // no state change
					MESSAGE_SINGLE(VERBOSE,MarkovBase<T>::logger,  "onTimeout(chain "<<chainNumber<<"): stays in the state "<<MarkovBase<T>::nextStates[chainNumber]<< " ");
				}
				calculateNextState(chainNumber);
			} // forall chains

			if ((stopTime == 0.0) || (wns::simulator::getEventScheduler()->getTime()+slotTime<stopTime)) {
				setTimeout(slotTime); // relative
			} // else: this was last event. Stop now
		}
	};// MarkovDiscreteTime

} }
#endif // WNS_MARKOVCHAIN_MARKOVDISCRETETIME_HPP


/**
 * @file
 */


