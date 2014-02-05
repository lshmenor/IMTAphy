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

#include <WNS/logger/Logger.hpp>
#include <WNS/logger/Master.hpp>
#include <WNS/simulator/ISimulator.hpp>

using namespace wns::logger;

Logger::Logger(
	const std::string& moduleName,
	const std::string& loggerName,
	logger::Master* m) :

	mName(moduleName),
	lName(loggerName),
	masterLogger(m),
	enabled(true),
	level(2),
	showFunction(false)
{
	if(enabled && masterLogger) {
		masterLogger->registerLogger(mName, lName);
	}
} // Logger

Logger::Logger(
	const std::string& moduleName,
	const std::string& loggerName) :

	mName(moduleName),
	lName(loggerName),
	masterLogger(wns::simulator::getMasterLogger()),
	enabled(true),
	level(2),
	showFunction(false)
{
	if (enabled && masterLogger)
	{
		masterLogger->registerLogger(mName, lName);
	}
} // Logger

Logger::Logger(
	const pyconfig::View& pyConfigView,
	logger::Master* m) :

	mName(pyConfigView.get<std::string>("moduleName")),
	lName(pyConfigView.get<std::string>("name")),
	masterLogger(m),
	enabled(pyConfigView.get<bool>("enabled")),
	level(pyConfigView.get<long int>("level")),
	showFunction(pyConfigView.get<bool>("showFunction"))
{
	if(enabled && masterLogger) {
		masterLogger->registerLogger(mName, lName);
	}
} // Logger

Logger::Logger(const pyconfig::View& pyConfigView) :
	mName(pyConfigView.get<std::string>("moduleName")),
	lName(pyConfigView.get<std::string>("name")),
	masterLogger(wns::simulator::getMasterLogger()),
	enabled(pyConfigView.get<bool>("enabled")),
	level(pyConfigView.get<long int>("level")),
	showFunction(pyConfigView.get<bool>("showFunction"))
{
	if (enabled && masterLogger)
	{
		masterLogger->registerLogger(mName, lName);
	}
} // Logger

Logger::Logger() :
	mName("unspecified"),
	lName("unspecified"),
	masterLogger(wns::simulator::getMasterLogger()),
	enabled(false),
	level(0)
{
}

Logger::~Logger()
{
} // ~Logger

Logger::Logger(const Logger& other) :
	mName(other.mName),
	lName(other.lName),
	masterLogger(other.masterLogger), // sharing the same instance
	enabled(other.enabled),
	level(other.level),
	showFunction(other.showFunction)
{
}

Logger&
Logger::operator=(const Logger& other)
{
	mName = other.mName;
	lName = other.lName;
	masterLogger = other.masterLogger; // sharing the same instance
	enabled = other.enabled;
	level = other.level;
	showFunction = other.showFunction;
	return *this;
}

void
Logger::configure(const wns::pyconfig::View& config)
{
	mName = config.get<std::string>("moduleName");
	lName = config.get<std::string>("name");
	enabled = config.get<bool>("enabled");
	level = config.get<long int>("level");
	showFunction = config.get<bool>("showFunction");
} // configure

void
Logger::switchOn()
{
	enabled=true;
} // switchOn

void
Logger::switchOff()
{
	enabled=false;
} // switchOff

long int
Logger::getLevel() const
{
	return level;
}

void
Logger::setLevel(long int _level)
{
	assure(_level >= OFF,"bad level");
	assure(_level < MAXLEVEL,"bad level");
	level = _level;
}

const std::string&
Logger::getModuleName() const
{
	return mName;
}

const std::string&
Logger::getLoggerName() const
{
	return lName;
}

void
Logger::setLoggerName(const std::string& name)
{
	lName = name;
}

Master*
Logger::getMaster() const
{
	return this->masterLogger;
}

#ifndef WNS_NO_LOGGING

void
Logger::send(const Message& m) const
{
	if( enabled && masterLogger && (m.getLevel() <= this->level) )
	{
		masterLogger->write(mName, lName, m.getString());
	}
}

#else

void
Logger::send(const Message&) const
{
}

#endif

bool
Logger::isShowFunction() const
{
	return showFunction;

}


