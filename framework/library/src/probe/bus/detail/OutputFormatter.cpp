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

#include <WNS/probe/bus/detail/OutputFormatter.hpp>

using namespace wns::probe::bus::detail;

STATIC_FACTORY_REGISTER_WITH_CREATOR( HumanReadable, OutputFormatter, 
        "HumanReadable", StatEvalTableCreator );
STATIC_FACTORY_REGISTER_WITH_CREATOR( PythonReadable, OutputFormatter, 
        "PythonReadable", StatEvalTableCreator );
STATIC_FACTORY_REGISTER_WITH_CREATOR( MatlabReadable, OutputFormatter, 
        "MatlabReadable", StatEvalTableCreator );
STATIC_FACTORY_REGISTER_WITH_CREATOR( MatlabReadableSparse, OutputFormatter, 
        "MatlabReadableSparse", StatEvalTableCreator );
STATIC_FACTORY_REGISTER_WITH_CREATOR( LogEvalReadable, OutputFormatter, 
        "LogEvalReadable", StatEvalTableCreator );
STATIC_FACTORY_REGISTER_WITH_CREATOR( Plain, OutputFormatter, 
        "Plain", StatEvalTableCreator );

void
OutputFormatter::print(std::ostream& strm,
                       std::string valueType) const
{
    int dim = sorters().size();
    std::list<int> empty;
    this->doPrint(strm, empty, dim, valueType);
}


void
HumanReadable::doPrint(std::ostream& strm,
                       std::list<int> fixedIndices,
                       int dim,
                       std::string valueType) const
{
    size_t level = fixedIndices.size();
    assure(dim + level == sorters().size(), "dim/level mismatch");

    if (dim == 0)
    {
        // End of recursion, print the actual value
        strm << data.getByIndex(fixedIndices).get(valueType);
    }
    else if (dim == 1)
    {
        // Print the data for this line
        // Add last index
        fixedIndices.push_back(0);
        for (int ii = 0; ii< sorters().at(level).getResolution(); ++ii)
        {
            fixedIndices.back() = ii;
            strm << "\t";
            doPrint(strm, fixedIndices, dim-1, valueType);
            strm << "\t|";
        }
        return;
    }
    else if (dim == 2)
    {
        // Print the table headings. Note that the intervals are defined by the
        // next level
        strm << "\t|";
        for (int ii = 0; ii< sorters().at(level+1).getResolution(); ++ii)
        {
            strm << "\t" << sorters().at(level+1).getInterval(ii) << "\t|";
        }
        strm << "\n";
        // Add next index
        fixedIndices.push_back(0);
        for (int ii = 0; ii< sorters().at(level).getResolution(); ++ii)
        {
            fixedIndices.back() = ii;
            // Precede each line with the interval it represents
            strm << "\n" << sorters().at(level).getInterval(ii) << "\t|";
            doPrint(strm, fixedIndices, dim-1, valueType);
        }
        return;
    }
    else
    {
        // Print the dimension headings
        // Add next index
        fixedIndices.push_back(0);
        for (int ii = 0; ii< sorters().at(level).getResolution(); ++ii)
        {
            strm << "\n\nDimension: " << dim
                 << ", index: " << ii
                 << ", " << sorters().at(level).getInterval(ii) << "\n";
            fixedIndices.back() = ii;
            doPrint(strm, fixedIndices, dim-1, valueType);
        }
        return;
    }
}


void
PythonReadable::print(std::ostream& strm,
                      std::string valueType) const
{
    strm << "returnValue = ";
    OutputFormatter::print(strm, valueType);
    // "deletes" the last 2 chars to get rid of trailing commas
    strm.seekp(-2, std::ios_base::cur);
    strm << "\n";
}


void
PythonReadable::doPrint(std::ostream& strm,
                        std::list<int> fixedIndices,
                        int dim,
                        std::string valueType) const
{
    size_t level = fixedIndices.size();
    assure(dim + level == sorters().size(), "dim/level mismatch");

    if (dim == 0)
    {
        // End of recursion, print the actual value
        strm << data.getByIndex(fixedIndices).get(valueType);
    }
    else if (dim == 1)
    {
        // Print the data for this line
        // Add last index
        fixedIndices.push_back(0);
        strm << "[ ";
        for (int ii = 0; ii< sorters().at(level).getResolution(); ++ii)
        {
            fixedIndices.back() = ii;
            doPrint(strm, fixedIndices, dim-1, valueType);
            strm << ", ";
        }
        // "deletes" the last 2 chars to get rid of trailing commas
        strm.seekp(-2, std::ios_base::cur);
        strm << "],\n";
        return;
    }
    else if (dim == 2)
    {
        // Print the table headings. Note that the intervals are defined by the
        // next level
        strm << getPrefix();
        for (int ii = 0; ii< sorters().at(level+1).getResolution(); ++ii)
        {
            strm << sorters().at(level+1).getInterval(ii) << "  ";
        }
        strm << "\n";
        strm << "[\n";
        // Add next index
        fixedIndices.push_back(0);
        for (int ii = 0; ii< sorters().at(level).getResolution(); ++ii)
        {
            fixedIndices.back() = ii;
            // Precede each line with the interval it represents
            strm << getPrefix() << sorters().at(level).getInterval(ii) << ":\n";
            doPrint(strm, fixedIndices, dim-1, valueType);
        }
        // "deletes" the last 2 chars to get rid of trailing commas
        strm.seekp(-2, std::ios_base::cur);
        strm << "\n],\n";
        return;
    }
    else
    {
        // Print the dimension headings
        // Add next index
        fixedIndices.push_back(0);
        strm << "[\n";
        for (int ii = 0; ii< sorters().at(level).getResolution(); ++ii)
        {
            strm << "\n\n" << getPrefix() << "Dimension: " << dim
                 << ", index: " << ii
                 << ", " << sorters().at(level).getInterval(ii) << "\n";
            fixedIndices.back() = ii;
            doPrint(strm, fixedIndices, dim-1, valueType);
        }
        // "deletes" the last 2 chars to get rid of trailing commas
        strm.seekp(-2, std::ios_base::cur);
        strm << "\n],\n";
        return;
    }
}


