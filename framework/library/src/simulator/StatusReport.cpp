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

#include <WNS/simulator/StatusReport.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/Assure.hpp>
#include <WNS/osi/PDU.hpp>

#include <iomanip>
#include <fstream>

using namespace wns::simulator;

StatusReport::StatusReport() :
	wns::events::PeriodicRealTimeout(),
	maxSimTime(0.0),
	startTime(time(NULL)),
	outputDir("output/"),
	statusFileName("output/WNSStatus.dat"),
	progressFileName("output/progress")
{
}

void StatusReport::start(const pyconfig::View& _pyConfigView)
{
	this->maxSimTime   = _pyConfigView.get<double>("maxSimTime");
	this->startTime    = time(NULL);

	this->outputDir = _pyConfigView.get<std::string>("outputDir");

	// Append trailing '/'?
	std::string::size_type len = this->outputDir.length();
	if (this->outputDir.find("/",len-1) == std::string::npos)
	{
		this->outputDir += "/";
	}

	this->statusFileName = this->outputDir + _pyConfigView.get<std::string>("statusFileName");
	this->progressFileName = this->outputDir + "progress";

	// try to write status file
	std::ofstream statusFile;
	statusFile.open(this->statusFileName.c_str());
	if (statusFile.good() == false)
	{
		wns::Exception e;
		e << "Can't open file " << this->statusFileName;
		throw e;
	}
	statusFile.close();

	// try to write progress file
	std::ofstream progressFile;
	progressFile.open(this->progressFileName.c_str());
	if (progressFile.good() == false)
	{
		wns::Exception e;
		e << "Can't open file " << this->progressFileName;
		throw e;
	}
	progressFile.close();

    // register memConsumption Probe
    memoryConsumption = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(),
                                              _pyConfigView.get<std::string>("memConsumptionProbeBusName")));

    // register simTime Probe
    simTimePerRealTime = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(),
                                              _pyConfigView.get<std::string>("simTimeProbeBusName")));

	this->writeStatus(false);
	this->startPeriodicTimeout(_pyConfigView.get<double>("statusWriteInterval"));
}

void StatusReport::stop()
{
	if (this->hasPeriodicRealTimeoutSet() == true)
	{
		this->cancelPeriodicRealTimeout();
	}
}

void StatusReport::periodically()
{
        try
	{
                this->writeStatus(false);
        }
        catch(WriteError& we)
	{
                std::cerr << we;
        }
}

void StatusReport::writeStatus(bool anEndOfSimFlag, std::string otherFileName)
{
 	std::ofstream statusFile;
	if (otherFileName == "")
	{
		statusFile.open(this->statusFileName.c_str());
		if (statusFile.good() == false)
		{
			throw WriteError(this->statusFileName);
		}
	}
	else
	{
		statusFile.open((outputDir + otherFileName).c_str());
		if (statusFile.good() == false)
		{
			throw WriteError(outputDir + otherFileName);
		}
	}

	std::ofstream progressFile;
	progressFile.open(this->progressFileName.c_str());
	if (progressFile.good() == false)
	{
		throw WriteError(this->progressFileName);
	}


	std::string startTimeStr, curTimeStr;
	time_t curTime;
	time_t hours, minutes, seconds;

	startTimeStr = ctime(&startTime);
	curTime = time(NULL);
	curTimeStr = ctime(&curTime);


	// @bug We've seen this assertion fail on SMP systems with dynamic
	// frequency scaling enabled (as power save strategy).
	// disabled: msg, 25.11.06
	// assure(curTime >= startTime, "\ncurTime: " + curTimeStr + "startTime: " + startTimeStr);

	//calculate running time
	hours   = (curTime - startTime) / 3600;
	minutes = (curTime - startTime - 3600 * hours) / 60;
	seconds = curTime - startTime - 3600 * hours - minutes * 60;

	double curSimTime =  wns::simulator::getEventScheduler()->getTime();

	statusFile << "        WNS" << std::endl
		   << "       =====" << std::endl
		   << "                 Status: "
		   << (anEndOfSimFlag ?
		       "Simulation terminated successfully" :
		       "Simulation is still running")
		   << std::endl
		   << "                     Start: "
		   << startTimeStr
		   << "                       Now: "
		   << curTimeStr
		   << "                  Duration:          "
		   << std::setw(4)
		   << hours
		   << ":"
		   << std::setw(2)
		   << std::setfill('0')
		   << minutes
		   << ":"
		   << std::setw(2)
		   << std::setfill('0')
		   << seconds
		   << std::endl
		   << std::endl
		   << "              Simulation time: "
		   << curSimTime
		   << std::endl
		   << "         Max. simulation time: "
		   << maxSimTime
		   << std::endl
		   << std::endl
		   << std::endl
#ifndef NDEBUG
		   << "Debug output\n"
		   << "----------------------------------------------------------\n"
		   << "Max number of PDUs:     " << wns::osi::PDU::getMaxExistingPDUs() << "\n"
		   << "Current number of PDUs: " << wns::osi::PDU::getExistingPDUs() << "\n"
#endif // NDEBUG
		   << std::endl
		   << std::endl
		   << "The following output is read from /proc/self/status\n"
		   << "----------------------------------------------------------\n";
	std::ifstream in("/proc/self/status");
	std::string line;
	while(getline(in, line) != NULL)
	{
		statusFile << line << std::endl;
	}

	this->probe();

	if(in.eof() == false)
	{
		statusFile << "WARNING: /proc/self/status not read until EOF!!" << std::endl;
	}
	statusFile.close();

	// calculate and write progress:
	double progress = 0.0;
 	if (this->maxSimTime > 0.0)
	{
		progress = curSimTime / this->maxSimTime;
	}
	progressFile << progress;
	progressFile.close();
}


void
StatusReport::probe()
{
    if (memoryConsumption != NULL)
    {
		std::ifstream in( "/proc/self/statm");
		unsigned long int sizeInPages(0);
		in >> sizeInPages;
		in.close();
		unsigned long int pagesize = getpagesize(); // value from /proc/self/statm is in "pages"
		unsigned long int vmemUsage = pagesize * sizeInPages / 1024; // express in kB
 		memoryConsumption->put(vmemUsage);
	}

    if (simTimePerRealTime != NULL)
    {
        double curSimTime =  wns::simulator::getEventScheduler()->getTime();
        time_t curTime = time(NULL);                
        double deltaSeconds = double(curTime - startTime);
        if(deltaSeconds > 0.0)
            simTimePerRealTime->put(curSimTime / deltaSeconds);
    }

}
