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

#ifndef WNS_LOGGER_MESSAGE_HPP
#define WNS_LOGGER_MESSAGE_HPP

#include <sstream>

namespace wns { namespace logger {

	/**
	 * @brief Messages are used in conjunction with Logger instead of simple
	 * "cout-Logging"
	 *
	 * @ingroup logging
	 */
	class Message
	{
	public:
		/**
		 * @brief Default constructor
		 */
		Message();

		/**
		 * @brief Constuctor setting an initial message
		 */
		explicit
		Message(const std::string& m);

		/**
		 * @brief Constructor setting an initial message and verbosity level
		 */
		Message(const std::string& m, long int _level);

		/**
		 * @brief Copy constructor
		 */
		Message(const Message& m);

		/**
		 * @brief Use Messages like streams ...
		 */
		template <typename Input>
		Message&
		operator <<(const Input& i)
		{
			message << i;
			return *this;
		}

		/**
		 * @brief Use Message like streams ...
		 *
		 * Usgae: stringstream << Message;
		 */
		friend
		std::stringstream&
		operator <<(std::stringstream& str, const Message& m)
		{
			str << m.message.str();
			return str;
		}

		/**
		 * @brief Return the content of the Message as std::string
		 */
		std::string
		getString() const;

		/**
		 * @brief Return the verbosity level of the Message
		 */
		long int
		getLevel() const;

	private:
		/**
		 * @brief content of the Message
		 */
		std::stringstream message;

		/**
		 * @brief The verbosity level of the message
		 */
		long int level;
	}; // Message
} // logger
} // wns

#endif // not defined __WNS_LOGGER_MESSAGE_HPP


