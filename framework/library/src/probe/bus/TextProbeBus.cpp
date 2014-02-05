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

#include <WNS/probe/bus/TextProbeBus.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/scheduler/Interface.hpp>

#include <sstream>
#include <iomanip>

using namespace wns::probe::bus;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    TextProbeBus,
    wns::probe::bus::ProbeBus,
    "TextProbeBus",
    wns::PyConfigViewCreator);


TextProbeBus::TextProbeBus(const wns::pyconfig::View& pyco):
    key_(pyco.get<std::string>("key")),
    outputPath_(wns::simulator::getConfiguration().get<std::string>("outputDir")),
    outfileBase_(pyco.get<std::string>("outputFilename")),
    writeHeader_(pyco.get<bool>("writeHeader")),
    prependSimTimeFlag_(pyco.get<bool>("prependSimTimeFlag")),
    isJSON_(pyco.get<bool>("isJSON")),
    simTimePrecision_(pyco.get<int>("simTimePrecision")),
    simTimeWidth_(pyco.get<int>("simTimeWidth")),
    skipInterval_(pyco.get<int>("skipInterval")),
    numCalls_(0),
    jsonEntrySeparator_("")
{
}

TextProbeBus::~TextProbeBus()
{
    if(isJSON_)
    {
        std::stringstream filename;
        filename << outputPath_ << "/" << outfileBase_ << "_Text.dat";

        // Try to append to existing file
        std::ofstream existingFile(filename.str().c_str(), std::ios::app);
        if (existingFile.good())
        {
            existingFile << "]}" << std::endl;
        }
        existingFile.close();
    }
}

void
TextProbeBus::output()
{
	std::stringstream filename;
	filename << outputPath_ << "/" << outfileBase_ << "_Text.dat";

	// Try to append to existing file
	std::ofstream existingFile(filename.str().c_str(), std::ios::app);
	if (existingFile.good())
	{
		printText(existingFile);
		existingFile.close();
		return;
	}
	existingFile.close();

	// If appending did not work, open a new file
	std::ofstream newFile(filename.str().c_str(), std::ios::out);
	printText(newFile);
	newFile.close();
}


void
TextProbeBus::onMeasurement(const wns::simulator::Time&, const double&, const IContext& reg)
{
	putText(reg.getString(key_).c_str());
}

bool
TextProbeBus::accepts(const wns::simulator::Time&, const IContext& reg)
{
	return reg.knows(key_);
}


void
TextProbeBus::printText(std::ostream& theStream)
{
    bool output_time = true;
    std::string prefix = "#";
    std::string separator(prefix + " -------------------------------------");

    if (writeHeader_)
    {
        if (!isJSON_)
        {
            theStream << prefix + " PROBE TEXT RESULT (THIS IS A MAGIC LINE)"
            << std::endl
            << separator
            << std::endl
            << prefix + " Text file of data, any style "
            << std::endl
            << separator
            << std::endl
            << prefix + "        Name: "
            << name_
            << std::endl
            << prefix + " Description: "
            << description_
            << std::endl
            << separator
            << std::endl;
        }
        else
        {
            theStream << "{ \"content\" : [\n" << std::endl;
        }

        if (!theStream.good())
        {
            throw(wns::Exception("Can't dump ProbeText data file"));
        }
        writeHeader_ = false;
    }

    while (! messages_.empty())
    {
        if (output_time && (!isJSON_))
		{
			time_t cur_time = time(NULL);

			output_time = false;
            std::string time_str = ctime(&cur_time);
			theStream << prefix + " "
					   << time_str;
			if (!theStream.good())
			{
				throw(wns::Exception("Can't dump ProbeText data file"));
			}
		}
        theStream << jsonEntrySeparator_ << std::endl;

		theStream << messages_.front()
				   << std::endl;

        if (isJSON_)
        {
            jsonEntrySeparator_ = ",";
        }

		if (!theStream.good())
		{
			throw(wns::Exception("Can't dump ProbeText data file"));
		}
		messages_.pop_front();
    }
}


void
TextProbeBus::putText(std::string message)
{
    numCalls_++;

    // Ignore this value?
    if (skipInterval_ && ((numCalls_ - 1) % skipInterval_))
    {
        return;
    }

    // Store value for later output
    std::stringstream str;
    if (prependSimTimeFlag_)
    {
        wns::simulator::Time simTime = wns::simulator::getEventScheduler()->getTime();

        str << "("
            << resetiosflags(std::ios::fixed)
            << resetiosflags(std::ios::scientific)
            << resetiosflags(std::ios::right)
            << setiosflags(std::ios::right)
            << setiosflags(std::ios::fixed)
            << setiosflags(std::ios::dec)
            << std::setprecision(simTimePrecision_)
            << std::setw(simTimeWidth_)
            << simTime
            << ")\t"
            << message;
    }
    else
    {
        str << message;
    }
    messages_.push_back(str.str());
}
