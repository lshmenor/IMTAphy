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

#ifndef CONSTANZE_HMM_HPP
#define CONSTANZE_HMM_HPP

#include <vector>

namespace constanze
{

	// comparision the result generated from data type double and long double
	//typedef double baumWelchDataType;
	typedef long double baumWelchDataType;

	/** @brief this class represent Hidden Markov Model */
	class HMM
        {

          public:

		/** @brief complex Constructor for initialize HMM Model*/
		HMM(int numberOfStates, std::vector<std::vector<baumWelchDataType>*> *transitionsMatrix, std::vector<std::vector<baumWelchDataType>*> *observationMatrix,std::vector<baumWelchDataType> *startStateProbability);

		/** @brief simple Comstructor for initialize HMM Model*/
		HMM(int numberOfStates);

		~HMM();

		/** @brief get number of states */
		int
		getNumberOfStates();

		/** @brief get probability of state index as start state */
		baumWelchDataType
		getStartStateProbability(int index);

		/** @brief get element in transiton matrix */
		baumWelchDataType
		getElementInTransitionsMatrix(int row, int column);

		/** @brief get element in observation  matrix */
		baumWelchDataType
		getElementInObservationMatrix(int row, int column);

		/** @brief interface for external circumstance to access
		 * transtion matrix
		 */
		std::vector<std::vector<baumWelchDataType>*>*
		getTransitionsMatrix();


	private:

		/** @brief representation of number of states */
		int numberOfStates;

		/** @brief representation of the matrix of transiton */
		std::vector<std::vector<baumWelchDataType>*> *transitionsMatrix;

		/** @brief representation of the matrix of observations */
		std::vector<std::vector<baumWelchDataType>*> *observationMatrix;

		/** @brief representation of probability of start states */
		std::vector<baumWelchDataType> *startStateProbability;

        };
}

#endif // NOT defined CONSTANZE_HMM_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
