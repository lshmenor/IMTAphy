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

#include <GLUE/mac/CSMACA.hpp>
#include <WNS/container/UntypedRegistry.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/service/phy/copper/Notification.hpp>

using namespace glue::mac;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	CSMACA,
	wns::ldk::FunctionalUnit,
	"glue.CSMACA",
	wns::ldk::FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	StopAndWait,
	wns::ldk::FunctionalUnit,
	"glue.StopAndWait",
	wns::ldk::FUNConfigCreator);


CSMACA::CSMACA(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
	wns::ldk::CommandTypeSpecifier<CSMACACommand>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<CSMACA>(),
	logger(config.get("logger")),
	iFrame(),
	ackFrame(),
	stopAndWaitARQName(config.get<std::string>("stopAndWaitARQName")),
	stopAndWaitARQ(NULL),
	backoff(
		this,
		config.get<wns::simulator::Time>("sifsLength"),
		config.get<wns::simulator::Time>("slotLength"),
		config.get<wns::simulator::Time>("ackLength"),
		wns::logger::Logger(config.get("backoffLogger"))),
	sifsLength(config.get<wns::simulator::Time>("sifsLength"))
{
	MESSAGE_SINGLE(NORMAL, this->logger, "created");
	wns::node::component::Interface* component = getFUN()->getLayer();
	std::string notificationName = config.get<std::string>("phyNotification");
	typedef wns::service::phy::copper::Notification Notification;

	/**
	 * @todo This may not work if something in the configuration file
	 * changes. It is not guaranteed, that the service is already
	 * available!
	 */
	Notification* notification = component->getService<Notification*>(notificationName);
	backoff.startObserving(notification);
}


CSMACA::~CSMACA()
{
}

bool
CSMACA::doIsAccepting(const wns::ldk::CompoundPtr& _compound) const
{
	if(stopAndWaitARQ->getCommand(_compound->getCommandPool())->isACK())
	{
		assure(this->ackFrame == wns::ldk::CompoundPtr(), "There may not be more than one ACK");
		// we're always accepting for ACKs
		return true;
	}
	else
	{
		// There is room for exactly one compound
		return this->iFrame == wns::ldk::CompoundPtr();
	}
}

void
CSMACA::doSendData(const wns::ldk::CompoundPtr& _compound)
{
	wns::ldk::arq::StopAndWaitCommand* sawCommand =
		stopAndWaitARQ->getCommand(_compound->getCommandPool());

	if (sawCommand->isACK() == true)
	{
		MESSAGE_SINGLE(NORMAL, this->logger, "ACK: waiting for SIFS");
		this->ackFrame = _compound;
		wns::simulator::getEventScheduler()->scheduleDelay(
			wns::events::MemberFunction<CSMACA>(this, &CSMACA::sifsExpired), this->sifsLength);
	}
	else
	{
		assure(this->iFrame == wns::ldk::CompoundPtr(), "already got a compound!");
		this->iFrame = _compound;

		MESSAGE_SINGLE(NORMAL, this->logger,
					   "Data arrived, requesting TXOP. Transmission counter is " << sawCommand->localTransmissionCounter);
		backoff.transmissionRequest(sawCommand->localTransmissionCounter>1);
	}
}

void
CSMACA::doWakeup()
{
	// simply forward the wakeup call
	this->getReceptor()->wakeup();
}

void
CSMACA::doOnData(const wns::ldk::CompoundPtr& _compound)
{
	this->getDeliverer()->getAcceptor(_compound)->onData(_compound);
}

void
CSMACA::onFUNCreated()
{
	// this CSMACA MAC works only with StopAndWait ARQ!!
	this->stopAndWaitARQ =
		this->getFUN()->findFriend<wns::ldk::arq::StopAndWait*>(this->stopAndWaitARQName);

	this->stopAndWaitARQ->preferACK(true);
}

void
CSMACA::backoffExpired()
{
	assure(this->iFrame != wns::ldk::CompoundPtr(), "Nothing to send");
	assure(this->getConnector()->hasAcceptor(this->iFrame) == true, "Trying to send although not accepting");
	MESSAGE_SINGLE(NORMAL, this->logger, "Backoff is elapsed sending compound: " << *(this->iFrame));
	this->getConnector()->getAcceptor(this->iFrame)->sendData(this->iFrame);
	this->iFrame = wns::ldk::CompoundPtr();
	// wakeup: we are ready to receive another compound
	this->getReceptor()->wakeup();
}

void
CSMACA::sifsExpired()
{
	assure(this->ackFrame != wns::ldk::CompoundPtr(), "No ACK to send");
	assure(this->getConnector()->hasAcceptor(this->ackFrame) == true, "Trying to send although not accepting");
	MESSAGE_SINGLE(NORMAL, this->logger, "SIFS is elapsed sending ACK: " << *(this->ackFrame));
	this->getConnector()->getAcceptor(this->ackFrame)->sendData(this->ackFrame);
	this->ackFrame = wns::ldk::CompoundPtr();
	// wakeup: we are ready to receive another compound
	this->getReceptor()->wakeup();
}

void
CSMACA::onCarrierIdle()
{
	this->wakeup();
}


StopAndWait::StopAndWait(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config) :
	wns::ldk::arq::StopAndWait(fuNet, config),

	shortResendTimeout(config.get<double>("shortResendTimeout")),
	longResendTimeout(config.get<double>("longResendTimeout"))
{
	wns::node::component::Interface* component = getFUN()->getLayer();
	std::string dataTransmissionNotificationName = config.get<std::string>("phyDataTransmissionFeedback");
	std::string carrierSensingNotificationName = config.get<std::string>("phyNotification");
	typedef wns::service::phy::copper::DataTransmissionFeedback DataTransmissionFeedback;
	typedef wns::service::phy::copper::Notification carrierSensingNotification;

	/**
	 * @todo This may not work if something in the configuration file
	 * changes. It is not guaranteed, that the service is already
	 * available!
	 */
	DataTransmissionFeedback* dtfb = component->getService<DataTransmissionFeedback*>(dataTransmissionNotificationName);
	carrierSensingNotification *csn = component->getService<carrierSensingNotification*>(carrierSensingNotificationName);

	this->wns::Observer<wns::service::phy::copper::DataTransmissionFeedbackInterface>::startObserving(dtfb);
	this->wns::Observer<wns::service::phy::copper::CarrierSensing>::startObserving(csn);

	waitingForAckTransmission = false;
}

wns::ldk::CompoundPtr
StopAndWait::getData()
{
	sendNow = false;
	// send a copy
	return activeCompound->copy();
}

void
StopAndWait::onDataSent(wns::osi::PDUPtr pdu)
{
	assure(wns::dynamicCast<wns::ldk::Compound>(pdu), "not a CompoundPtr");
	wns::ldk::CompoundPtr compound = wns::dynamicCast<wns::ldk::Compound>(pdu);

	wns::ldk::arq::StopAndWaitCommand* sawCommand =
		this->getCommand(compound->getCommandPool());
	if (sawCommand->isACK() == true)
	{
		// For ack we don't start the timer, of course ...
		return;
	}

	assure(
		compound->getBirthmark() == activeCompound->getBirthmark(),
		"Compound which has been sent is not the compound we're currently transmitting.");
	this->setTimeout(shortResendTimeout);
	MESSAGE_SINGLE(VERBOSE, this->logger, "Data is send, waiting for ACK");
	waitingForAckTransmission = true;
}

void
StopAndWait::onCarrierIdle()
{
}

void StopAndWait::onCarrierBusy()
{
	MESSAGE_SINGLE(VERBOSE, this->logger, "onCarrierBusy, waitForAck is " << waitingForAckTransmission);
	if(waitingForAckTransmission)
	{
		// This could be the ack that we are waiting on, so extend the timeout
		assure(hasTimeoutSet(), "Resend Timeout already finished, but carrier just got busy");
		this->cancelTimeout();
		this->setTimeout(longResendTimeout);
		waitingForAckTransmission = false;
	}
}

void StopAndWait::onTimeout()
{
	waitingForAckTransmission = false;
	wns::ldk::arq::StopAndWait::onTimeout();
	MESSAGE_SINGLE(NORMAL, this->logger,
				   "Timeout, increase transmission counter to " << getCommand(activeCompound)->localTransmissionCounter);
}

void StopAndWait::onCollision()
{
}

