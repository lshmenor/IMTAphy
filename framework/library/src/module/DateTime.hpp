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

#ifndef WNS_MODULE_DATETIME_HPP
#define WNS_MODULE_DATETIME_HPP

#include <string>
#include <ctime>

namespace wns { namespace module {
	/**
	 * @brief Class to represent points in time.
	 */
	class DateTime
	{
	public:
		/**
		 * @brief Default Constructor.
		 *
		 * This constructor creates an empty DateTime.
		 *
		 * @see isEmpty()
		 */
		DateTime();
		/**
		 * @brief time_t Constructor.
		 *
		 * This constructor creates a DateTime instance initialized by a
		 * time_t. time_t is the standard C type for times. It contains
		 * the seconds elapsed since 1970-01-01 00:00:00.
		 */
		DateTime(const time_t t);

		/**
		 * @brief Equality operator.
		 */
		bool operator==(const DateTime b) const;
		/**
		 * @brief Inequality operator.
		 */
		bool operator!=(const DateTime b) const;
		/**
		 * @brief Greater than operator.
		 */
		bool operator>(const DateTime b) const;
		/**
		 * @brief Less than operator.
		 */
		bool operator<(const DateTime b) const;
		/**
		 * @brief Greater or equal operator.
		 */
		bool operator>=(const DateTime b) const;
		/**
		 * @brief Less or equal operator.
		 */
		bool operator<=(const DateTime b) const;

		/**
		 * @brief Returns the time as time_t.
		 * @returns the time as time_t.
		 */
		time_t getTime() const;
		/**
		 * @brief Returns the time as a string.
		 * @returns the time as string.
		 */
		std::string getString() const;
		/**
		 * @brief Check if the DateTime object is empty
		 * @returns true, if the DateTime object is empty.
		 */
		bool isEmpty() const;

	private:
		/**
		 * @brief Internal representation of time
		 */
		time_t time;

		/**
		 * @brief True if no time is stored
		 */
		bool empty;

	};
}
}
#endif // NOT defined WNS_MODULE_DATETIME_HPP
