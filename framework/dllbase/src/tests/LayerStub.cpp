/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#include <DLL/tests/LayerStub.hpp>
#include <WNS/node/tests/Stub.hpp>

using namespace dll::tests;


LayerStub::LayerStub()
{
    fun_ = new wns::ldk::fun::Main(this);
    node_ = new wns::node::tests::Stub();
}

void
LayerStub::doStartup()
{
}

LayerStub::~LayerStub()
{
    delete fun_;
    delete node_;
}

std::string
LayerStub::getName() const
{
    return "LayerStub";
}

wns::node::Interface*
LayerStub::getNode() const
{
    return node_;
}

wns::ldk::fun::FUN*
LayerStub::getFUN() const
{
    return fun_;
}

void
LayerStub::onNodeCreated()
{
}

void
LayerStub::onWorldCreated()
{
}

void
LayerStub::onShutdown()
{
}

std::string
LayerStub::getNodeName() const
{
    return std::string("None");
}

const dll::ILayer2::AssociationInfoContainer&
LayerStub::getAssociationInfoProvider(const std::string& id) const
{
    return aic_;
}
