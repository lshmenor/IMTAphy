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

#ifndef WNS_STATUSREPORT_HPP
#define WNS_STATUSREPORT_HPP

#include <WNS/events/PeriodicRealTimeout.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <ctime>
#include <string>

namespace wns { namespace simulator {

	class StatusReport :
		public wns::events::PeriodicRealTimeout
	{
	public:
		class WriteError :
			public wns::Exception
		{
		public:
			WriteError(const std::string& fileName) :
				wns::Exception("Couldn't write to file: " + fileName)
			{
			}
            ~WriteError() throw() {}
		};

		StatusReport();

		void
		start(const pyconfig::View& _pyConfigView);

		void
		stop();

		void
		periodically();

		void
		writeStatus(bool anEndOfSimFlag, std::string otherFileName = "");

	private:
		double maxSimTime;
		time_t startTime;
		std::string outputDir;
		std::string statusFileName;
		std::string progressFileName;

        wns::probe::bus::ContextCollectorPtr memoryConsumption;
        wns::probe::bus::ContextCollectorPtr simTimePerRealTime;

        void
        probe();
	};
} // simulator
} // wns

#endif // WNS_STATUSREPORT_HPP
