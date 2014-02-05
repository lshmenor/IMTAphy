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

#ifndef WNS_CPPUNIT_HPP
#define WNS_CPPUNIT_HPP

#include <WNS/TestFixture.hpp>
#include <WNS/evaluation/statistics/moments.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/Conversion.hpp>

namespace wns {

	/**
	 * @brief Specialized version for Power
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 */
	inline void
	assertMaxRelError(
		const Power& expected,
		const Power& actual,
		double maxRelativeError,
		const std::string sourceFile,
		const std::string sourceLine)
	{
		std::string expectedString = wns::to<std::string>(expected.get_dBm()) + " dBm";
		std::string actualString = wns::to<std::string>(actual.get_dBm()) + " dBm";
		std::string msg = wns::failureMessage(sourceFile, sourceLine, expectedString, actualString);
		bool ok = std::fabs(expected.get_mW() - actual.get_mW()) <= std::fabs(expected.get_mW() * maxRelativeError);

		CPPUNIT_ASSERT_MESSAGE(msg, ok);
	}

	/**
	 * @brief Specialized version for Ratio
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 */
	inline void
	assertMaxRelError(
		const Ratio& expected,
		const Ratio& actual,
		double maxRelativeError,
		const std::string sourceFile,
		const std::string sourceLine)
	{
		std::string expectedString = wns::to<std::string>(expected.get_dB()) + " dB";
		std::string actualString = wns::to<std::string>(actual.get_dB()) + " dB";
		std::string msg = failureMessage(sourceFile, sourceLine, expectedString, actualString);

		bool ok = std::fabs(expected.get_factor() - actual.get_factor()) <= std::abs(expected.get_factor() * maxRelativeError);
		CPPUNIT_ASSERT_MESSAGE(msg, ok);
	}

	template <class SAMPLEGENERATOR>
	void assertMeanValue(
		double expected,
		SAMPLEGENERATOR& sampleGenerator,
		unsigned long int maxTrials,
		const std::string sourceFile,
		const std::string sourceLine)
	{
		assure(maxTrials > 100, "needs a least 100 trials, please increase maxTrials");
		std::stringstream maxTrialsExceeded;
		maxTrialsExceeded << "Exceeded maxTrials (" << maxTrials << ") without reaching the expected mean value (" << expected << ")";
		const std::string maxTrialsExceededString = maxTrialsExceeded.str();
                wns::evaluation::statistics::Moments m;

		while(m.trials() <= 100)
		{
			m.put(sampleGenerator());
		}

		double relCf = 1.0;
		while (((relCf > 10e-4) && (std::fabs(expected - m.mean()) > m.getConfidenceInterval99Mean()/10)))
		{
			m.put(sampleGenerator());
			while(m.trials()%100 != 0)
			{
				m.put(sampleGenerator());
			}

			if(m.mean() != 0)
			{
				relCf = m.getConfidenceInterval99Mean()/m.mean();
			}
			else
			{
				relCf = m.getConfidenceInterval99Mean();
			}
			CPPUNIT_ASSERT_MESSAGE( maxTrialsExceededString, m.trials() < maxTrials );
		}

		double ci99mean = m.getConfidenceInterval99Mean();
		std::stringstream actualString;
		actualString << (m.mean() - ci99mean) << " ... " << (m.mean() + ci99mean) << " (" << m.trials() << " trials)";
		std::string msg = failureMessage(sourceFile, sourceLine, wns::to<std::string>(expected), actualString.str());
		CPPUNIT_ASSERT_MESSAGE( msg, std::fabs(expected - m.mean()) <= ci99mean);
	}
}

#define WNS_ASSERT_MEAN_VALUE(expected, generator, maxTrials) wns::assertMeanValue((expected), (generator), (maxTrials),__FILE__,  wns::to<std::string>(__LINE__))


#endif
