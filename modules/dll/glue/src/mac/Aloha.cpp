/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/mac/Aloha.hpp>
#include <WNS/container/UntypedRegistry.hpp>
#include <WNS/events/MemberFunction.hpp>
#include <WNS/simulator/ISimulator.hpp>

using namespace glue;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Aloha,
	wns::ldk::FunctionalUnit,
	"glue.mac.Aloha",
	wns::ldk::FUNConfigCreator);


Aloha::Aloha(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
	wns::ldk::CommandTypeSpecifier<AlohaCommand>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<Aloha>(),
	logger(config.get("logger")),
	allowTransmission(false),
	maxWait(config.get<wns::simulator::Time>("maximumWaitingTime")),
	uniform(0.0, 1.0, wns::simulator::getRNG())
{
        MESSAGE_SINGLE(NORMAL, this->logger, "created");

	wns::simulator::Time randomBackoff = uniform() * this->maxWait;
	MESSAGE_SINGLE(
		NORMAL,
		this->logger,
		"First Compound will be sent in " << randomBackoff << " seconds the earliest");
        wns::events::MemberFunction<Aloha> ev (this, &Aloha::allowTransmissionAfterElapsedBackoff);
        wns::simulator::getEventScheduler()->scheduleDelay(ev, randomBackoff);
}


Aloha::~Aloha()
{

}


bool
Aloha::doIsAccepting(const wns::ldk::CompoundPtr& /*_compound*/) const
{
        return allowTransmission;
}


void
Aloha::doSendData(const wns::ldk::CompoundPtr& compound)
{
	assure(allowTransmission, "doSendData called although backoff timer has not yet elapsed");
	assure(this->getConnector()->hasAcceptor(compound) == true, "Not able to send data. Aloha may only be used over NON-BLOCKING PHYs");

	MESSAGE_SINGLE(NORMAL, this->logger, "Sending compound: " << *compound);
	this->getConnector()->getAcceptor(compound)->sendData(compound);

        allowTransmission = false;

	// start random backoff for the next compound
	wns::simulator::Time randomBackoff = uniform() * this->maxWait;
	MESSAGE_SINGLE(
		NORMAL,
		this->logger,
		"Next Compound ("<< *compound <<") will be sent in " << randomBackoff << " seconds the earliest");
	wns::events::MemberFunction<Aloha> ev (this, &Aloha::allowTransmissionAfterElapsedBackoff);
	wns::simulator::getEventScheduler()->scheduleDelay(ev, randomBackoff);
}

void
Aloha::doWakeup()
{
	// simply forward the wakeup call
	this->getReceptor()->wakeup();
}


void
Aloha::doOnData(const wns::ldk::CompoundPtr& compound)
{
	this->getDeliverer()->getAcceptor(compound)->onData(compound);
}


void
Aloha::allowTransmissionAfterElapsedBackoff()
{
        allowTransmission = true;
	this->getReceptor()->wakeup();
}
