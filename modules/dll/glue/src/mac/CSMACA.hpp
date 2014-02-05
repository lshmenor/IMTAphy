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

#ifndef GLUE_MAC_CSMACA_HPP
#define GLUE_MAC_CSMACA_HPP

#include <GLUE/mac/Backoff.hpp>

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/arq/StopAndWait.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/service/phy/copper/DataTransmissionFeedback.hpp>
#include <WNS/service/phy/copper/CarrierSensing.hpp>
#include <WNS/Observer.hpp>
#include <WNS/simulator/Time.hpp>

namespace glue { namespace mac {

	class CSMACACommand :
		public wns::ldk::Command
	{
	public:
		struct {
		} local;

		struct {
		} peer;

		struct {
		} magic;
	};

	/**
	 * @brief 802.11 style MAC
	 *
	 * @warning NOT READY FOR USAGE! This FU needs testing (but it is
	 * unclear how, see CSMACATest).
	 */
	class CSMACA :
		public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<CSMACACommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<CSMACA>,
		public virtual BackoffObserver
	{
	public:

		CSMACA(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

		virtual
		~CSMACA();

	private:
		// FunctionalUnit / CompoundHandlerInterface
		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& _compound) const;

		virtual void
		doSendData(const wns::ldk::CompoundPtr& _compound);

		virtual void
		doWakeup();

		virtual void
		doOnData(const wns::ldk::CompoundPtr& _compound);

		virtual void
		onFUNCreated();

		virtual void
		backoffExpired();

		virtual void
		onCarrierIdle();

		virtual void
		sifsExpired();

		wns::logger::Logger logger;

		wns::ldk::CompoundPtr iFrame;
		wns::ldk::CompoundPtr ackFrame;

		std::string stopAndWaitARQName;
		wns::ldk::arq::StopAndWait* stopAndWaitARQ;

		Backoff backoff;
		wns::simulator::Time sifsLength;
	};


	class StopAndWait :
		public wns::ldk::arq::StopAndWait,
		public wns::Observer<wns::service::phy::copper::DataTransmissionFeedbackInterface>,
        public wns::Observer<wns::service::phy::copper::CarrierSensing>
	{
	public:
		StopAndWait(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config);

		virtual wns::ldk::CompoundPtr getData();

		virtual void    onTimeout();

		// observer DataTransmissionFeedbackInterface
		virtual void    onDataSent(wns::osi::PDUPtr pdu);

		// observer CarrierSensing
		virtual void	onCarrierIdle();
		virtual void	onCarrierBusy();
		virtual void	onCollision();
	
	private:
		double shortResendTimeout;
		double longResendTimeout;
		bool waitingForAckTransmission;

		
	};
} // mac
} // glue

#endif // GLUE_MAC_CSMACA_HPP
