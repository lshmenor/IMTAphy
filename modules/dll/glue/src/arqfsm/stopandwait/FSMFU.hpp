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

#ifndef GLUE_ARQFSM_STOPANDWAIT_FSMFU_HPP
#define GLUE_ARQFSM_STOPANDWAIT_FSMFU_HPP

#include <WNS/ldk/fsm/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>

#include <WNS/events/CanTimeout.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/module/Base.hpp>

namespace glue { namespace arqfsm { namespace stopandwait {

	/**
	 * @brief Command used by the StopAndWait arq implementation.
	 *
	 */
	class StopAndWaitCommand :
		public wns::ldk::Command
	{
	public:
		typedef enum {I, ACK} FrameType;

		StopAndWaitCommand()
		{
			peer.type = I;
			peer.NS = 0;
		}

		/*
		 * I - Information Frame
		 * ACK - Receiver Ready (ACK)
		 */
		struct {} local;
		struct {
			FrameType type;
			int NS;
		} peer;
		struct {} magic;
	};


	/**
	 * Variables available in FU with FSM support
	 */
	class Variables
	{
	public:
		Variables(const wns::pyconfig::View& config) :
			resendTimeout(config.get<double>("resendTimeout")),
			modulo(2),
			NS(0),
			NR(0),
			activeCompound(wns::ldk::CompoundPtr()),
			ackCompound(wns::ldk::CompoundPtr()),
			sendNow(false),
			logger("GLUE", "StopAndWait")
		{
			assure(modulo >= 2, "Invalid modulo.");
		}

		/**
		 * @brief Time between two transmissions of the same PDU.
		 *
		 */
		double resendTimeout;

		/**
		 * @brief Window size.
		 *
		 */
		const int modulo;

		/**
		 * @brief Sequence number of the last packet sent.
		 *
		 */
		int NS;

		/**
		 * @brief Sequence number of the last packet received.
		 *
		 */
		int NR;

		/**
		 * @brief The last packet sent but not acknowledged yet.
		 *
		 */
		wns::ldk::CompoundPtr activeCompound;

		/**
		 * @brief An ACK to be sent.
		 *
		 */
		wns::ldk::CompoundPtr ackCompound;

		/**
		 * @brief Remember to send the activeCompound.
		 *
		 */
		bool sendNow;

		wns::logger::Logger logger;
	};

	typedef wns::ldk::fsm::FunctionalUnit<Variables> MyFUInterface;

	typedef MyFUInterface::BaseFSM::StateInterface StateInterface;

	class FSMFU :
		public MyFUInterface,
		public wns::ldk::CommandTypeSpecifier<StopAndWaitCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		virtual public wns::ldk::SuspendableInterface,
		public wns::ldk::SuspendSupport,
		public wns::Cloneable<FSMFU>,
		public wns::events::CanTimeout
	{
	public:
		FSMFU(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config);

		virtual
		~FSMFU()
		{}

		// SDU and PCI size calculation
		void
		calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

		virtual bool
		onSuspend() const;

	private:
		virtual void
		onTimeout();

		wns::pyconfig::View config;

		/**
		 * @brief Number of bits added to the header for each I-Frame
		 *
		 */
		int bitsPerIFrame;

		/**
		 * @brief Number of bits added to the header for each RR-Frame
		 *
		 */
		int bitsPerRRFrame;
	};

} // stopandwait
} // arqfsm
} // glue

#endif // GLUE_ARQFSM_STOPANDWAIT_FSMFU_HPP


