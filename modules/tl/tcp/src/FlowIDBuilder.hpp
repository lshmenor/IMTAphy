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

#ifndef TCP_FLOWIDBUILDER_HPP
#define TCP_FLOWIDBUILDER_HPP

#include <TCP/UpperConvergence.hpp>

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/Service.hpp>

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Key.hpp>


#include <sstream>

namespace tcp {

    class FlowIDKey :
        public wns::ldk::Key
    {
    public:
        FlowIDKey(const wns::service::tl::FlowID&);

        virtual bool
        operator<(const Key& other) const;

        virtual std::string
        str() const;

        virtual
        ~FlowIDKey()
        {}

        wns::service::tl::FlowID flowID_;
    };

    typedef wns::SmartPtr<FlowIDKey> FlowIDKeyPtr;
    typedef wns::SmartPtr<const FlowIDKey> ConstFlowIDKeyPtr;

	class FlowIDBuilder :
		public wns::ldk::KeyBuilder
	{
	public:
		FlowIDBuilder(
			wns::ldk::fun::FUN* _fun,
			const wns::pyconfig::View& _config);

		virtual
		~FlowIDBuilder(){}

		virtual
		void onFUNCreated();

		virtual wns::ldk::ConstKeyPtr
		operator () (const wns::ldk::CompoundPtr& /*compound*/, int /*direction*/) const;

	private:
		wns::pyconfig::View config;

		const wns::ldk::fun::FUN* fun;

        wns::ldk::CommandReaderInterface* tcpHeaderReader;

	};

} // namespace tcp

#endif // NOT defined TCP_FLOWID_HPP