void
MatlabReadable::print(std::ostream& strm,
                      std::string valueType) const
{
    strm << getPrefix() << "\n";
    size_t ii = 0;
    for (; ii<sorters().size(); ++ii)
    {
        strm << getPrefix() << ii+1 << ". Column: " << sorters().at(ii).getIdName() << "\n";
    }
    strm << getPrefix() << ii+1 << ". Column: value\n\n";
    OutputFormatter::print(strm, valueType);
}


void
MatlabReadable::doPrint(std::ostream& strm,
                        std::list<int> fixedIndices,
                        int dim,
                        std::string valueType) const
{
    size_t level = fixedIndices.size();
    assure(dim + level == sorters().size(), "dim/level mismatch");

    if (dim == 0)
    {
        // Print this entry with all its indices
        int counter = 0;
        std::list<int>::const_iterator index = fixedIndices.begin();
        std::list<int>::const_iterator end   = fixedIndices.end();
        for (; index != end; ++index)
        {
            strm << sorters().at(counter).getMin(*index) << "\t";
            ++counter;
        }
        strm << data.getByIndex(fixedIndices).get(valueType) << "\n";
        return;
    }
    else
    {
        // Add last index
        fixedIndices.push_back(0);
        for (int ii = 0; ii< sorters().at(level).getResolution(); ++ii)
        {
            fixedIndices.back() = ii;
            doPrint(strm, fixedIndices, dim-1, valueType);
        }
        return;
    }
}

void
MatlabReadableSparse::doPrint(std::ostream& strm,
                              std::list<int> fixedIndices,
                              int dim,
                              std::string valueType) const
{
    size_t level = fixedIndices.size();
    assure(dim + level == sorters().size(), "dim/level mismatch");

    if (dim == 0)
    {
        if(data.getByIndex(fixedIndices).get(valueType) != 0)
        {
            // Print this entry with all its indices
            int counter = 0;
            std::list<int>::const_iterator index = fixedIndices.begin();
            std::list<int>::const_iterator end   = fixedIndices.end();
            for (; index != end; ++index)
            {
                strm << sorters().at(counter).getMin(*index) << "\t";
                ++counter;
            }
            strm << data.getByIndex(fixedIndices).get(valueType) << "\n";
        }
        return;
    }
    else
    {
        // Add last index
        fixedIndices.push_back(0);
        for (int ii = 0; ii< sorters().at(level).getResolution(); ++ii)
        {
            fixedIndices.back() = ii;
            doPrint(strm, fixedIndices, dim-1, valueType);
        }
        return;
    }
}

void
LogEvalReadable::doPrint(std::ostream& strm,
                              std::list<int> fixedIndices,
                              int dim,
                              std::string valueType) const
{
    size_t level = fixedIndices.size();
    assure(dim + level == sorters().size(), "dim/level mismatch");

    if (dim == 0)
    {
        // Print this entry with all its indices
        int counter = 0;
        std::list<int>::const_iterator index = fixedIndices.begin();
        std::list<int>::const_iterator end   = fixedIndices.end();
        for (; index != end; ++index)
        {
            strm << sorters().at(counter).getMin(*index) << "\t";
            ++counter;
        }
        if((valueType == "min" || valueType == "max") 
            && data.getByIndex(fixedIndices).get("trials") == 0)
        {
            strm << 0.0 << "\n";
        }
        else
        {
            strm << data.getByIndex(fixedIndices).get(valueType) << "\n";
        }
        return;
    }
    else
    {
        // Add last index
        fixedIndices.push_back(0);
        for (int ii = 0; ii< sorters().at(level).getResolution(); ++ii)
        {
            fixedIndices.back() = ii;
            doPrint(strm, fixedIndices, dim-1, valueType);
        }
        return;
    }
}
