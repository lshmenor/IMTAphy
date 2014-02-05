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

#ifndef WNS_LOGGER_DELIMITERFORMAT_HPP
#define WNS_LOGGER_DELIMITERFORMAT_HPP

#include <WNS/logger/FormatStrategy.hpp>

namespace wns { namespace logger {

	/**
	 * @brief Formats messages, elements are separated by a delimiter
	 *
	 * Each message is formatted into a single line. The elements of a
	 * message are separated by a configurable delimiter.
	 *
	 * The order of elements is:
	 * simulationTime | module | location | content
	 */
	class DelimiterFormat :
		virtual public FormatStrategy
	{
	public:
		DelimiterFormat(const pyconfig::View& pyco);

		virtual std::string
		formatMessage(const RawMessage& m);

		virtual std::string formatRegistration(const std::string& /*aModuleRef*/,
						       const std::string& /*aLocationRef*/) { return ""; };
	private:

		std::string
		escapeBackslash(const std::string& s);

		std::string delimiter;
	};

} // logger
} // wns

#endif // not defined WNS_LOGGER_DELIMITERFORMAT_HPP


