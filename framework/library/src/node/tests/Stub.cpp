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

#include <WNS/node/tests/Stub.hpp>

using namespace wns::node::tests;

unsigned int Stub::idCounter = 0;

Stub::Stub() :
	contextProviderRegistry()
{
	id = idCounter++;
}

Stub::~Stub()
{
}

void
Stub::addService(const std::string&, service::Service*)
{
	//	assure(false, "Don't call node::tests::Stub::addService");
}

void
Stub::startup()
{
}

void
Stub::onWorldCreated()
{
    assure(false, "Don't call node::tests::Stub::onWorldCreated");
}

void
Stub::onShutdown()
{
	assure(false, "Don't call node::tests::Stub::onShutdown");
}

wns::probe::bus::ContextProviderCollection&
Stub::getContextProviderCollection()
{
	return contextProviderRegistry;
}

std::string
Stub::getName() const
{
//	assure(false, "Don't call node::tests::Stub::getName");
	std::string ret;
	ret = this->id;
	return ret;
}

unsigned int
Stub::getNodeID() const
{
    return id;
}

wns::service::Service*
Stub::getAnyService(const component::FQSN& /*name*/) const
{
	assure(false, "Don't call node::tests::Stub::getService");
	return NULL;
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
