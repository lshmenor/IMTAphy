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
#include <CONSTANZE/BaumWelch.hpp>

using namespace constanze;

BaumWelch::BaumWelch()
{
}

BaumWelch::~BaumWelch()
{
}

HMM*
BaumWelch::baumWelch(HMM *initialHMM, std::vector<int> *observationVector, int iterations)
{
	int numberOfStates = initialHMM->getNumberOfStates();

    for(int iteration = 0; iteration < iterations; iteration++) {

		// execution in every iteration forward and backward procedure
		std::vector<std::vector<baumWelchDataType>*> *forwardMatrix = forward(initialHMM, observationVector);
		std::vector<std::vector<baumWelchDataType>*> *backwardMatrix = backward(initialHMM, observationVector);

		// calculate new start probability
		std::vector<baumWelchDataType> *startStateProbability = new std::vector<baumWelchDataType>();

		for(int i = 0; i < numberOfStates; i++){
			startStateProbability->push_back(calculateGamma(i,0,forwardMatrix,backwardMatrix,initialHMM));
		}

		// calculate new transition matrix
		std::vector<std::vector<baumWelchDataType>*> *transitionsMatrix = new std::vector<std::vector<baumWelchDataType>*>();
		for(int i = 0; i < numberOfStates; i++){
			transitionsMatrix->push_back(new std::vector<baumWelchDataType>());
		}

		for(int i = 0; i < numberOfStates; i++){
			for(int  j = 0; j < numberOfStates; j++){
				baumWelchDataType num = 0.0;
				baumWelchDataType denom = 0.0;

				for(unsigned int t = 0; t < observationVector->size()-1; t++){
					num += calculateXi(t,i,j,observationVector,forwardMatrix,backwardMatrix,initialHMM);
					denom += calculateGamma(i,t,forwardMatrix,backwardMatrix,initialHMM);
				}
				transitionsMatrix->at(i)->push_back(divide(num,denom));
			}
		}

		/**
		 * observation matrix will in this fall not be changed, but this is only a
		 * special applying.
		 */
		std::vector<std::vector<baumWelchDataType>*> *observationMatrix = new std::vector<std::vector<baumWelchDataType>*>();

		for(int i = 0; i < numberOfStates; i++){
			observationMatrix->push_back(new std::vector<baumWelchDataType>());
		}

		for(int i = 0; i < numberOfStates; i++){
			for(int j = 0; j < numberOfStates; j++){
				if(i == j)
					observationMatrix->at(i)->push_back(1.0);
				else
					observationMatrix->at(i)->push_back(0.0);
			}
		}

		// set memory for hmm free
		delete initialHMM;

		initialHMM = new HMM(numberOfStates,transitionsMatrix,observationMatrix,startStateProbability);

		for(int i = 0; i < numberOfStates; i++){
			delete forwardMatrix->at(i);
			delete backwardMatrix->at(i);
		}

		delete forwardMatrix;
		delete backwardMatrix;

	} // for iteration

	/**
	 * baum-welch algorithm finished, return an object of class HMM with output
	 * transition matrix as paramete.r
	 */
    return initialHMM;

}

std::vector<std::vector<baumWelchDataType>*>*
BaumWelch::forward(HMM *initialHMM, std::vector<int> *observationVector)
{
	/**
	 * forward matrix will be contained in vector of vector, the interior vector
	 * for time und exterior for state.
	 */
	std::vector<std::vector<baumWelchDataType>*> *forwardMatrix = new std::vector<std::vector<baumWelchDataType>*>();

	for(int k = 0; k < initialHMM->getNumberOfStates(); k++){
		forwardMatrix->push_back(new std::vector<baumWelchDataType>());
	}

	/**
	 * initialize all value in forward matrix for all states in time 0.
	 * alpha_1(i) = pi_i * b_i(O_1)
	 * alpha_t(i): propability, that state sequence are O_1,O_2...,O_t and at
	 * time t in state i
	 * pi_i: start probability in state i
	 * b_i(O_t): probability, that in state i the state which at time t to be observed
	 */
	for(int i = 0; i < initialHMM->getNumberOfStates(); i++){
		forwardMatrix->at(i)->push_back(initialHMM->getStartStateProbability(i) * initialHMM->getElementInObservationMatrix(i,observationVector->at(0)));
	}

	/**
	 * with the values in time t-1 are all value in time t recursiv calculated.
	 * alpha_t+1(j) = (sum_{alpha_t(i)* a_ij}) * b_j(O_t+1)
	 * a_ij: transition probability from state i to state j
	 */
	for(unsigned int t = 1; t < observationVector->size(); t++){
		for(int j = 0; j < initialHMM->getNumberOfStates(); j++){
			baumWelchDataType sum = 0.0;
			for(int i = 0; i < initialHMM->getNumberOfStates(); i++){
				sum += forwardMatrix->at(i)->at(t-1) * initialHMM->getElementInTransitionsMatrix(i,j);
			}
			forwardMatrix->at(j)->push_back(sum * initialHMM->getElementInObservationMatrix(j,observationVector->at(t)));
		}
	}

	return forwardMatrix;
}

