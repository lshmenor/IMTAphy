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

#ifndef CONSTANZE_BAUMWELCH_HPP
#define CONSTANZE_BAUMWELCH_HPP

#include <CONSTANZE/HMM.hpp>

namespace constanze
{

	/** @brief this class implement Baum-Welch algorithm
	    Literature: TODO
	 */

	// implementation with templete is possible
	//template baumWelchDataType
	//class BaumWelch<baumWelchDataType>
	class BaumWelch
        {
        public:
	        BaumWelch();

                ~BaumWelch();

	        /**
		 * @brief baum-welch algorithm.
		 * initialHMM: the Hidden Morcov Model, that was at begin initialized
		 * observationVector: states sequence
		 * iteration: the iterationen of execution of baum-welch algorithm
		 */
	        HMM* baumWelch(HMM *initialHMM, std::vector<int> *observationVector,int iterations);

		/** @brief interface to access HMM from external circumstance*/
		HMM* getHMM();

	private:

		/** @brief represent an object of class HMM*/
		HMM *hmm;

                /**
		 * @brief implement forward algorithm.
		 * initialHMM: the HMM, that was at begin initialized
		 * observationVector: states sequence
		 */
                std::vector<std::vector<baumWelchDataType>*>*
                forward(HMM *initialHMM, std::vector<int> *observationVector);

                /**
		 * @brief implement backward algorithm.
		 * initialHMM: the HMM, that was at begin initialized
		 * observationVector: states sequence
		 */
                std::vector<std::vector<baumWelchDataType>*>*
                backward(HMM *initialHMM, std::vector<int> *observationVector);

		/**
		 * @brief calculate gamma value in state i and time t.
		 * i: state i
		 * t: time t
		 * forwardMatrix: the Matrix, that was calculated with forward procedure
		 * backwardMatrix: the Matrix, that was calculated with backward procedure
		 * h: object of Hidden Marcov Model
		 */
		baumWelchDataType
		calculateGamma(int i, int t, std::vector<std::vector<baumWelchDataType>*> *forwardMatrix, std::vector<std::vector<baumWelchDataType>*> *backwardMatrix, HMM *h);

		/**
		 * @brief calculate Xi function of baum-welch algorithm.
		 * t: time t
		 * i: state i
		 * j: state j
		 * observationVector: state sequence
		 * forwardMatrix: the Matrix, that was calculated with forward procedure
		 * backwardMatrix: the Matrix, that was calculated with backward procedure
		 */
		baumWelchDataType
		calculateXi(int t,int i,int j,std::vector<int> *observationVector,std::vector<std::vector<baumWelchDataType>*> *forwardMatrix,std::vector<std::vector<baumWelchDataType>*> *backwardMatrix,HMM *h);

		/**
		 * @brief divide opeation, award 0 value by denom
		 * num: numerator
		 * denom: denominator
		 */
		baumWelchDataType
		divide(baumWelchDataType num,baumWelchDataType denom);

        };
}

#endif // NOT defined CONSTANZE_BAUMWELCH_HPP

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
