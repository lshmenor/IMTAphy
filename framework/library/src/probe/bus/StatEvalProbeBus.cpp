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

#include <WNS/probe/bus/StatEvalProbeBus.hpp>
#include <WNS/container/UntypedRegistry.hpp>
#include <WNS/simulator/ISimulator.hpp>

#include <fstream>

using namespace wns::probe::bus;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    StatEvalProbeBus,
    wns::probe::bus::ProbeBus,
    "StatEvalProbeBus",
    wns::PyConfigViewCreator);

StatEvalProbeBus::StatEvalProbeBus(const wns::pyconfig::View& pyco) :
    outputPath(wns::simulator::getConfiguration().get<std::string>("outputDir")),
    filename(pyco.get<std::string>("outputFilename")),
    appendFlag(pyco.get<bool>("appendFlag")),
    firstWrite(true)
{
    wns::pyconfig::View pycoRoot = wns::simulator::getConfiguration();
    outputPath = pycoRoot.get<std::string>("outputDir");

    statEval = wns::evaluation::statistics::Factory::creator(pyco.get<std::string>("statEval.nameInFactory"))
        ->create(pyco.get("statEval"));
}

StatEvalProbeBus::~StatEvalProbeBus()
{
    delete statEval;
}

bool
StatEvalProbeBus::accepts(const wns::simulator::Time&, const IContext&)
{
    return true;
}

void
StatEvalProbeBus::onMeasurement(const wns::simulator::Time&,
                                const double& aValue,
                                const IContext&)
{
    this->statEval->put(aValue);
}

void
StatEvalProbeBus::output()
{
    if (!appendFlag || firstWrite)
    {
        std::ofstream out((outputPath + "/" + filename).c_str(),
                          std::ios::out);

        statEval->print(out); // header only
        statEval->printLog(out);
        firstWrite = false;
    }
    else
    {
        std::ofstream out((outputPath + "/" + filename).c_str(),
                          std::ios::out | std::ios::app);

        statEval->print(out); // header only
        statEval->printLog(out);
    }
}

