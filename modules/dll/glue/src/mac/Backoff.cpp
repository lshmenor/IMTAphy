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

#include <GLUE/mac/Backoff.hpp>

#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/container/UntypedRegistry.hpp>
#include <WNS/Assure.hpp>

using namespace glue::mac;


Backoff::Backoff(
	BackoffObserver* _backoffObserver,
	wns::simulator::Time _sifsLength,
	wns::simulator::Time _slotLength,
	wns::simulator::Time _ackLength,
	const wns::logger::Logger& _logger) :

	backoffObserver(_backoffObserver),
	sifsLength(_sifsLength),
	slotLength(_slotLength),
	ackLength(_ackLength),
	difsLength(_sifsLength + 2.0*slotLength),
	eifsLength(_sifsLength + slotLength + _ackLength),
	state(idle),
    transmissionState(none),
	cwMin(7),
	cwMax(255),
	cw(cwMin),
	uniform(0.0, 1.0, wns::simulator::getRNG()),
	currentSignal(),
	logger(_logger),
	// we start with an idle carrier
	carrierSenseResult(carrierIdle),
	counter(0)
{
	assureNotNull(backoffObserver);

	if (this->carrierSenseResult == carrierIdle)
	{
		this->startNewBackoffCountdown(this->difsLength);
	}
}


Backoff::~Backoff()
{
}

void
Backoff::startNewBackoffCountdown(wns::simulator::Time ifsLength)
{
	assure(this->carrierSenseResult == carrierIdle, "carrier must be idle to start backoff");
	// draw backoff
	// it includes the DIFS, so add one
	this->counter = int(this->uniform() * this->cw) + 1;
	MESSAGE_SINGLE(
		NORMAL,
		this->logger,
		"Draw new backoff from 0-" << this->cw << " -> " << this->counter << " slots, waiting for " << ifsLength);
	// First stage: Try to survive (A|D|E)IFS
	this->waitForTimer(ifsLength);
}

void
Backoff::waitForTimer(wns::simulator::Time& waitLength)
{
	assure(this->currentSignal == NULL, "Only one active signal at a time allowed");
        this->currentSignal =
            wns::simulator::getEventScheduler()->
            scheduleDelay( LocalSignal(this, &Backoff::timerExpired), waitLength);
	this->state = inSlotCountdown;
}

void
Backoff::timerExpired()
{
	assure(this->state == inSlotCountdown, "timer expired but not in state inWait");
	this->currentSignal = wns::events::scheduler::IEventPtr();
	--this->counter;
   	if(this->counter == 0)
	{
		// backoff finished
		MESSAGE_SINGLE(
			NORMAL,
			this->logger,
			"Backoff has finished, transmission state is " << ((this->transmissionState == pending) ? "pending -> tx" : "none"));
		if (this->transmissionState == pending)
		{
			this->state = finished;
			this->transmissionState = transmitting;
			this->backoffObserver->backoffExpired();
		}
		else
		{
			this->state = idle;
		}
	}
	else
	{
		MESSAGE_SINGLE(
			NORMAL,
			this->logger,
			"Counting down backoff, now " << this->counter << " slots, waiting for " << this->slotLength);
		this->waitForTimer(this->slotLength);
	}
}

void
Backoff::transmissionRequest(const bool isRetransmission)
{

	assure(this->transmissionState == none, "already one transmission pending");
	this->transmissionState = pending;

	if ((this->state == idle) && (this->carrierSenseResult == carrierIdle))
	{
		// Postbackoff has expired, the medium is idle... go!
		MESSAGE_SINGLE(
			NORMAL,
			logger,
			"Post-Backoff has expired -> direct go");
		this->transmissionState = transmitting;
		this->backoffObserver->backoffExpired();
		return;
	}

	// compute backoff window
	if(isRetransmission)
	{
		if (this->cw < this->cwMax)
		{
			this->cw = this->cw * 2 + 1;
		}
		else
		{
			this->cw = this->cwMax;
		}
		// retransmission requires new backoff window
		this->counter = int(this->uniform() * this->cw) + 1;
		MESSAGE_SINGLE(
			NORMAL,
			logger,
			"Data is retransmission -> cw = " << this->cw << " slots, new counter = " << this->counter);
	}
	else
	{
		this->cw = this->cwMin;
		MESSAGE_SINGLE(
			NORMAL,
			logger,
			"Data, first transmission attempt, lowering contention window to " << this->cw << " slots");
	}
}

void
Backoff::onCarrierIdle()
{
	wns::simulator::Time nextWaitTime;
	CarrierSensingResult lastCarrierSense = this->carrierSenseResult;

	this->carrierSenseResult = carrierIdle;

	if(lastCarrierSense != carrierIdle)
	{
		if(this->transmissionState == transmitting)
		{
			this->carrierSenseResult = carrierIdle;
			// it was our transmission, which is finished now
			this->transmissionState = none;
			// Do a new (post-) backoff
			this->startNewBackoffCountdown(this->difsLength);
		}
		else
		{
			// it was another transmission, so continue backoff
			// starting with DIFS or EIFS, depending on the last state
			if(lastCarrierSense == carrierError)
			{
				nextWaitTime = this->eifsLength;
			}
			else
			{
				nextWaitTime = this->difsLength;
			}
			this->carrierSenseResult = carrierIdle;
			++this->counter;
			this->waitForTimer(nextWaitTime);
		}
	}
}

void
Backoff::onCarrierBusy()
{
	this->carrierSenseResult = carrierBusy;
	if(this->state == inSlotCountdown)
	{
		// Abort countdown
		assureNotNull(this->currentSignal);
		// delete current event waiting for DIFS of BackoffSlotElapsed
		wns::simulator::getEventScheduler()->cancelEvent(this->currentSignal);
		this->currentSignal = wns::events::scheduler::IEventPtr();
		this->state = waitingForCarrierIdle;
		MESSAGE_SINGLE(
			NORMAL,
			this->logger,
			"Carrier busy detected during countdown -> freeze.");
	}
}

void
Backoff::onCollision()
{
	this->carrierSenseResult = carrierError;
	if (this->state == inSlotCountdown)
	{
		assureNotNull(this->currentSignal);
		// delete current event waiting for DIFS of BackoffSlotElapsed
		wns::simulator::getEventScheduler()->cancelEvent(this->currentSignal);
		this->currentSignal = wns::events::scheduler::IEventPtr();
		this->state = waitingForCarrierIdle;
		MESSAGE_SINGLE(
			NORMAL,
			this->logger,
			"CarrierError detected during countdown -> abort + EIFS.");
	}
}

bool
Backoff::isCarrierIdle() const
{
	return this->carrierSenseResult == carrierIdle;
}
