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
#ifndef WNS_MARKOVCHAIN_MARKOVBASE_HPP
#define WNS_MARKOVCHAIN_MARKOVBASE_HPP

#include <WNS/logger/Logger.hpp>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace wns {  namespace markovchain {

	/**
	 * @brief type for the matrix of transitions
	 */
	typedef boost::numeric::ublas::matrix<double> TransitionMatrixType;

	/**
         * @brief Base Class to represent a Markov Process.
         * @author Sara Gutierrez de Mesa (sgm@comnets.rwth-aachen.de)
	 * @author Rainer Schoenen (rs@comnets.rwth-aachen.de)
	 *
	 * Functions and attributes that have in common
	 * MarkovContinuousTime and MarkovDiscreteTime.
	 * both classes derive from it
         */
	template<class T>
        class MarkovBase
        {
	public:

                friend std::ostream&
                operator <<(std::ostream& os, const MarkovBase& markov)
                {
                        os << "TransitionMatrix=";
                        os << markov.transitionsMatrix;
                        return os;
                }

		/**
		 * @brief Creates an empty MarkovBase.
		 * numberOfChains is needed.
		 */

		MarkovBase(int _numberOfChains) :
			numberOfStates(0),
			numberOfChains(_numberOfChains),
			startStates(std::vector<int>(numberOfChains,0)),
			actualStates(std::vector<int>(numberOfChains)),
			nextStates(std::vector<int>(numberOfChains)),
			nextTransitionTime(std::vector<double>(numberOfChains,0.0)),
			logger(std::string("WNS"), std::string("MarkovModel"))
		{
			for (int chainNumber = 0; chainNumber<numberOfChains; chainNumber++) {
				startStates[chainNumber]=0; // initial value
			}
			actualStates = startStates;
			MESSAGE_SINGLE(VERBOSE, logger, "MarkovBase(C=" <<_numberOfChains<< ") called");
		}

		/**
		 * @brief Creates a Complete  MarkovBase.
		 *
		 * NumberOfStates: nos
		 * NumberOfChains: noc
		 * states: vector that contains all the states
		 * matrix: transition matrix/probability matrix
		 * startstates: vector with position of startstate of each chain
		 * actualstates: vector with position of actualstate of each chain
		 * nextstates: vector with position of nextstate of each chain
		 * nextTransitionTime: vector with Nexttransitiontime  of each chain
		 *
		 */

		MarkovBase(int nos,
			   int noc,
			   std::vector<T> states,
			   TransitionMatrixType matrix,
			   std::vector<int> sStates) :
			numberOfStates(nos),
			numberOfChains(noc),
			vectorOfStates(states),
			transitionsMatrix(matrix),
			startStates(sStates),
			actualStates(std::vector<int>(numberOfChains)),
			nextStates(std::vector<int>(numberOfChains)),
			nextTransitionTime(std::vector<double>(numberOfChains,0.0)),
			logger(std::string("Mrkv"), std::string("MarkovModel"))
		{
			actualStates = startStates;
			MESSAGE_SINGLE(VERBOSE,logger, "MarkovBase(C=" << noc << ",...) called");
		}

		/**
		 * @brief MarkovBase destructor
		 */

		virtual
		~MarkovBase()
		{
			MESSAGE_SINGLE(VERBOSE, logger, "~MarkovBase() called");
		}

		/**
		 * @brief setNumberOfStates.
		 * The allocation of vectors and matrices is done afterwards
		 */
		void
		setNumberOfStates(int _numberOfStates)
		{
			numberOfStates = _numberOfStates;
			prepareTransitionMatrixAndVectorOfStates();
		}

		/**
		 * @brief get Vector of States
		 */
		const std::vector<T>&
		getStates() const
		{
			assure(numberOfStates > 0, "numberOfStates uninitialized");
			return vectorOfStates;
		}

		/**
		 * @brief get the actual state index for a certain chain
		 */
		int
		getActualStateIndex(int actualchain) const
		{
			return actualStates[actualchain];
		}

		/**
		 * @brief get the state probability for a state
		 */
		double
		getStateProbability(int stateIndex) const
		{
			assure(!stateProbabilities.empty(),"stateProbabilities is empty");
			assure(stateProbabilities[0]>=0.0, "calculateStateProbabilities() has not been called before");
			return stateProbabilities[stateIndex];
		}

		const std::vector<double>& getStateProbabilities() const
		{
			return stateProbabilities;
		}


		/**
		 * @brief returns a state from the vector of states
		 */
		const T*
		getStateContent(int index) const
		{
			assure(!vectorOfStates.empty(),"vectorOfStates is empty");
			return &(vectorOfStates[index]);
		}
		/**
		 * @brief set Start States
		 */
		void
		setStartStates(std::vector<int> states)
		{
			startStates = states;
		}


		/**
		 * @brief set Transitions/probabilities Matrix elements
		 */
		void
		setTransitionMatrixElement(int row, int col, double value)
		{
			assure((row >= 0) && (row < numberOfStates), "wrong dimension");
			assure((col >= 0) && (col < numberOfStates), "wrong dimension");
			transitionsMatrix(row, col) = value;
		}

		/**
		 * @brief get Transitions/probabilities Matrix elements
		 */
		double
		getTransitionMatrixElement(int row, int col) const
		{
			assure((row >= 0) && (row < numberOfStates), "wrong dimension");
			assure((col >= 0) && (col < numberOfStates), "wrong dimension");
			return transitionsMatrix(row, col);
		}

		// wish: double&	operator[](int row, int col)

		/**
		 * @brief set Transitions/probabilities Matrix
		 */
		void
		setTransitionMatrix(const TransitionMatrixType& matrix)
		{
			transitionsMatrix = matrix; // copy
		}

		/**
		 * @brief Reads the number of states from a stream.
		 */
		int
		readNumberOfStates (std::istream& in)
		{
			std::string line;
			int nos = -1;
			while (in.good() && nos < 0) {
				std::getline(in, line);
				int comment_pos = line.find_first_not_of(" \t");
				if (line[comment_pos] != '#') {
					std::string ident;
					std::size_t pos_eq = line.find("=", 0);
					if (pos_eq != std::string::npos) {
						line.replace(pos_eq, 1, " ");
						std::istringstream instream(line);
						instream >> ident;
						instream >> nos;
						numberOfStates = nos;
					}
				}
			}
			prepareTransitionMatrixAndVectorOfStates();
			return nos;
		}

		/**
		 * @brief Reads the matrix of transitions/probabilities between
		 * the states from a stream.
		 */
		void
		readTransitionsFromFile(std::istream& in)
		{
			std::string line;
			assure((numberOfStates > 0), "numberOfStates in not defined");
			int row = 0;
			while (in.good() && row < numberOfStates) {
				std::getline(in, line);
				int comment_pos = line.find_first_not_of(" \t");
				if (line[comment_pos] != '#') {
				    std::istringstream instream(line);
				    double value;
				    for (int col = 0; col < numberOfStates; col++) {
					    instream >> value;
					    transitionsMatrix(row, col) = value;
				    }
				    row++;
				}
			}
			MESSAGE_BEGIN(VERBOSE, MarkovBase<T>::logger, m, "transitionMatrix=\n");
			m << transitionsMatrix;
			MESSAGE_END();
		}

	protected:
		/**
		 * @brief determine the probability of each state.
		 * this cannot be done here, as it depends on the
		 * transition matrix type (continuous or discrete)
		 */
		virtual void
		calculateStateProbabilities() // = 0 problematic because of MarkovBaseTest
		{
			assure(0, "cannot calculateStateProbabilities in MarkovBase");
		}

		/**
		 * @brief Internal representation of the number of states
		 */
		int numberOfStates;

		/**
		 * @brief Internal representation of the number of chains
		 */
		int numberOfChains;

	        /**
		 * @brief representation of vector of states
		 */
		std::vector<T> vectorOfStates;

		/**
		 * @brief Internal representation of the matrix of transitions/probabilities
		 */
		TransitionMatrixType transitionsMatrix;

		/**
		 * @brief Internal representation of start states: vector of
		 * start states, one for each chain.
		 */
		std::vector<int> startStates;

		/**
		 * @brief Internal representation of actual states: vector of
		 * actual states: one for each chain.
		 */
		std::vector<int> actualStates;

		/**
		 * @brief Internal representation of next state: vector of
		 * next states, one for each chain.
		 */
		std::vector<int> nextStates;

		/**
		 * @brief Internal representation of next state transition time: vector of
		 * next states transition time, one for each chain.
		 */
		std::vector<double> nextTransitionTime;

		/**
		 * @brief probability of each state. Calculated in calculateStateProbabilities
		 * (different for continuous and discrete)
		 */
		std::vector<double> stateProbabilities;

         /**
		 * @brief logger
		 */
		wns::logger::Logger logger;

	private:
		friend class MarkovBaseTest;

		/**
		 * @brief prepare transitions/probabilities Matrix and the
		 * vector of states. It is called when the numberOfStates
		 * is known.
		 */
		void
		prepareTransitionMatrixAndVectorOfStates()
		{
			assure(numberOfStates > 0, "numberOfStates uninitialized");
			transitionsMatrix = TransitionMatrixType(numberOfStates, numberOfStates);
			vectorOfStates = std::vector<T>(numberOfStates);
			stateProbabilities = std::vector<double>(numberOfStates,-1.0);
		}

	};//MarkovBase

} // markovchain
} // wns

#endif //WNS_MARKOVCHAIN_MARKOVBASE_HPP



/**
 * @file MarkovBase.hpp
 */



