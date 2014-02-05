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

#include <WNS/logger/ConsoleFormat.hpp>
#include <iomanip>
#include <cstdlib>

using namespace wns::logger;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	ConsoleFormat,
	FormatStrategy,
	"Console",
	wns::PyConfigViewCreator);

ConsoleFormat::ConsoleFormat(const pyconfig::View& pyco) :
	blackNWhite(false),
	module2color(),
	debugTimeWidth(pyco.get<int>("timeWidth")),
	debugTimePrecision(pyco.get<int>("timePrecision")),
	maxLocationLength(pyco.get<int>("maxLocationLength")),
	currentLocationLength(0)
{
	for(int i=0; i<pyco.len("colorMap"); ++i) {
		pyconfig::View mcView = pyco.getView("colorMap", i);
		std::string name = mcView.get<std::string>("name");
		std::string modifier = mcView.get<std::string>("modifier");
		module2color[name] = modifier;
	}

	if (pyco.get<int>("colors") == 0) { // Off
		blackNWhite = true;
	} else if (pyco.get<int>("colors") == 1) { // Auto
		if(!isatty(1) || !isatty(2)) {
			blackNWhite = true;
		}

		char* p = getenv("TERM");
		if(p && p == std::string("dumb")) {
			blackNWhite = true;
		}
	}
}

std::string ConsoleFormat::highlightModuleName(const std::string& module)
{
	if (blackNWhite) {
		return "";
	}

	std::map<std::string, std::string>::iterator itr = module2color.find(module);
	if(itr == module2color.end()) {
		return std::string("\033[00;00m");
	}
	return itr->second;
}

std::string ConsoleFormat::formatMessage(const RawMessage& m)
{
	long int n;

	std::stringstream formattedMessage;
	formattedMessage << "("
			 << std::resetiosflags(std::ios::fixed)
			 << std::resetiosflags(std::ios::scientific)
			 << std::resetiosflags(std::ios::right)
			 << std::setiosflags(std::ios::right)
			 << std::setiosflags(std::ios::fixed)
			 << std::setiosflags(std::ios::dec)
			 << std::setprecision(debugTimePrecision)
			 << std::setw(debugTimeWidth)
			 << m.time
			 << ") "
			 << highlightModuleName(m.module)
			 << "[" << std::setw(5) << m.module.substr(0, 5) << "] "
			 << m.location << " ";

	if ((m.location.length() > currentLocationLength) && (m.location.length() <= maxLocationLength)) {
		currentLocationLength = m.location.length();
	}
	formattedMessage << std::setw(currentLocationLength - m.location.length()) << "";
	formattedMessage << "   ";

	n = m.message.length();
	formattedMessage << m.message << highlightModuleName("NULL") << "\n";

	// insert spaces after each "\n" to have a nice output
        std::string ret = formattedMessage.str();
	unsigned long int i = 0;
	while ((i = ret.find("\n", i) + 1) < ret.length())
	{
		ret.insert(i, std::string(15 + debugTimeWidth + currentLocationLength, ' '));
	}

	return ret;
}


