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

#include <GLUE/trigger/Trigger.hpp>

#include <GLUE/BERProvider.hpp>
#include <WNS/ldk/sar/Fixed.hpp>

#include <WNS/Assure.hpp>

#include <string>
#include <cmath>

using namespace glue::trigger;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Trigger,
				     wns::ldk::FunctionalUnit,
				     "glue.Trigger",
				     FUNConfigCreator);

Trigger::Trigger(fun::FUN* fun, const wns::pyconfig::View& _config) :
	FunctionalUnitLight(fun, _config),
	connected(false),
	config(_config),
	logger(config.get<wns::pyconfig::View>("logger")),
	packetHeaderSize(config.get<int>("packetHeaderSize"))
{
	friends.berProvider = NULL;
	friends.sar = NULL;
} // Trigger

Trigger::~Trigger()
{
	if (connected)
		friends.berProvider->detachBERConsumer(this);
} // ~Trigger

void
Trigger::onFUNCreated()
{
	friends.berProvider = getFUN()->findFriend<BERProvider*>(config.get<std::string>("BERProvider"));
	friends.berProvider->attachBERConsumer(this);
	connected = true;

	assureType(friends.berProvider, wns::ldk::FunctionalUnit*);

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Attached Trigger FU to BERProvider "
	  << dynamic_cast<wns::ldk::FunctionalUnit*>(friends.berProvider)->getName();
	MESSAGE_END();

	friends.sar = getFUN()->findFriend<wns::ldk::sar::Fixed*>(config.get<std::string>("SAR"));
	assureType(friends.sar, wns::ldk::sar::Fixed*);
} // onFUNCreated


void
Trigger::onBERProviderDeleted()
{
	connected = false;

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Lost BERProvider!";
	MESSAGE_END();
} // BERProviderDeleted


void
Trigger::onNewMeasurement(double BER, int packetSize)
{
	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Received BER: " << BER
	  << ", packetSize: " << packetSize;
	MESSAGE_END();

	double dHeaderSize = (double)packetHeaderSize;

	int optPayloadSize = (int)(std::sqrt(dHeaderSize*dHeaderSize/4 - dHeaderSize / log(1-BER)) - dHeaderSize/2);

	friends.sar->setSegmentSize(optPayloadSize);

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Set fragment size to: " << optPayloadSize
	  << "Bit";
	MESSAGE_END();
} // reportBER



