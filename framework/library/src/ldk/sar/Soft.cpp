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

#include <WNS/ldk/sar/Soft.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/Ttos.hpp>

using namespace wns::ldk;
using namespace wns::ldk::sar;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Soft, FunctionalUnit, "wns.sar.Soft", FUNConfigCreator);

SoftCommand::SoftCommand()
	: SARCommand()
{
	PER  = 0.0;
} // SoftCommand

double
SoftCommand::getErrorRate() const
{
	return PER;
}

void
SoftCommand::setPER(const double _PER)
{
	PER = _PER;
}

Soft::Soft(fun::FUN* fuNet, const wns::pyconfig::View& config) :
	    SAR<SoftCommand>(fuNet, config),
		HasReceptor<>(),
		HasConnector<>(),
		HasDeliverer<>(),
		Delayed<Soft>(),
		Cloneable<Soft>(),

		incoming(),
		fragmentNumber(0),
		PERProviderName(config.get<std::string>("PERProvider")),
		useProbe(config.get<bool>("useProbe")),
		probeName(config.get<std::string>("probeName")),
		perProbeBus( new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&getFUN()->getLayer()->getContextProviderCollection()),
								   probeName))
{
	friends.PERProvider = NULL;
} // Soft

Soft::~Soft()
{}

void
Soft::onFUNCreated()
{
	friends.PERProvider = getFUN()->findFriend<FunctionalUnit*>(PERProviderName);
	assure(friends.PERProvider, "SoftSAR requires a PERProvider friend with name '" + PERProviderName + "'");

}

void
Soft::processIncoming(const CompoundPtr& compound)
{
	SoftCommand* command = getCommand(compound->getCommandPool());

	ErrorRateProviderInterface* ppi = dynamic_cast<ErrorRateProviderInterface*>(
		friends.PERProvider->getCommand(compound->getCommandPool()));

	assure(ppi, "Expected a ErrorRateProviderInterface instance.");

	PERStorageEntry received;
	received.first  = compound;
	received.second = ppi->getErrorRate();

	incoming.push_back(received);

	MESSAGE_BEGIN(NORMAL, logger, m, "");
	m << "received fragment "
	  << " pos "
	  << command->magic.pos
	  << " with PER: "
	  << received.second
	  << " lastFragment "
	  << ( command->peer.lastFragment ? "yes" : "no" )
		;
	MESSAGE_END();

	assure(command->magic.fragmentNumber == fragmentNumber,
		   "Missing fragment number: " + wns::Ttos(fragmentNumber) +
		   ", received fragment number: " + wns::Ttos(command->magic.fragmentNumber));

	if(!command->peer.lastFragment)
	{
		++fragmentNumber;
		return;
	}

	MESSAGE_BEGIN(NORMAL, logger, m, "");
	m << " -> compound complete, starting reassembly";
	MESSAGE_END();

	CommandPool* reassembledPCI = getFUN()->createCommandPool();
	getFUN()->getProxy()->partialCopy(this, reassembledPCI, compound->getCommandPool());
	CompoundPtr reassembled(new Compound(reassembledPCI, compound->getData()));

	double  packetGood = 1.0;
	int     counter = 0;

	for ( PERStorageContainer::iterator it = incoming.begin() ;
		  it != incoming.end() ;
		  it++ )
	{
		MESSAGE_BEGIN(NORMAL, logger, m, "");
		m << "fragment " << counter
		  << " PER = " << (*it).second;
		MESSAGE_END();

		packetGood  *= (1 - (*it).second);
		counter++;
	}
	double resultingPER = 1 - packetGood;

	MESSAGE_BEGIN(NORMAL, logger, m, "");
	m << "Resulting overall PER = " << resultingPER ;
	MESSAGE_END();

	perProbeBus->put(resultingPER);

	SoftCommand* myCommand = activateCommand(reassembledPCI);
	myCommand->setPER( resultingPER );

	incoming.clear();
	fragmentNumber = 0;

	getDeliverer()->getAcceptor(reassembled)->onData(reassembled);

} // processIncoming


