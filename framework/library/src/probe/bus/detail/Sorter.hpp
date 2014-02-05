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

#ifndef WNS_PROBE_BUS_DETAIL_SORTER_HPP
#define WNS_PROBE_BUS_DETAIL_SORTER_HPP

#include <WNS/pyconfig/View.hpp>

namespace wns { namespace probe { namespace bus { namespace detail {

	typedef unsigned int IDType;

	/** @brief Helper class for equidistant sorting */
	class Sorter
	{
		std::string idName_;
		IDType min_;
		IDType max_;
		int resolution_;
		IDType stepsize_;

		int calcIndex(IDType id) const;
	public:
		Sorter(const wns::pyconfig::View& pyco);
		Sorter(std::string _idName, IDType _min, IDType _max, int _resolution);

		int getIndex(IDType id) const;
		bool checkIndex(IDType id) const;
		std::string getInterval(int index) const;
		IDType getMin(int index) const;
		int getResolution() const;
		std::string getIdName() const;
	};

}}}}
#endif // not defined WNS_PROBE_BUS_DETAIL_SORTER_HPP
