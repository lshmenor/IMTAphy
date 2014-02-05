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

#ifndef GLUE_BERMEASUREMENTREPORTING_HPP
#define GLUE_BERMEASUREMENTREPORTING_HPP

#include <GLUE/BERConsumer.hpp>
#include <GLUE/BERProvider.hpp>

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Processor.hpp>

#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue {

	/**
	 * @brief Contains BER and packetSize
	 * @author Klaus Sambale <ksw@comnets.rwth-aachen.de>
	 * @todo PacketSize in Bit?
	 *
	 * The measure BER will be transmitterd to BER
	 * The packet Size is in the magic part.
	 */
	class BERMeasurementReportingCommand :
		public wns::ldk::Command
	{
	public:
		/**
		 * @brief Constructor
		 */
		BERMeasurementReportingCommand()
		{
			peer.BER = 0.0;
			magic.packetSize = 0;
		} // BERMeasurementReportingCommand

		struct {} local;
		struct {
			double BER;
		} peer;
		struct {
			int packetSize;
		} magic;

	}; // BERMeasurementReportingCommand


	/**
	 * @brief FU reporting local BER measurements to a peer FU
	 * @author Klaus Sambale <ksw@comnets.rwth-aachen.de>
	 *
	 * The reports are created periodically and the FU will try to send them
 	 * immedeatley. If this fails, because the lower FU is not accpeting it
 	 * will store the report and try to send it later. If the next report
 	 * will be ready before the stored/old one has been sent, the old report
 	 * will be discarded.
	 *
	 * In case no data for a new report is available (i.e., no PDUs with an
	 * according BER have been received) no report will be generated.
	 */
	class BERMeasurementReporting :
		virtual public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<BERMeasurementReportingCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<BERMeasurementReporting>,
		public wns::events::PeriodicTimeout,
		public BERConsumer,
		public BERProvider
	{
	public:
		/**
		 * @brief Constructor
		 */
		explicit
		BERMeasurementReporting(
			wns::ldk::fun::FUN* fuNet,
			const wns::pyconfig::View& config);


		/**
		 * @brief Destructor
		 */
		virtual
		~BERMeasurementReporting();

		/**
		 * @name BERConsumer interface
		 */
		//@{
		/**
		 * @brief Implementation of glue::BERConsumer::onBERProviderDeleted()
		 */
		virtual void
		onBERProviderDeleted();

		/**
		 * @brief Implementation of glue::BERConsumer::onNewMeasurement()
		 */
		virtual void
		onNewMeasurement(double BER, int packetSize);
		//@}

		/**
		 * @name wns::events::PeriodicTimeout interface
		 */
		//@{
		virtual void
		periodically();
		//@}

	private:
		/**
		 * @name FunctionalUnit interface
		 */
		//@{
		/**
		 * @brief May not be called, will throw
		 *
		 * @todo (msg) If we have some FUs which might not be used by
		 * other FUs for DataTransmission, does it make sense to have
		 * special FUs for this?
		 * This FU is a good example it will never be used by any other
		 * FU for data transmission, or?
		 */
		virtual void
		doSendData(const wns::ldk::CompoundPtr&)
		{
			throw wns::Exception("doSendData of BERMeasurementReporting FU may not be called.");
		} // doSendData

		/**
		 * @brief Receive measurement reports from peer FU
		 */
		virtual void
		doOnData(const wns::ldk::CompoundPtr& compound);

		/**
		 * @brief Find BER provider
		 */
		virtual void
		onFUNCreated();

		/**
		 * @brief Add headerSize
		 */
		virtual void
		calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

		/**
		 * @brief May not be called, will throw
		 *
		 * @todo (msg) If we have some FUs which might not be used by
		 * other FUs for DataTransmission, does it make sense to have
		 * special FUs for this?
		 * This FU is a good example it will never be used by any other
		 * FU for data transmission, or?
		 */
		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr&) const
		{
			throw wns::Exception("doIsAccepting of BERMeasurementReporting FU may not be called.");
		} // doIsAccepting

		/**
		 * @brief Send report, if one is available
		 */
		virtual void
		doWakeup();
		//@}

		struct Friends
		{
			BERProvider* berProvider;
		} friends;

		/**
		 * @brief Size of the command
		 */
		int headerSize;

		double summedBER;
		int summedPacketSize;
		int numberOfPackets;

		bool hasSomethingToSend;
		wns::ldk::CompoundPtr compoundToBeSent;

		wns::pyconfig::View config;
		wns::logger::Logger logger;
	};

} // glue

#endif // NOT defined GLUE_BERMEASUREMENTREPORTING


