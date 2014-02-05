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

#ifndef GLUE_ARQFSM_ARQBASESTATE_HPP
#define GLUE_ARQFSM_ARQBASESTATE_HPP

#include <GLUE/arqfsm/InSignals.hpp>
#include <GLUE/arqfsm/OutSignals.hpp>

#include <WNS/ldk/Compound.hpp>

#include <WNS/Exception.hpp>

namespace glue { namespace arqfsm {

	template <class FSMFU, class COMMAND>
	class ARQBaseState :
		public FSMFU::UnhandledSignals,
		public InSignals<typename FSMFU::BaseFSM::StateInterface>,
		public OutSignals
	{
	protected:
		typedef typename FSMFU::BaseFSM::StateInterface StateInterface;

	public:
		ARQBaseState(typename FSMFU::BaseFSM* t, const std::string& stateName) :
			FSMFU::UnhandledSignals(t, stateName)
		{} // ARQBaseState

		virtual void
		doInitState()
		{
			onInitState();
		} // doInitState

		virtual void
		doExitState()
		{
			onExitState();
		} // doExitState

		// FU signals
		virtual StateInterface*
		doSendData(const wns::ldk::CompoundPtr& compound)
		{
			return this->onSendData(compound);
		} // doSendData

		virtual StateInterface*
		doOnData(const wns::ldk::CompoundPtr& compound)
		{
			COMMAND* command = dynamic_cast<COMMAND*>(this->getFU()->getCommand(compound->getCommandPool()));
			if (!command)
				throw wns::Exception("Invalid command type");

			switch(command->peer.type)
			{
			case COMMAND::I:
				return onI(compound, command->peer.NS);

			case COMMAND::ACK:
				return onACK(command->peer.NS);

			default:
				throw wns::Exception("Unknown frame type received.");
			}

			return this;
		} // doOnData

		virtual StateInterface*
		doWakeup()
		{
			return this->onWakeup();
		} // doWakeup

		virtual void
		doIsAccepting(const wns::ldk::CompoundPtr&, bool& accepting) const
		{
			this->onIsAccepting(accepting);
		} // doIsAccepting

	protected:
		virtual
		~ARQBaseState()
		{} // ~ARQBaseState

		virtual void
		onInitState()
		{} // onInitState

		virtual void
		onExitState()
		{} // onExitState

		// state signals
		void
		sendSendData(wns::ldk::CompoundPtr compound)
		{
			this->getFU()->queueSendData(compound);
		} // sendSendData

		void
		sendOnData(wns::ldk::CompoundPtr compound)
		{
			this->getFU()->queueOnData(compound);
		} // sendOnData

		void
		sendWakeup()
		{
			this->getFU()->queueWakeup();
		} // sendWakeup

		wns::ldk::CompoundPtr
		createReply(wns::ldk::CompoundPtr compound)
		{
			return wns::ldk::CompoundPtr(new wns::ldk::Compound(this->getFU()->createReply(compound->getCommandPool())));
		} // createReply

		bool
		isAccepting(wns::ldk::CompoundPtr compound)
		{
			return this->getFU()->isAccepting(compound);
		} // isAccepting

		std::string
		getFUNName()
		{
			return this->getFU()->getFUN()->getName();
		} // getFUNName
	};

} // arqfsm
} // glue

#endif // GLUE_ARQFSM_ARQBASESTATE_HPP


