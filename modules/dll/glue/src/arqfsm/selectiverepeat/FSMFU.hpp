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

#ifndef GLUE_ARQFSM_SELECTIVEREPEAT_FSMFU_HPP
#define GLUE_ARQFSM_SELECTIVEREPEAT_FSMFU_HPP

#include <WNS/ldk/fsm/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>
#include <WNS/ldk/DelayedDeliveryInterface.hpp>

#include <WNS/events/MultipleTimeout.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/module/Base.hpp>

#include <vector>
#include <queue>

namespace glue { namespace arqfsm { namespace selectiverepeat {

	/**
	 * @brief Command used by the SelectiveRepeat arq implementation.
	 *
	 */
	class SelectiveRepeatCommand :
		public wns::ldk::Command
	{
	public:
		typedef enum {I, ACK} FrameType;

		SelectiveRepeatCommand()
		{
			peer.type = I;
			peer.NS = 0;
		} // SelectiveRepeatCommand

		/*
		 * I - Information Frame
		 * ACK - received Packet (ACK)
		 */
		struct {} local;
		struct {
			FrameType type;
			int NS;
		} peer;
		struct {} magic;
	};


	/**
	 * @brief Variables available in FU with FSM support
	 */
	class Variables
	{
	public:
		typedef std::list<wns::ldk::CompoundPtr> CompoundQueue;
		typedef std::vector<wns::ldk::CompoundPtr> CompoundContainer;

		Variables(const wns::pyconfig::View& config) :
			windowSize(config.get<int>("windowSize")),
			sequenceNumberSize(config.get<int>("sequenceNumberSize")),
			resendTimeout(config.get<double>("resendTimeout")),
			NS(0),
			lowerBoundNS(0),
			levelSendBuffer(0),
			leastNR(0),
			sentCompounds(),
			pendingCompoundsQueue(),
			receivedCompounds(),
			pendingACKsQueue(),
			delayingDelivery(false),
			logger(config.get<wns::pyconfig::View>("logger"))
		{
			/**
			 * @todo: ksw,msg
			 * - move constraint checking to PyConfig
			 */
			if (sequenceNumberSize < 2*windowSize)
				throw wns::Exception("SequenceNumberSize is to small for chosen windowSize.");

			sentCompounds.resize(sequenceNumberSize);
			receivedCompounds.resize(sequenceNumberSize);
		} // Variables

		virtual
		~Variables()
		{} // ~Variables

		/**
		 * @brief Window size.
		 */
		int windowSize;

		/**
		 * @brief Divisor for Sequence Numbers.
		 */
		int sequenceNumberSize;

		/**
		 * @brief Time between two transmissions of the same PDU.
		 */
		double resendTimeout;

		/**
		 * @brief Sequence number of the next packet to be sent.
		 */
		int NS;

		/**
		 * @brief Sequence number of the compound at the lower bound of
		 * the current send window.
		 */
		int lowerBoundNS;

		/**
		 * @brief Number of packets in arq send buffer
		 */
		int levelSendBuffer;

		/**
		 * @brief "Least" sequence number of all received packets not yet
		 * delivered to upper FU.
		 */
		int leastNR;

		/**
		 * @brief Container for sent packets not yet acknowledged.
		 */
		CompoundContainer sentCompounds;

		/**
		 * @brief Compounds waiting to be sent.
		 */
		CompoundQueue pendingCompoundsQueue;

		/**
		 * @brief Container for received packets not yet delivered to
		 * upper FU.
		 */
		CompoundContainer receivedCompounds;

		/**
		 * @brief ACKs waiting to be sent
		 */
		CompoundQueue pendingACKsQueue;

		bool delayingDelivery;

		/**
		 * @brief Logger
		 */
		wns::logger::Logger logger;
	};

	typedef wns::ldk::fsm::FunctionalUnit<Variables> MyFUInterface;

	typedef MyFUInterface::BaseFSM::StateInterface StateInterface;

	class FSMFU :
		public MyFUInterface,
		public wns::ldk::CommandTypeSpecifier<SelectiveRepeatCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		virtual public wns::ldk::SuspendableInterface,
		public wns::ldk::SuspendSupport,
		virtual public wns::ldk::DelayedDeliveryInterface,
		public wns::Cloneable<FSMFU>,
		public wns::events::MultipleTimeout<wns::ldk::CompoundPtr>
	{
	public:
		FSMFU(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config);

		virtual
		~FSMFU()
		{} // ~FSMFU

		// SDU and PCI size calculation
		virtual void
		calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

		virtual bool
		onSuspend() const;

	private:
		virtual void
		onTimeout(const wns::ldk::CompoundPtr& compound);

		virtual void
		doDelayDelivery();

		virtual void
		doDeliver();

		wns::pyconfig::View config;

		/**
		 * @brief Number of bits added to the header for each I-Frame
		 */
		int bitsPerIFrame;

		/**
		 * @brief Number of bits added to the header for each ACK-Frame
		 */
		int bitsPerACKFrame;
	};

} // selectiverepeat
} // arqfsm
} // glue

#endif // GLUE_ARQFSM_SELECTIVEREPEAT_FSMFU_HPP


