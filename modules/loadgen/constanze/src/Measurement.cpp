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


#include <CONSTANZE/Measurement.hpp>
#include <iostream>

using namespace constanze;

Measurement::Measurement(const wns::pyconfig::View& config):
	pyco(config),
	log(pyco.get("logger")),
	estimatorNumberOfStates(config.get<int>("estimatorNumberOfStates")),
	probeWindow(config.get<double>("probeWindow")),
	MMPPestimationResultFileName(config.get<std::string>("MMPPestimationResultFileName"))
{
	MESSAGE_SINGLE(NORMAL, log, "Measurement created.");
	MESSAGE_SINGLE(NORMAL, log, "estimatorNumberOfStates="<<estimatorNumberOfStates);
	meanRateVector = new std::vector<double>();

	// pyconfig, configurate mean rate for every state
	for(int i = 0; i < config.get<int>("estimatorNumberOfStates"); i++) {
		double meanRate = pyco.get<double>("meanRateList",i);
		meanRateVector->push_back(meanRate);
		MESSAGE_SINGLE(NORMAL, log, "  meanRateList["<<i<<"]="<<meanRate);
	}

	boundaryVector = new std::vector<double>();

	/**
	 *calculation boundary between 2 states, index 0 for boundary between state
	 * 0 an 1, index 1 for boundary between state 1 and 2 etc.
	 */
	for(unsigned int j = 0; j < meanRateVector->size()-1; j++){
		double boundary = meanRateVector->at(j)+(meanRateVector->at(j+1)-meanRateVector->at(j))/2;
		boundaryVector->push_back(boundary);
		MESSAGE_SINGLE(NORMAL, log, "  boundaryVector["<<j<<"]="<<boundary);
	}
}

Measurement::~Measurement()
{
	delete meanRateVector;
	delete boundaryVector;
}

HMM*
Measurement::calculate(int numberOfStates, std::vector<int> *observationVector)
{
	int iterations = 100; // make sense

	HMM *initialHMM = new HMM(numberOfStates);
	HMM *hmm = baumwelch->baumWelch(initialHMM, observationVector, iterations);

	return hmm;
}

void
Measurement::output(std::vector<std::vector<baumWelchDataType>*> *transitionsMatrix, int numberOfStates)
{
	for(int i = 0; i < numberOfStates; i++){
		for(int j = 0; j < numberOfStates; j++){
			std::cout << transitionsMatrix->at(i)->at(j) << '\t';
		}
		std::cout << std::endl;
	}
}

void Measurement::output_python(std::vector<std::vector<baumWelchDataType>*> *transitionsMatrix, int numberOfStates,std::string estimationResultFileName)
{
	std::fstream outfile;
	outfile.open(estimationResultFileName.c_str(),std::ios::out);


	outfile << "numberOfStates = " << numberOfStates << std::endl;
	outfile << "slotTime = " << probeWindow << std::endl;
	for(int i = 0; i < numberOfStates; i++){
		outfile << "# rate["<<i<<"] = "<< meanRateVector->at(i) << "bit/s" << std::endl;
	}

	outfile << "transitionMatrix = " << " (";
	for(int i = 0; i < numberOfStates; i++){

		if(i != 0)
			outfile << "                     ";

		outfile << "( ";

		for(int j = 0; j < numberOfStates; j++){

			if(j != numberOfStates-1)
				outfile << transitionsMatrix->at(i)->at(j) << ", ";
			else{
				if(i != numberOfStates-1)
					outfile << transitionsMatrix->at(i)->at(j) << " ),";
				else
					outfile << transitionsMatrix->at(i)->at(j) << " ))";
			}
		}
		outfile << std::endl;
	}

	outfile.close();
}

int
Measurement::getEstimatorNumberOfStates()
{
	return estimatorNumberOfStates;
}

std::string
Measurement::getMMPPestimationResultFileName()
{
	return MMPPestimationResultFileName;
}

