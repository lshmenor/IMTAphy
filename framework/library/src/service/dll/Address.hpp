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

#ifndef WNS_SERVICE_DLL_ADDRESS_HPP
#define WNS_SERVICE_DLL_ADDRESS_HPP

#include <WNS/osi/PDU.hpp>

#include <stdint.h>
#include <set>
#include <ostream>

namespace wns { namespace service { namespace dll {

	/**
	 * @brief Base class of each DLL address
	 *
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 */
	class Address
	{
	public:
		virtual
		~Address()
		{};
	}; // Address

	/**
	 * @brief DLL address to address exactly one station
	 *
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 */
	class UnicastAddress :
		virtual public Address
	{
	public:
		/**
		 * @brief Used to mark invalid addresses
		 */
		static const long int invalid;

		/**
		 * @brief Address will be intialized to UnicastAddres::invalid
		 */
		UnicastAddress();

		/**
		 * @brief Construct Address from long int
		 */
		explicit
		UnicastAddress(long int _address);

		/**
		 * @brief Destructor
		 */
		virtual
		~UnicastAddress();

		/**
		 * @brief Enables storing in std::map
		 */
		virtual bool
		operator <(const UnicastAddress& other) const;

		/**
		 * @brief Enables storing in std::map
		 */
		virtual bool
		operator ==(const UnicastAddress& other) const;

		/**
		 * @brief Enables storing in wns::container::Matrix
		 */
		virtual bool
		operator !=(const UnicastAddress& other) const;

		/**
		 * @brief Comfortable usage in streams
		 */
		friend std::ostream&
		operator<<(std::ostream& str, const UnicastAddress& address)
		{
			str << address.address;
			return str;
		}

		/**
		 * @brief Enables usage in wns::pyconfig::View::get
		 */
		friend std::istream&
		operator>>(std::istream &str, UnicastAddress& address)
		{
			long int tmp;
			str >> tmp;

			address.address = tmp;
			if(!(address.address > 0))
			{
				wns::Exception e;
				e << "Address must be greater than 0. Was: " << tmp;
				throw e;
			}
			return str;
		}

		/**
		 * @brief Returns true if UnicastAddress is valid, false else
		 */
		bool
		isValid() const;

		/**
		 * @brief Returns the internal representation (for SDL)
		 */
		long int
		getInteger() const;

	private:
		/**
		 * @brief Internal representation
		 */
		long int address;
	}; // UnicastAddress

	/**
	 * @brief DLL address to address all stations
	 *
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 *
	 * This class is intially left empty. It is only here to distinguish
	 * types.
	 */
	class BroadcastAddress :
		virtual public Address
	{
	}; // BroadcastAddress

} // dll
} // service
} // wns

#endif // NOT defined WNS_SERVICE_DLL_ADDRESS_HPP



