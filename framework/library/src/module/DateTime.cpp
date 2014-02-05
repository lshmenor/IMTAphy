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

// Dependencies

#include <WNS/module/DateTime.hpp>
#include <WNS/Exception.hpp>

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;
using namespace wns::module;

// Constructor

// default constructor
DateTime::DateTime()
  : time(0), empty(true)
{
}

// time_t constructor
DateTime::DateTime(const time_t t)
  : time(t), empty(false)
{
	if(t == 0) empty=true;
}

// Operators

bool DateTime::operator==(const DateTime b) const
{
  return empty || b.empty || (time == b.time);
}

bool DateTime::operator!=(const DateTime b) const
{
  return empty || b.empty || (time != b.time);
}

bool DateTime::operator>(const DateTime b) const
{
  return empty || b.empty || (time > b.time);
}

bool DateTime::operator<(const DateTime b) const
{
  return empty || b.empty || (time < b.time);
}

bool DateTime::operator>=(const DateTime b) const
{
  return empty || b.empty || (time >= b.time);
}

bool DateTime::operator<=(const DateTime b) const
{
  return empty || b.empty || (time <= b.time);
}

// Accessors
time_t DateTime::getTime() const
{
  return time;
}

string DateTime::getString() const
{
  if(empty)
    return "";

  struct tm tm;
  if(NULL == localtime_r(&time, &tm)) {
              // i can hardly think of any error causing this function call
              // to fail.
    throw Exception("Please report this exception to osz@illator.de.");
  }

  std::stringstream ss;
  ss << 1900 + tm.tm_year;
  ss << "-" << std::setfill('0') << std::setw(2) << 1 + tm.tm_mon;
  ss << "-" << std::setfill('0') << std::setw(2) << tm.tm_mday;
  ss << " " << std::setfill('0') << std::setw(2) << tm.tm_hour;
  ss << ":" << std::setfill('0') << std::setw(2) << tm.tm_min;
  ss << ":" << std::setfill('0') << std::setw(2) << tm.tm_sec;

  return ss.str();
}

bool DateTime::isEmpty() const
{
  return empty;
}

