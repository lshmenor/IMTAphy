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

#ifndef WNS_PYCONFIG_PARSER_HPP
#define WNS_PYCONFIG_PARSER_HPP

#include <WNS/pyconfig/View.hpp>

namespace wns { namespace pyconfig {

	/**
	 * @brief Load Python style config files
	 * @ingroup group_main_classes
	 */
	class Parser
		: public View
	{
	public:
		/**
		 * @brief Default constructor
		 */
		Parser();

		/**
		 * @brief Constructor loading the given file as configuration
		 */
		explicit
		Parser(const std::string& filename, const std::string& pathname = "");

		/**
		 * @brief Destructor
		 */
		virtual
		~Parser();

		/**
		 * @brief Load "filename"
		 */
		void
		load(const std::string& filename);

		/**
		 * @brief Use "s" as configuration content
		 */
		void
		loadString(const std::string& s);

		/**
		 * @brief Append pathname to the python search path
		 *
		 * @note sys.path is shared across Parser instances
		 */
		void
		appendPath(const std::string& pathname);

		/**
		 * @brief Create a View object from a string
		 */
		static View
		fromString(const std::string& s);

	private:
		/**
		 * @brief initialize the Python interpreter
		 */
		void
		initPython();

		static long serial;

		std::string module_name;
	};

}}

#endif // NOT defined WNS_PYCONFIG_PARSER_HPP


/**
 * @file
 */


