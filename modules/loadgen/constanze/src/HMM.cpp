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

#include <CONSTANZE/HMM.hpp>

using namespace constanze;

HMM::HMM(int numberOfStates):
	numberOfStates(numberOfStates)
{
	// data type for transitionsMatrix and observationMatrix are vector of vector
	transitionsMatrix = new std::vector<std::vector<baumWelchDataType>*>();
	observationMatrix = new std::vector<std::vector<baumWelchDataType>*>();
	startStateProbability = new std::vector<baumWelchDataType>();

	for(int i = 0; i < numberOfStates; i++){
		transitionsMatrix->push_back(new std::vector<baumWelchDataType>());
	}

	for(int j = 0; j < numberOfStates; j++){
		observationMatrix->push_back(new std::vector<baumWelchDataType>());
	}

	// initialize transitionsMatrix, observationMatrix and startStateProbability
	for(int k = 0; k < numberOfStates; k++){
		for(int l = 0; l < numberOfStates; l++){
			transitionsMatrix->at(k)->push_back(1.0/numberOfStates);
			if(k == l)
				observationMatrix->at(k)->push_back(1.0);
			else
				observationMatrix->at(k)->push_back(0.0);
		}
		startStateProbability->push_back(1.0/numberOfStates);
	}

}

HMM::HMM(int numberOfStates,
		 std::vector<std::vector<baumWelchDataType>*> *transitionsMatrix,
		 std::vector<std::vector<baumWelchDataType>*> *observationMatrix,
		 std::vector<baumWelchDataType> *startStateProbability):

         numberOfStates(numberOfStates),
		 transitionsMatrix(transitionsMatrix),
         observationMatrix(observationMatrix),
         startStateProbability(startStateProbability)
{
}

HMM::~HMM()
{
	//set memory for interior vector free
	for(int i = 0; i < numberOfStates; i++){
		delete transitionsMatrix->at(i);
	}

	for(int j = 0; j < numberOfStates; j++){
		delete observationMatrix->at(j);
	}

	//set memory for exterior vector free
	delete transitionsMatrix;
	delete observationMatrix;
	delete startStateProbability;
}

int
HMM::getNumberOfStates()
{
    return numberOfStates;
}

baumWelchDataType
HMM::getStartStateProbability(int index)
{
	return startStateProbability->at(index);
}

baumWelchDataType
HMM::getElementInTransitionsMatrix(int row, int column)
{
	return transitionsMatrix->at(row)->at(column);
}

baumWelchDataType
HMM::getElementInObservationMatrix(int row, int column)
{
	return observationMatrix->at(row)->at(column);
}

std::vector<std::vector<baumWelchDataType>*>*
HMM::getTransitionsMatrix()
{
	return transitionsMatrix;
}
