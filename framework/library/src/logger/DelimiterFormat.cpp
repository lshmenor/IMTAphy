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

#include <WNS/logger/DelimiterFormat.hpp>

using namespace wns::logger;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	DelimiterFormat,
	FormatStrategy,
	"wns.logger.DelimiterFormat",
	wns::PyConfigViewCreator);

DelimiterFormat::DelimiterFormat(const pyconfig::View& pyco) :
	delimiter(pyco.get<std::string>("delimiter"))
{}

std::string
DelimiterFormat::formatMessage(const RawMessage& m)
{
	std::stringstream s;
	s << m.time << this->delimiter
	  << m.module << this->delimiter
	  << m.location << this->delimiter
	  << this->escapeBackslash(m.message) << std::endl;
	return s.str();
}

std::string
DelimiterFormat::escapeBackslash(const std::string& s)
{
	std::string res;
	for(size_t ii = 0; ii < s.length(); ++ii)
	{
		std::string character(1, s[ii]);
		if(character == "\n")
		{
			res.append("\\n");
		}
		else
		{
			res.append(character);
		}
	}
	return res;
}


