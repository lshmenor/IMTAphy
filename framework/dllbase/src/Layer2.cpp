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

#include <DLL/Layer2.hpp>
#include <DLL/StationManager.hpp>
#include <DLL/UpperConvergence.hpp>

#include <WNS/ldk/ControlServiceInterface.hpp>
#include <WNS/ldk/ManagementServiceInterface.hpp>
#include <WNS/service/dll/StationTypes.hpp>
#include <WNS/service/phy/ofdma/Handler.hpp>
#include <WNS/service/phy/ofdma/DataTransmission.hpp>
#include <WNS/ldk/utils.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/pyconfig/Parser.hpp>

#include <sstream>

using namespace dll;

ILayer2::StationIDType ILayer2::invalidStationID = -1;
ILayer2::StationIDType ILayer2::broadcastStationID = -2;

Layer2::Layer2(wns::node::Interface* _node, const wns::pyconfig::View& _config, StationManager* _sm) :
	wns::node::component::Component(_node, _config),

	config(_config),
	logger(_config.get("logger")),
	stationManager( (_sm == NULL) ? TheStationManager::getInstance() : _sm),
	stationID(config.get<StationIDType>("stationID")),
	type(wns::service::dll::StationTypes::fromString(config.get<std::string>("stationType"))),
	address(config.get<wns::service::dll::UnicastAddress>("address")),
	ring(config.get<unsigned long int>("ring")),
	air(),
	agent()
{
}


void
Layer2::doStartup()
{
	{
		if (!agent.knows("MAC.Id"))
		{ // new method for ProbeBus
			agent.setContext("MAC.Id", stationID);
			agent.setContext("MAC.Ring", ring);
			agent.setContext("MAC.StationType", type);

			wns::probe::bus::ContextProviderCollection& collection =
				getNode()->getContextProviderCollection();

			collection.addProvider(wns::probe::bus::contextprovider::Container(&this->agent));
		}
	}

	//create the DLL-FUN
	wns::pyconfig::View funConfig(config, "fun");
	fun = new wns::ldk::fun::Main(this, config);
	wns::ldk::configureFUN(fun, funConfig);

	// fire up control and management Services
	{ // do control services
		for (int ii = 0; ii<config.len("controlServices"); ++ii){
			wns::pyconfig::View controlServiceView = config.get("controlServices",ii);
			std::string serviceName = controlServiceView.get<std::string>("serviceName");
			std::string creatorName = controlServiceView.get<std::string>("nameInServiceFactory");
			wns::ldk::ControlServiceCreator* serviceCreator = wns::ldk::ControlServiceFactory::creator(creatorName);
			wns::ldk::ControlServiceInterface* service = serviceCreator->create(getCSR(), controlServiceView);
			addControlService(serviceName, service);
			MESSAGE_BEGIN(NORMAL, logger, m, getName());
			m << " Registered Control Service: " << serviceName;
			MESSAGE_END();
		}
		// do management services @todo pab: get rid of copy and paste patterns
		for (int ii = 0; ii<config.len("managementServices"); ++ii){
			wns::pyconfig::View managementServiceView = config.get("managementServices",ii);
			std::string serviceName = managementServiceView.get<std::string>("serviceName");
			std::string creatorName = managementServiceView.get<std::string>("nameInServiceFactory");
			wns::ldk::ManagementServiceCreator* serviceCreator = wns::ldk::ManagementServiceFactory::creator(creatorName);
			wns::ldk::ManagementServiceInterface* service = serviceCreator->create(getMSR(), managementServiceView);
			addManagementService(serviceName, service);
			MESSAGE_BEGIN(NORMAL, logger, m, getName());
			m << " Registered Management Service: " << serviceName;
			MESSAGE_END();
		}
	}

	/**
	 * @todo pab: when WiMAC conforms to control/management service concept
	 * enable to call fun->onFUNCreated(); */
	// resolve intra-FUN dependencies
	// fun->onFUNCreated();

	// global station registry
	stationManager->registerStation(stationID, address, this);

	if(!config.isNone("upperConvergenceName"))
	{
		std::string upperConvergenceName = config.get<std::string>("upperConvergenceName");
		UpperConvergence* upperConvergence =
			getFUN()->findFriend<UpperConvergence*>(upperConvergenceName);

		// register UpperConvergence as the DLL DataTransmissionService
		addService(config.get<std::string>("dataTransmission"), upperConvergence);
		addService(config.get<std::string>("notification"), upperConvergence);
		addService(config.get<std::string>("flowEstablishmentAndRelease"), upperConvergence);
		upperConvergence->setMACAddress(address);
	}

    if((getStationType() == wns::service::dll::StationTypes::eNB() || 
        getStationType() == wns::service::dll::StationTypes::AP()) &&
        wns::simulator::getRegistry()->knows("DLLAssoc"))
    {
        std::map<int, int> assoc;
        assoc = wns::simulator::getRegistry()->find<std::map<int, int> >("DLLAssoc");
        assoc[getDLLAddress().getInteger()] = getDLLAddress().getInteger();
        wns::simulator::getRegistry()->update("DLLAssoc", assoc);
    }
} // Layer2

Layer2::~Layer2()
{
    if (fun != NULL) {
	delete fun;
	fun = NULL;
    }
} // ~Layer2

Layer2::StationIDType
Layer2::getID() const
{
	return stationID;
} // getID

wns::service::dll::UnicastAddress
Layer2::getDLLAddress() const
{
	return address;
}

// pab: 2006-07-21 This function is to vanish soon.
std::string
Layer2::getName() const
{
	return getNode()->getName();
} // getName

Layer2::StationType
Layer2::getStationType() const
{
	return type;
} // getType

StationManager*
Layer2::getStationManager()
{
	return stationManager;
} // getStationManager

void
Layer2::addAssociationInfoProvider(const std::string& id, dll::services::control::AssociationInfo* provider)
{
	if (! air.knows(id) )
	{
		AssociationInfoContainer tmp;
		air.insert(id, tmp);
	}

	AssociationInfoContainer tmp2(air.find(id));
	tmp2.push_back(provider);
	air.update(id, tmp2);
}

const Layer2::AssociationInfoContainer&
Layer2::getAssociationInfoProvider(const std::string& id) const
{
	return air.find(id);
}

void
Layer2::ContextAgent::doVisit(wns::probe::bus::IContext& context) const
{
	ContextIterator iter = myContextInfo.begin();
	ContextIterator end  = myContextInfo.end();

	for( ; iter != end; ++iter)
	{
		context.insertInt( iter->first, iter->second );
	}
}


void
Layer2::onShutdown()
{
    fun->onShutdown();
} 


