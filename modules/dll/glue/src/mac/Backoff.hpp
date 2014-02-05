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

#ifndef GLUE_MAC_BACKOFF_HPP
#define GLUE_MAC_BACKOFF_HPP

#include <WNS/service/phy/copper/CarrierSensing.hpp>
#include <WNS/Observer.hpp>

#include <WNS/events/MemberFunction.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/events/scheduler/IEvent.hpp>

namespace glue { namespace mac {

	class BackoffObserver
	{
	public:
		virtual
		~BackoffObserver()
		{}

		virtual void
		backoffExpired() = 0;

		// FIXME: should not be here
		virtual void
		onCarrierIdle()
		{}
	};

	/**
	 * @brief 802.11 DCF style backoff
	 */
	class Backoff :
		public wns::Observer<wns::service::phy::copper::CarrierSensing>
	{
		typedef wns::events::MemberFunction<Backoff> LocalSignal;

	public:
		explicit
		Backoff(
			BackoffObserver* backoffObserver,
			wns::simulator::Time sifsLength,
			wns::simulator::Time slotLength,
			wns::simulator::Time ackLength,
			const wns::logger::Logger& _logger);

		virtual ~Backoff();

		virtual void	transmissionRequest(bool isRetransmission = false);

		virtual void	timerExpired();

		virtual void	onCarrierIdle();

		virtual void	onCarrierBusy();

		virtual void	onCollision();

		virtual bool	isCarrierIdle() const;

	private:
		void startNewBackoffCountdown(wns::simulator::Time ifsLength);

		void waitForTimer(wns::simulator::Time& waitLength);

		enum State {
			idle,
			inSIFS,
			inSlotCountdown,
			finished,
			waitingForCarrierIdle
		};

		enum CarrierSensingResult {
			carrierIdle,
			carrierBusy,
			carrierError
		};

		enum tTransmissionState {
			none,
			pending,
			transmitting
		};

		BackoffObserver* backoffObserver;

		wns::simulator::Time sifsLength;
		wns::simulator::Time slotLength;
		wns::simulator::Time ackLength;
		wns::simulator::Time difsLength;
		wns::simulator::Time eifsLength;

		State state;
		tTransmissionState transmissionState;

		int cwMin;
		int cwMax;
		int cw;
		wns::distribution::Uniform uniform;
		wns::events::scheduler::IEventPtr currentSignal;
		wns::logger::Logger logger;
		CarrierSensingResult carrierSenseResult;

		// For testing purpose this method and variable is protected and may be
		// set by a special version of the backoff
	protected:
		int counter;
		
	};
} // mac
} // glue

#endif // GLUE_MAC_BACKOFF_HPP
