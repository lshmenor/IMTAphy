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

#ifndef WNS_SERVICE_PHY_OFDMA_PATTERN_HPP
#define WNS_SERVICE_PHY_OFDMA_PATTERN_HPP

#include <vector>
#include <WNS/SmartPtr.hpp>
#include <WNS/PowerRatio.hpp>

namespace rise { namespace antenna {
	class Beamforming;
}}

namespace wns { namespace service { namespace phy { namespace ofdma {

	class Pattern :
		virtual public wns::RefCountable
	{
		friend class rise::antenna::Beamforming;
	public:
		wns::Ratio getOmniAttenuation() const
			{
				return omniAttenuation;
			}

		double getEntry(unsigned int i) const
			{
				return pattern.at(i);
			}

		unsigned int getSize() const
			{
				return pattern.size();
			}

		wns::Ratio getMaxGain() const
			{
				assure(!pattern.empty(), "empty pattern has no maximum gain");
				double maxEntry = 0.0;
				for (unsigned int i = 0; i < pattern.size();  i++) {
					if(pattern.at(i) > maxEntry)
						maxEntry = pattern.at(i);
				}
				assure(maxEntry > 0.0, "all zero pattern seems to be invalid");
				return wns::Ratio::from_factor(maxEntry * maxEntry);
			}

	protected:
		std::vector<double> pattern;
		/** gives the ratio of the power that passes through the pattern
		 * as compared to the power that an omnidirectional antenna
		 * would absorb/emit. This is needed in the SDMA reception case
		 * at a BF antenna to derive the intercell interference. See
		 * @sa ofdma::Receiver for details.
		 * @todo: ALWAYS SET TO ONE --> TO BE REMOVED
		 */
		wns::Ratio omniAttenuation;
	};

	class SumPattern :
		public Pattern
	{
	public:
		void add(wns::SmartPtr<Pattern> other)
			{
				assure(other->getSize() > 0, "pattern to add is empty");
				if(pattern.empty()){
					pattern.reserve(other->getSize());
					for (unsigned int i = 0; i < other->getSize();  i++) {
						pattern.push_back(other->getEntry(i));
					}
				}
				else{
					assure(pattern.size() == other->getSize(), "patterns do not have the same size");
					for (unsigned int i = 0; i < pattern.size();  i++) {
						pattern[i] += other->getEntry(i);
					}
				}
			}
	};

	typedef wns::SmartPtr<Pattern> PatternPtr;

} // ofdma
} // phy
} // service
} // wns
#endif



