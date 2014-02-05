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

#include <WNS/node/component/Component.hpp>
#include <WNS/node/component/tests/ComponentStub.hpp>

#include <WNS/pyconfig/helper/Functions.hpp>

using namespace wns::node::component::tests;

ComponentStub::ComponentStub(node::Interface* _node, const std::string& _name) :
	node(_node),
	name(_name),
	calledStartup(0)
{
}

void
ComponentStub::doStartup()
{
	++calledStartup;
}

ComponentStub::~ComponentStub()
{
}

void
ComponentStub::onNodeCreated()
{
}

void
ComponentStub::onWorldCreated()
{
}

void
ComponentStub::onShutdown()
{
}

int
ComponentStub::getCalledStartup() const
{
	return calledStartup;
}

std::string
ComponentStub::getName() const
{
	return name;
}

wns::node::Interface*
ComponentStub::getNode() const
{
	return node;
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