std::vector<std::vector<baumWelchDataType>*>*
BaumWelch::backward(HMM *initialHMM, std::vector<int> *observationVector)
{

	/**
	 * backward matrix will be contained in vector of vector, the interior vector
	 * for time und exterior for state.
	 */
	std::vector<std::vector<baumWelchDataType>*> *backwardMatrix = new std::vector<std::vector<baumWelchDataType>*>();

	/**
	 * for the simplicity of data storage, after storage is helpMatrix reversed,
	 * so achieve the backward matrix.
	 */
	std::vector<std::vector<baumWelchDataType>*> *helpMatrix = new std::vector<std::vector<baumWelchDataType>*>();

	for(int k = 0; k < initialHMM->getNumberOfStates(); k++){
		backwardMatrix->push_back(new std::vector<baumWelchDataType>());
		helpMatrix->push_back(new std::vector<baumWelchDataType>());
	}

	/**
	 * initialize all values of backward matrix for all states in time t, data will be
	 * storaged in first column in helpMatrix.
	 * beta_T(i) = 1
	 * beta_t(i): probability, that state sequence O_t+1,O_t+2,...,O_T are
	 * observed and at time t in state i
	 */
	for(int i = 0; i < initialHMM->getNumberOfStates(); i++){
		helpMatrix->at(i)->push_back(1.0);
	}

	/**
	 * recursiv calculate values in time t-1 with the given value in time t
	 * beta_t(i) = sum_{a_ij * b_j(O_t+1) * beta_t+1(j)}
	 */
	for(int t = observationVector->size()-2; t >= 0; t--){
		for(int i = 0; i < initialHMM->getNumberOfStates(); i++){
			baumWelchDataType sum = 0;
			for(int j = 0; j < initialHMM->getNumberOfStates(); j++){
				sum += helpMatrix->at(j)->at(observationVector->size()-2-t) * initialHMM->getElementInTransitionsMatrix(i,j) * initialHMM->getElementInObservationMatrix(j,observationVector->at(t+1));
			}
			helpMatrix->at(i)->push_back(sum);
		}
	}

	// reverse helpMatrix to achieve backward matrix
	for(int i = 0; i < initialHMM->getNumberOfStates(); i++){
		for(unsigned int j = 0; j < observationVector->size(); j++){
			backwardMatrix->at(i)->push_back(helpMatrix->at(i)->at(observationVector->size()-1-j));
		}
	}

	// set memory free
	for(int i = 0; i < initialHMM->getNumberOfStates(); i++){
		delete helpMatrix->at(i);
	}

	delete helpMatrix;

	return backwardMatrix;
}

/**
 * calculate gamma_i(t).
 * gamma_i(t): given state sequence O, probability, that at time t in state i
 */
baumWelchDataType
BaumWelch::calculateGamma(int i,int t,std::vector<std::vector<baumWelchDataType>*> *forwardMatrix,std::vector<std::vector<baumWelchDataType>*> *backwardMatrix,HMM *h)
{

	// num = alpha_t(i) * beta_t(i)
	baumWelchDataType num = forwardMatrix->at(i)->at(t) * backwardMatrix->at(i)->at(t);
	baumWelchDataType denom = 0.0;

	// denom = sum_{alpha_t(j) * beta_t(j)}
	for(int j = 0; j < h->getNumberOfStates(); j++)
		denom += forwardMatrix->at(j)->at(t) * backwardMatrix->at(j)->at(t);
	return divide(num,denom);

}

/**
 * calculate Xi_t(i,j)
 * Xi_t(i,j): probability, that at time t in state i and at time t+1 in state j
 */
baumWelchDataType
BaumWelch::calculateXi(int t,int i,int j,std::vector<int> *observationVector,std::vector<std::vector<baumWelchDataType>*> *forwardMatrix,std::vector<std::vector<baumWelchDataType>*> *backwardMatrix,HMM *h)
{

	baumWelchDataType num;
	baumWelchDataType denom = 0.0;

	// num = alpha_t(i) * a_ij * b_j(O_t+1) * beta_t+1(j)
	num = forwardMatrix->at(i)->at(t) * h->getElementInTransitionsMatrix(i,j) * h->getElementInObservationMatrix(j,observationVector->at(t+1)) * backwardMatrix->at(j)->at(t+1);

	// denom = sum_{sum_{num}}
	for(int j = 0; j < h->getNumberOfStates(); j++){
		for(int i = 0; i < h->getNumberOfStates(); i++){
			denom += forwardMatrix->at(i)->at(t) * h->getElementInTransitionsMatrix(i,j) * h->getElementInObservationMatrix(j,observationVector->at(t+1)) * backwardMatrix->at(j)->at(t+1);
		}
	}
	// try { result=num/denom; } catch ...
	return divide(num,denom);
}

baumWelchDataType
BaumWelch::divide(baumWelchDataType num, baumWelchDataType denom)
{
	if(denom == 0.0)
		return 0.0;
	else
		return num/denom;
}

HMM*
BaumWelch::getHMM(){
	return hmm;
}

