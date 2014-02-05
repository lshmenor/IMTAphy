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

#include <WNS/node/Interface.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/pyconfig/View.hpp>

using namespace wns::node::component;

Component::Component(
	wns::node::Interface* _node,
	const pyconfig::View& _pyco) :

	node(_node),
	name(_pyco.get<std::string>("name")),
	pyco(_pyco)
{
	assure(node, "Node may not be NULL");
}

Component::~Component()
{
}

wns::node::Interface*
Component::getNode() const
{
	assure(node, "Node may not be NULL");
	return node;
}

std::string
Component::getName() const
{
	return name;
}

const wns::pyconfig::View&
Component::getConfig() const
{
	return pyco;
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
