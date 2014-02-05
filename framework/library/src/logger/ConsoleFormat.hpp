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

#ifndef WNS_LOGGER_CONSOLEFORMAT_HPP
#define WNS_LOGGER_CONSOLEFORMAT_HPP

#include <WNS/logger/FormatStrategy.hpp>

namespace wns { namespace logger {

	class ConsoleFormat :
		virtual public FormatStrategy
	{
	public:
		ConsoleFormat(const pyconfig::View& pyco);

		virtual std::string formatMessage(const RawMessage& m);

		virtual std::string formatRegistration(const std::string& /*aModuleRef*/,
						       const std::string& /*aLocationRef*/) { return ""; };
	private:
		std::string highlightModuleName(const std::string& module);

		bool blackNWhite;

		/**
		 * @brief module::Modulename 2 color mapping
		 */
		std::map<std::string, std::string> module2color;

		/**
		 * @brief The time is displayed in a field of this width
		 */
		unsigned long int debugTimeWidth;

		/**
		 * @brief Number of digits after decimal point
		 */
		unsigned long int debugTimePrecision;

		/**
		 * @brief The maximum length of the string of the location
		 */
		unsigned long int maxLocationLength;

		/**
		 * @brief The length of the string of the "longest" location
		 */
		unsigned long int currentLocationLength;

	}; // ConsoleFormat

} // logger
} // wns

#endif // not defined WNS_LOGGER_CONSOLEFORMAT_HPP


