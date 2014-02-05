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

#ifndef WNS_SERVICE_TL_FLOWID_HPP
#define WNS_SERVICE_TL_FLOWID_HPP

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/Service.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/IOutputStreamable.hpp>

#include <sstream>

namespace wns { namespace service { namespace tl {
	class FlowID :
		public wns::IOutputStreamable
	{
	public:
		FlowID() :
			srcAddress(),
			srcPort(-1),
			dstAddress(),
			dstPort(-1)
		{
		}

		FlowID(
			const wns::service::nl::Address& _srcAddress,
			wns::service::tl::Port _srcPort,
			const wns::service::nl::Address& _dstAddress,
			wns::service::tl::Port _dstPort) :
			srcAddress(_srcAddress),
			srcPort(_srcPort),
			dstAddress(_dstAddress),
			dstPort(_dstPort)
		{}

		virtual ~FlowID()
		{}

		virtual void
		swapSrcDest()
			{
				wns::service::nl::Address tmpSrcAddress = srcAddress;
				wns::service::tl::Port tmpSrcPort = srcPort;
				srcAddress = dstAddress;
				srcPort = dstPort;
				dstAddress = tmpSrcAddress;
				dstPort = tmpSrcPort;
			}

		virtual bool
		operator ==(const FlowID& other) const
		{
			if(srcAddress == other.srcAddress &&
			   srcPort == other.srcPort &&
			   dstAddress == other.dstAddress &&
			   dstPort == other.dstPort) {
				return true;
			} else {
				return false;
			}
		}

		bool
		operator <(const FlowID& other) const
		{
			if(srcAddress < other.srcAddress) {
				return true;
			} else if (srcAddress == other.srcAddress) {
				if(srcPort < other.srcPort) {
					return true;
				} else if(srcPort == other.srcPort) {
					if(dstAddress < other.dstAddress) {
						return true;
					} else if(dstAddress == other.dstAddress) {
						if(dstPort < other.dstPort) {
							return true;
						}
					}
				}
			}
			return false;
		}

		virtual std::string
		doToString() const
		{
			std::stringstream ss;
			ss << "SRC: " << srcAddress << ":" << srcPort << ", "
			   << "DST: " << dstAddress << ":" << dstPort;
			return ss.str();
		}

		wns::service::nl::Address srcAddress;
		wns::service::tl::Port srcPort;

		wns::service::nl::Address dstAddress;
		wns::service::tl::Port dstPort;
	};
} // tl
} // service
} // wns

#endif // NOT defined WNS_SERVICE_TL_FLOWID_HPP

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
