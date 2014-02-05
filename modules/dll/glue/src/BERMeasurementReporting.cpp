/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/BERMeasurementReporting.hpp>

using namespace glue;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	BERMeasurementReporting,
	wns::ldk::FunctionalUnit,
	"glue.BERMeasurementReporting",
	FUNConfigCreator);

BERMeasurementReporting::BERMeasurementReporting(
	wns::ldk::fun::FUN* fuNet,
	const wns::pyconfig::View& _config) :

	wns::ldk::CommandTypeSpecifier<BERMeasurementReportingCommand>(fuNet),
	HasReceptor<>(),
	HasConnector<>(),
	HasDeliverer<>(),
	wns::Cloneable<BERMeasurementReporting>(),
	headerSize(_config.get<int>("headerSize")),
	summedBER(0.0),
	summedPacketSize(0),
	numberOfPackets(0),
	hasSomethingToSend(false),
	compoundToBeSent(),
	config(_config),
	logger(_config.get<wns::pyconfig::View>("logger"))
{
	friends.berProvider = NULL;
	startPeriodicTimeout(config.get<double>("transmissionInterval"),
			     config.get<double>("transmissionInterval"));
}


BERMeasurementReporting::~BERMeasurementReporting()
{
} // ~BERMeasurement


void
BERMeasurementReporting::doOnData(const CompoundPtr& compound)
{
	BERMeasurementReportingCommand* berCommand = getCommand(compound->getCommandPool());

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Received BER measurement report from peer entity\n"
	  << " BER: " << berCommand->peer.BER
	  << ", packet size: " << berCommand->magic.packetSize;
	MESSAGE_END();

	notifyBERConsumers(berCommand->peer.BER, berCommand->magic.packetSize);
} // doOnData


void
BERMeasurementReporting::onFUNCreated()
{
	friends.berProvider = getFUN()->findFriend<BERProvider*>(config.get<std::string>("BERProvider"));
	friends.berProvider->attachBERConsumer(this);
} // onFUNCreated


void
BERMeasurementReporting::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
{
	getFUN()->calculateSizes(commandPool, commandPoolSize, dataSize, this);

	commandPoolSize += headerSize;
} // calculateSizes


void
BERMeasurementReporting::onBERProviderDeleted()
{
	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Lost BERProvider!";
	MESSAGE_END();
} // BERProviderDeleted


void
BERMeasurementReporting::onNewMeasurement(double BER, int packetSize)
{
	if (BER < 0.0 || BER > 0.5)
	{
		std::stringstream ss;

		ss << "Invalid BER: " << BER << "! "
		   << "BER has to be in interval [0.0; 0.5].";

		throw wns::Exception(ss.str());
	}
	else if (packetSize < 1)
	{
		std::stringstream ss;

		ss << "Invalid packet size: " << packetSize << "! "
		   << "Packet size has to be in interval [1, oo).";

		throw wns::Exception(ss.str());
	}

	summedBER += BER * packetSize;
	summedPacketSize += packetSize;
	++numberOfPackets;
} // reportBER


void
BERMeasurementReporting::periodically()
{
	hasSomethingToSend = false;

	if (numberOfPackets)
	{
		CompoundPtr compound = CompoundPtr(new Compound(getFUN()->getProxy()->createCommandPool()));

		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << " Generating BER measurement report for peer entity";
		MESSAGE_END();

		BERMeasurementReportingCommand* berCommand = activateCommand(compound->getCommandPool());
		berCommand->peer.BER = summedBER / summedPacketSize;
		berCommand->magic.packetSize = summedPacketSize / numberOfPackets;

		hasSomethingToSend = true;
		compoundToBeSent = compound;
		wakeup();
	}

	summedBER = 0.0;
	summedPacketSize = 0;
	numberOfPackets = 0;
} // periodically


void
BERMeasurementReporting::doWakeup()
{
	if (hasSomethingToSend && getConnector()->hasAcceptor(compoundToBeSent))
	{
		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << " Transmitting BER measurement report to peer entity";
		MESSAGE_END();

		getConnector()->getAcceptor(compoundToBeSent)->sendData(compoundToBeSent);
		hasSomethingToSend = false;
	}
} // wakeup



