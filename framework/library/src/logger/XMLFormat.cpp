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

#include <WNS/logger/XMLFormat.hpp>

using namespace wns::logger;

STATIC_FACTORY_REGISTER_WITH_CREATOR(XMLFormat, FormatStrategy, "wns.logger.XMLFormat", wns::PyConfigViewCreator);

XMLFormat::XMLFormat(const pyconfig::View&)
{}

std::string XMLFormat::formatMessage(const RawMessage& m)
{
	std::stringstream s;
	s << "<message>" << std::endl
	  << "  <time>" << m.time << "</time>" << std::endl
	  << "  <module>" << m.module << "</module>" << std::endl
	  << "  <location>" << m.location << "</location>" << std::endl
	  << "  <text>" << m.message << "</text>" << std::endl
	  << "</message>" << std::endl;

	return s.str();
}

std::string XMLFormat::formatRegistration(const std::string& aModuleRef,
					  const std::string& aLocationRef)
{
	std::stringstream s;
	s << "<registerLogger>" << std::endl
	  << "  <module>" << aModuleRef << "</module>" << std::endl
	  << "  <location>" << aLocationRef << "</location>" << std::endl
	  << "</registerLogger>" << std::endl;
	return s.str();
}


