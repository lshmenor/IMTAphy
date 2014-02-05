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

#include <WNS/node/NodeSimulationModel.hpp>
#include <WNS/node/Node.hpp>
#include <WNS/osi/PDU.hpp>

using namespace wns::node;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    NodeSimulationModel,
    wns::simulator::ISimulationModel,
    "wns.Node.NodeSimulationModel",
    wns::PyConfigViewCreator);

NodeSimulationModel::NodeSimulationModel(const wns::pyconfig::View& config) :
    registry_(),
    logger_(config.get("logger")),
    config_(config)
{
}

NodeSimulationModel::~NodeSimulationModel()
{
	// Shut down the final part of the logger
#ifndef NDEBUG
	long int pdusAvailableBeforeNodesDeletd = wns::osi::PDU::getExistingPDUs();
#endif

	MESSAGE_SINGLE(NORMAL, logger_, "Deleting all nodes");
	while(!registry_.empty())
	{
		wns::node::Interface* doomed = registry_.begin()->second;
		MESSAGE_SINGLE(NORMAL, logger_, "Deleting: " << doomed->getName());
		delete doomed;
		registry_.erase(registry_.begin()->first);
	}

	// Some final debug stuff
#ifndef NDEBUG
	std::cout << "\n";
	std::cout << "PDUs available before Nodes deleted: " << pdusAvailableBeforeNodesDeletd << "\n";
	std::cout << "PDUs available after Nodes deleted:  " << wns::osi::PDU::getExistingPDUs() << "\n";
	std::cout << "Maximum number of PDUs available:    " << wns::osi::PDU::getMaxExistingPDUs() << "\n";
#endif
}

void
NodeSimulationModel::doStartup()
{
    // Construct Nodes
    for (int ii = 0; ii < config_.len("nodes"); ++ii)
    {
        wns::pyconfig::View nodeView =
            config_.get<wns::pyconfig::View>("nodes",ii);

        wns::node::Node* currentNode = new wns::node::Node(&registry_, nodeView);
        currentNode->startup();
    }

    // World created -> call onWorldCreated for each node
    for(wns::node::Registry::const_iterator itr = registry_.begin();
        itr != this->registry_.end();
        ++itr)
    {
        itr->second->onWorldCreated();
    }
}

void
NodeSimulationModel::doShutdown()
{
	MESSAGE_SINGLE(NORMAL, logger_, "Calling onShutdown for all nodes");
	for(wns::node::Registry::const_iterator itr = registry_.begin();
	    itr != registry_.end();
	    ++itr)
    {
		itr->second->onShutdown();
	}
}
