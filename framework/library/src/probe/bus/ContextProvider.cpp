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

#include <WNS/probe/bus/ContextProvider.hpp>

using namespace wns::probe::bus;
using namespace wns::probe::bus::contextprovider;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Constant,
	IContextProvider,
	"wns.ProbeBus.ConstantContextProvider",
	wns::PyConfigViewCreator);

Constant::Constant(const std::string& key, int value) :
	key_(key),
	value_(value)
{}


Constant::Constant(const pyconfig::View& config) :
	key_(config.get<std::string>("key")),
	value_(config.get<int>("value"))
{}


void
Constant::doVisit(IContext& c) const
{
    c.insertInt(key_, value_);
}

Variable::Variable(const std::string& key, int value) :
	key_(key),
	value_(value)
{}

void
Variable::doVisit(IContext& c) const
{
    c.insertInt(key_, value_);
}

void
Variable::set(int value)
{
	value_ = value;
}

int
Variable::get() const
{
	return value_;
}

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
