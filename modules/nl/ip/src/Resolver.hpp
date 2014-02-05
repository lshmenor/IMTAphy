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

#ifndef IP_RESOLVER_HPP
#define IP_RESOLVER_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/PyConfigViewCreator.hpp>

namespace ip { namespace resolver {

	class ResolverInterface
	{
	public:
	  virtual
	  ~ResolverInterface() {};

	  virtual void
	  resolve() = 0;

	  virtual wns::service::nl::Address
	  getAddress() = 0;

	  virtual wns::service::nl::Address
	  getSubnetMask() = 0;
	};

	typedef wns::PyConfigViewCreator<ResolverInterface, ResolverInterface> Creator;

	typedef	wns::StaticFactory<resolver::Creator> Factory;

    /**
     * @brief Resolves to a static address
     *
     * \pyco{ip.AddressResolver.FixedAddressResolver}
     */
	class FixedAddressResolver:
		public ResolverInterface
	{

	public:
	  FixedAddressResolver(const wns::pyconfig::View& _pyco);

	  virtual void
	  resolve();

	  virtual wns::service::nl::Address
	  getAddress();

	  virtual wns::service::nl::Address
	  getSubnetMask();

	private:
	  wns::service::nl::Address address;
	  wns::service::nl::Address subnetMask;

	};

    /**
     * @brief Uses DHCP to resolve addresses
     *
     * \pyco{ip.AddressResolver.VirtualDHCPResolver}
     */
	class VirtualDHCPResolver:
		public ResolverInterface
	{

	public:
	  VirtualDHCPResolver(const wns::pyconfig::View& _pyco);

	  virtual void
	  resolve();

	  virtual wns::service::nl::Address
	  getAddress();

	  virtual wns::service::nl::Address
	  getSubnetMask();

	private:
	  wns::service::nl::Address address;
	  wns::service::nl::Address subnetMask;

	  std::string zone;
	};

}// resolver
}// ip
#endif // IP_RESOLVER_HPP
