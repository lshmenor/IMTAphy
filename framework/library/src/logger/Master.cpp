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

#include <WNS/logger/Master.hpp>
#include <WNS/Assure.hpp>

#include <iomanip>

using namespace wns::logger;

Master::Master() :
	doLogging(true),
	haveBacktrace(false),
	numberOfLinesForBacktrace(10000),
	backtrace()
{
}

Master::Master(const pyconfig::View& pyco) :
	doLogging(true),
	haveBacktrace(false),
	numberOfLinesForBacktrace(10000),
	backtrace()
{
	this->configure(pyco);
}


Master::~Master()
{
	this->clearLoggerChain();
}

void
Master::configure(const pyconfig::View& pyco)
{
	this->doLogging = pyco.get<bool>("enabled");
	this->haveBacktrace = pyco.get<bool>("backtrace.enabled");
	this->numberOfLinesForBacktrace = pyco.get<size_t>("backtrace.length");

	this->clearLoggerChain();

	for (int ii = 0; ii < pyco.len("loggerChain"); ++ii)
	{
		pyconfig::View loggerView = pyco.getView("loggerChain", ii);
		std::string formatStrategy = loggerView.get<std::string>("format.__plugin__");
		FormatStrategyCreator* fsc = FormatStrategyFactory::creator(formatStrategy);

		std::string outputStrategy = loggerView.get<std::string>("output.__plugin__");
		OutputStrategyCreator* osc = OutputStrategyFactory::creator(outputStrategy);

		loggerChain[osc->create()] = fsc->create(loggerView.getView("format"));
	}
}

void
Master::registerLogger(const std::string& aModuleRef,
		       const std::string& aLocationRef)
{
	for (LoggerChain::iterator itr = loggerChain.begin();
	     itr != loggerChain.end();
	     ++itr)
	{
		*(itr->first) << itr->second->formatRegistration(aModuleRef, aLocationRef);
	}
}

void Master::saveForBacktrace(const RawMessage& m)
{
	backtrace.push_back(m);
	if (backtrace.size() > numberOfLinesForBacktrace)
	{
		backtrace.pop_front();
	}
}

void Master::outputBacktrace() const
{
        for(std::deque<RawMessage>::const_iterator itr = backtrace.begin();
	    itr != backtrace.end();
	    ++itr)
	{
                outputMessage(*itr);
        }
}


bool Master::isEnabled() const
{
	return this->doLogging;
}


void Master::outputMessage(const RawMessage& m) const
{
	for(LoggerChain::const_iterator itr = loggerChain.begin();
	    itr != loggerChain.end();
	    ++itr)
	{
		*(itr->first) << itr->second->formatMessage(m);
	}
}

void Master::clearLoggerChain()
{
	while(!loggerChain.empty())
	{
		LoggerChain::iterator i = loggerChain.begin();
		delete i->first;
		delete i->second;
		loggerChain.erase(i);
	}
}


