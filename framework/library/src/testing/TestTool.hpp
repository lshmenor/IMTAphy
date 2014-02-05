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

#ifndef WNS_TESTING_TESTTOOL_HPP
#define WNS_TESTING_TESTTOOL_HPP

#include <string>
#include <vector>

namespace wns { namespace testing {

	/**
	 * @brief Match a file against a regular expression.
	 *
	 */
	bool compareFile(const std::string filename, const std::string regex);

	/**
	 * @brief Check whether a file matches a regexp
	 */
	bool matchInFile(const std::string filename, const std::string regex);

	/**
	 * @brief Check whether a file matches a list of regexps
	 */
	bool matchInFile(const std::string filename, std::vector<std::string> regexps);

	/**
	 * @brief Check whether a string matches a regexp
	 */
	bool matchInString(const std::string content, const std::string regex);

	/**
	 * @brief Check whether a string matches a list of regexps
	 */
	bool matchInString(const std::string content, std::vector<std::string> regexps);

	/**
	 * @brief Match a string against a regular expression.
	 *
	 */
	bool compareString(const std::string content, const std::string regex, bool matchEntireContent = true);

}
}

#endif // NOT defined WNS_TESTING_TESTTOOL_HPP


