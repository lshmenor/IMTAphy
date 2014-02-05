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

#ifndef __WNS_LOGGER_LOGGERTESTHELPER_HPP
#define __WNS_LOGGER_LOGGERTESTHELPER_HPP

#include <WNS/logger/Master.hpp>

namespace wns { namespace logger {
	class TestFormat :
		virtual public FormatStrategy
	{
	public:
		TestFormat(const pyconfig::View&)
		{}

		std::string formatMessage(const RawMessage& m)
		{
			return m.module + m.location + m.message;
		}

		std::string
		formatRegistration(const std::string& /*aModuleRef*/,
				   const std::string& /*aLocationRef*/)
		{
			return "";
		}
	}; // TestFormat

	class TestOutput :
		virtual public OutputStrategy
	{
	public:
		virtual void operator <<(const std::string& m)
		{
			result += m;
		}
		static std::string result;
	}; // TestOutput
} // logger
} // wns
#endif // not defined__WNS_LOGGER_LOGGERTESTHELPER_HPP


