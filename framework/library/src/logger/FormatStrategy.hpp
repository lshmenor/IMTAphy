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

#ifndef WNS_LOGGER_FORMATSTRATEGY_HPP
#define WNS_LOGGER_FORMATSTRATEGY_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/simulator/Time.hpp>

#include <string>

namespace wns { namespace logger {

	struct RawMessage
	{
		RawMessage() :
			time(),
			module(),
			location(),
			message()
		{
		}

		wns::simulator::Time time;
		std::string module;
		std::string location;
		std::string message;
	}; // RawMessage

	class FormatStrategy
	{
	public:
		virtual std::string
		formatMessage(const RawMessage& m) =0;

		virtual std::string
		formatRegistration(const std::string& aModuleRef,
				   const std::string& aLocationRef) =0;

		virtual
		~FormatStrategy();
	}; // FormatStrategy

	typedef PyConfigViewCreator<FormatStrategy, FormatStrategy> FormatStrategyCreator;
	typedef StaticFactory<FormatStrategyCreator> FormatStrategyFactory;

} // logger
} // wns
#endif // NOT defined WNS_LOGGER_FORMATSTRATEGY_HPP


