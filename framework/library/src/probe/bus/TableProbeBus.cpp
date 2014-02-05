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

#include <WNS/probe/bus/TableProbeBus.hpp>
#include <WNS/probe/bus/detail/OutputFormatter.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <sstream>
#include <fstream>

using namespace wns::probe::bus;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    TableProbeBus,
    wns::probe::bus::ProbeBus,
    "TableProbeBus",
    wns::PyConfigViewCreator);



TableProbeBus::TableProbeBus(const wns::pyconfig::View& _pyco):

	sorters(),
	evals(),
	formats(),
    outputPath(wns::simulator::getConfiguration().get<std::string>("outputDir")),
	outfileBase(_pyco.get<std::string>("outputFilename")),
    t(NULL)
{
 	for (int ii = 0; ii<_pyco.len("axisParams"); ++ii)
	{
		sorters.push_back( detail::Sorter(_pyco.get("axisParams",ii)) );
	}

 	for (int ii = 0; ii<_pyco.len("evals"); ++ii)
	{
		evals.push_back( _pyco.get<std::string>("evals",ii) );
	}

 	for (int ii = 0; ii<_pyco.len("formats"); ++ii)
	{
		formats.push_back( _pyco.get<std::string>("formats",ii) );
	}

	t = new detail::StatEvalTable(sorters);
}

TableProbeBus::~TableProbeBus()
{
	delete t;
}

void
TableProbeBus::output()
{
	for (size_t ff = 0; ff<formats.size(); ++ff)
	{
		std::string format = formats.at(ff);

		// Create suitable OutputFormatter from factory
		detail::OutputFormatter* of = detail::FormatterFactory::creator(format)->create(*(this->t));

		for (size_t ee = 0; ee<evals.size(); ++ee)
		{
			std::string valueType = evals.at(ee);

			std::ofstream out((outputPath + "/" + outfileBase + "_" + valueType + of->getFilenameSuffix()).c_str(),
							  std::ios::out);
			out << of->getPrefix() << "This table contains the " << valueType << " of: " << outfileBase << "\n";
            out << of->getPrefix() << "\n";
			out << of->getPrefix() << "Dimensions:\n";
            out << of->getPrefix() << "\n";
			for (size_t ii = 0; ii<sorters.size(); ++ii)
			{
				out << of->getPrefix() << "Dim " << ii + 1 << ": '" << sorters.at(ii).getIdName() << "'\n";
			}
			of->print(out, valueType);
			out.close();
		}

		// OutputFormatter is no longer used
		delete of;
	}
}


void
TableProbeBus::onMeasurement(const wns::simulator::Time&, const double& value, const IContext& reg)
{
	std::list<detail::IDType> ids;
	for (size_t ii = 0; ii<sorters.size(); ++ii)
	{
		ids.push_back(reg.getInt(sorters.at(ii).getIdName()));
	}
	t->get(ids).put(value);
}

bool
TableProbeBus::accepts(const wns::simulator::Time&, const IContext& reg)
{
	for (size_t ii = 0; ii<sorters.size(); ++ii)
	{
		assure(reg.knows(sorters.at(ii).getIdName()), "could not find idName: "<<sorters.at(ii).getIdName());
		if (sorters.at(ii).checkIndex(reg.getInt(sorters.at(ii).getIdName())) == false)
		{
			return false;
		}
	}
	return true;
}
