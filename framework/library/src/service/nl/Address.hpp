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

#ifndef WNS_SERVICE_NL_ADDRESS_HPP
#define WNS_SERVICE_NL_ADDRESS_HPP

#include <WNS/Assure.hpp>

#include <ostream>
#include <sstream>
#include <string>

namespace wns { namespace service { namespace nl {

        /**
	 * @brief Values for the protocol header field. See RFC 1700 for details.
	 */
	enum protocolNumber {
		IP = 4,
		TCP = 6,
		UDP = 17,
		IPv6 = 41,
		// Numbers from 138-252 are not assigned yet
		Constanze = 138,
		PubSub = 139,
		Overlay = 140
		// Numbers 253-254 are for experiments and tests
		// Number 255 is reserved
	};

	typedef std::string FQDN;

	class Address
	{
	public:
		Address() :
			address(0)
		{}

		explicit
		Address(const std::string& addr)
		{
			std::stringstream tmp(addr);
			char c;
			unsigned long int n1;
			unsigned long int n2;
			unsigned long int n3;
			unsigned long int n4;
			tmp >> n1;
			tmp >> c;
			assure(c=='.', "Malformed IPAddress given!");
			tmp >> n2;
			tmp >> c;
			assure(c=='.', "Malformed IPAddress given!");
			tmp >> n3;
			tmp >> c;
			assure(c=='.', "Malformed IPAddress given!");
			tmp >> n4;
			assure(n1<=255, "Malformed IPAddress given!");
			assure(n2<=255, "Malformed IPAddress given!");
			assure(n3<=255, "Malformed IPAddress given!");
			assure(n4<=255, "Malformed IPAddress given!");

			address = (n1 << 24) + (n2 << 16) + (n3 << 8) + n4;
			}

		explicit
		Address(unsigned long int a)
		{
			address=a;
		}

		bool
		operator==(const Address& right) const
		{
			return address == right.address;
		}

		bool
		operator!=(const Address& right) const
		{
			return !( (*this)==right);
		}

		void
		operator=(const Address& src)
		{
			address = src.address;
		}

		bool
		operator<(const Address& src) const
		{
			return address < src.address;
		}

		bool
		operator>(const Address& src) const
		{
			return address > src.address;
		}

		bool
		operator<=(const Address& src) const
		{
			return (((*this) < src) || ((*this) == src));
		}

		bool
		operator>=(const Address& src) const
		{
			return (((*this) > src) || ((*this) == src));
		}

		const Address
		operator++()
		{
			++address;
			return *this;
		}

		const Address
		operator++(int)
		{
			Address tmp(*this);
			++address;
			return tmp;
		}

		const Address
		operator--()
		{
			--address;
			return *this;
		}

		const Address
		operator--(int)
		{
			Address tmp(*this);
			--address;
			return tmp;
		}

		Address
		operator&(Address mask) const
		{
			return Address(address&mask.address);
		}

		Address
		operator|(Address mask) const
		{
			return Address(address|mask.address);
		}

		friend
		std::ostream&
		operator <<(std::ostream& str, const Address& a)
		{
			std::stringstream tmp;
			unsigned long int n1 = (a.address&(255<<24))>>24;
			unsigned long int n2 = (a.address&(255<<16))>>16;
			unsigned long int n3 = (a.address&(255<<8))>>8;
			unsigned long int n4 = a.address&255;

			// Write to tempory and then write to output
			// You need this when someone outside uses manipulators on the stream

			tmp << n1 << "." << n2 << "." << n3 << "." << n4;
			str << tmp.str();
			return str;
		}

		unsigned long int
		getInteger() const { return this->address; }

	private:
		unsigned long int address;
	};

} // nl
} // service
} // wns

#endif //_WNS_SERVICE_NL_ADDRESS_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
