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

#ifndef GLUE_CONVERGENCE_LOWER2COPPER_HPP
#define GLUE_CONVERGENCE_LOWER2COPPER_HPP

#include <GLUE/convergence/Lower.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>

#include <WNS/service/phy/copper/DataTransmission.hpp>
#include <WNS/service/phy/copper/Notification.hpp>
#include <WNS/service/phy/copper/Handler.hpp>
#include <WNS/service/phy/copper/CarrierSensing.hpp>

#include <WNS/service/dll/Address.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/Observer.hpp>

#include <map>

namespace glue { namespace convergence {

	class Lower2Copper:
		virtual public Lower,
		virtual public wns::Observer<wns::service::phy::copper::Handler>,
		virtual public wns::Observer<wns::service::phy::copper::CarrierSensing>,
		public wns::ldk::CommandTypeSpecifier<LowerCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<Lower2Copper>
	{
		typedef wns::Subject<wns::service::phy::copper::Handler> HandlerSubject;
		typedef wns::Subject<wns::service::phy::copper::CarrierSensing> CarrierSensingSubject;

	public:
		Lower2Copper(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
		virtual ~Lower2Copper();

		// wns::service::phy::copper::Handler
		virtual void onData(const wns::osi::PDUPtr& pdu, double ber, bool collision);

		// wns::service::phy::copper::CarrierSensing
		virtual void onCarrierIdle();
		virtual void onCarrierBusy();
		virtual void onCollision();
		virtual void setNotificationService(wns::service::Service* phy);
		virtual wns::service::phy::copper::Notification* getNotificationService() const;
		virtual void setDataTransmissionService(wns::service::Service* phy);
		virtual wns::service::phy::copper::DataTransmission* getDataTransmissionService() const;
 		virtual void setMACAddress(const wns::service::dll::UnicastAddress& address);

	private:
		// CompoundHandlerInterface
		virtual void doSendData(const wns::ldk::CompoundPtr& sdu);
		virtual void doOnData(const wns::ldk::CompoundPtr& compound);
		virtual void onFUNCreated();
		virtual bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;
		virtual void doWakeup();
		bool hasCommandOf(FunctionalUnit* routing, const wns::ldk::CompoundPtr& compound) const
		{
			return getFUN()->getProxy()->commandIsActivated(compound->getCommandPool(), routing);
		}

		void pushUp(const wns::ldk::CompoundPtr& compound, double ber, const wns::osi::PDUPtr& pdu);

        void traceIncoming(wns::ldk::CompoundPtr compound, bool collision);

		wns::pyconfig::View config;
		wns::logger::Logger logger;

		struct Friends {
			UnicastUpper* unicastRouting;
			BroadcastUpper* broadcastRouting;
		} friends;

        /** @brief Detailed output about channel state */
        wns::probe::bus::ContextCollectorPtr jsonTracing;

		wns::service::phy::copper::DataTransmission* dataTransmission;
		wns::service::phy::copper::Notification* notificationService;
		wns::service::dll::UnicastAddress address;
		bool isBlocking;
	};
} // convergence
} // glue

#endif // NOT defined GLUE_CONVERGENCE_LOWER2COPPER_HPP


