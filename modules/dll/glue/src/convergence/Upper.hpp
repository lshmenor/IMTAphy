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

#ifndef GLUE_CONVERGENCE_UPPER_HPP
#define GLUE_CONVERGENCE_UPPER_HPP

#include <DLL/UpperConvergence.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Forwarding.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/node/component/Interface.hpp>

#include <WNS/pyconfig/View.hpp>

namespace glue { namespace convergence {

	/**
	 * @brief Contains MAC address of sending station
	 */
	class UpperCommand :
		public wns::ldk::Command
	{
	public:
		struct Local {
		} local;
		struct Peer {
			wns::service::dll::UnicastAddress sourceMACAddress;
		} peer;
		struct Magic {
		} magic;
	protected:
		UpperCommand()
		{
			this->peer.sourceMACAddress = wns::service::dll::UnicastAddress();
		}
	}; // UpperCommand

	/**
	 * @brief Contains MAC address of sending and receiving station
	 */
	class UnicastUpperCommand :
		public UpperCommand
	{
	public:
		typedef wns::service::dll::UnicastAddress Address;

		UnicastUpperCommand()
		{
		} // UnicastUpperCommand

		struct Peer :
			public UpperCommand::Peer
		{
			Address targetMACAddress;
		} peer;
	}; // UnicastUpperCommand


	/**
	 * @brief Contains MAC address of sending station and special broadcast
	 * MAC address
	 */
	class BroadcastUpperCommand :
		public UpperCommand
	{
	public:
		typedef wns::service::dll::BroadcastAddress Address;

		/**
		 * @brief Constructor
		 */
		BroadcastUpperCommand()
		{
		} // BroadcastUpperCommand

		/**
		 * @brief Special broadcast MAC address
		 */
		struct Peer :
			public UpperCommand::Peer
		{
			Address targetMACAddress;
		} peer;
	}; // BroadcastUpperCommand

	/**
	 * @brief Used by IP for broadcast or unicast transmission
	 *
	 * This class can be configured to be either a broadcast or unicast
	 * unit and implements the methods which are equal for both.
	 */
	template<class COMMAND>
	class Upper :
		virtual public wns::service::dll::DataTransmission<typename COMMAND::Address>,
		virtual public wns::service::dll::Notification,
 		public wns::ldk::CommandTypeSpecifier<COMMAND>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::ldk::FunctionalUnit,
		public wns::Cloneable< Upper<COMMAND> >
	{
	public:
		using FunctionalUnit::sendData;
		typedef COMMAND Command;

		Upper(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
			wns::ldk::CommandTypeSpecifier<Command>(fun),
			wns::ldk::HasReceptor<>(),
			wns::ldk::HasConnector<>(),
			wns::ldk::HasDeliverer<>(),
			wns::ldk::FunctionalUnit(),
			wns::Cloneable<Upper>(),

			logger_(config.get<wns::pyconfig::View>("logger")),
			sourceMACAddress_()
		{
		} // Upper

		virtual void
		registerFlowHandler(wns::service::dll::FlowHandler*){};

		virtual void
		registerIRuleControl(wns::service::dll::IRuleControl*){};

		virtual void
		registerHandler(wns::service::dll::protocolNumber protocol, wns::service::dll::Handler* dh)
		{
			if (NULL == dh)
			{
				throw wns::Exception("invalid data handler (NULL)");
			}
			assure(!dataHandlerRegistry_.knows(protocol), "data handler already set");
			dataHandlerRegistry_.insert(protocol, dh);
		} // registerHandler

		virtual void
		setMACAddress(const wns::service::dll::UnicastAddress& sourceMACAddress)
		{
			sourceMACAddress_ = sourceMACAddress;
		} // setMACAddress

		virtual void
		sendData(const typename Command::Address& targetMACAddress,
			 const wns::osi::PDUPtr& sdu,
			 wns::service::dll::protocolNumber protocol,
			 int /* _dllFlowID = 0 */)
		{
			sdu->setPDUType(protocol);
			wns::ldk::CompoundPtr compound(
				new wns::ldk::Compound(
					this->getFUN()->getProxy()->createCommandPool(), sdu));

			Command* command = this->activateCommand(compound->getCommandPool());
			command->peer.sourceMACAddress = sourceMACAddress_;
			command->peer.targetMACAddress = targetMACAddress;

			sendData(compound);
		} // sendData

		virtual wns::service::dll::UnicastAddress
		getMACAddress() const
		{
			return sourceMACAddress_;
		}

	protected:
		wns::logger::Logger logger_;
		wns::service::dll::UnicastAddress sourceMACAddress_;

	private:
		/**
		 * @brief Needed for demultiplexing of upper layer protocols.
		 */
		typedef wns::container::Registry<wns::service::dll::protocolNumber, wns::service::dll::Handler*> DataHandlerRegistry;

		virtual void
		doOnData(const wns::ldk::CompoundPtr& compound)
		{
			MESSAGE_BEGIN(NORMAL, logger_, m, this->getFUN()->getName());
			m << ": doOnData(), forwarding to upper Component";
			MESSAGE_END();

			MESSAGE_BEGIN(VERBOSE, logger_, m, this->getFUN()->getName());
			m << ": Compound backtrace"
			  << compound->dumpJourney(); // JOURNEY
			MESSAGE_END();

			// Forward to upper layer
			try
			{
				wns::service::dll::protocolNumber protocolNr =
 					wns::service::dll::protocolNumberOf(compound->getData());

				wns::service::dll::Handler* handler = dataHandlerRegistry_.find(protocolNr);
;
				handler->onData(compound->getData());
			}
			catch(const DataHandlerRegistry::UnknownKeyValue& ukv)
			{
				wns::Exception e;
				e << "No handler for this upper layer protocol registered.\n";
				e << ukv;
				throw e;
			}
			catch(...)
			{
				throw;
			}
		} // doOnData

 		virtual void
 		doSendData(const wns::ldk::CompoundPtr& compound)
 		{
			if (this->isAccepting(compound)) {
				this->getConnector()->getAcceptor(compound)->sendData(compound);
			}
 		} // doSendData

		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& compound) const
		{
			return this->getConnector()->hasAcceptor(compound);
		} // doIsAccepting

		virtual void
		doWakeup()
		{
			getReceptor()->wakeup();
		} // doWakeUp

		/**
		 * @brief Registry for datahandlers. Each datahandler is select
		 * by the protocol number.
		 */
		DataHandlerRegistry dataHandlerRegistry_;
	}; // class Upper

	/**
	 * @brief Provides unicast service for IP
	 */
	class UnicastUpper :
		public Upper<UnicastUpperCommand>
	{
	public:
		/**
		 * @brief Constructor
		 */
		UnicastUpper(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
			Upper<UnicastUpperCommand>(fun, config)
		{
		} // UnicastUpper

		/**
		 * @brief Not implemented for broadcast ( assure(false, ....) )
		 */
		virtual wns::ldk::CommandPool*
		createReply(const wns::ldk::CommandPool* original) const;
	}; // UnicastUpper

	/**
	 * @brief Provides broadcast service for IP (also used by other FUs
	 * inside GLUE)
	 */
	class BroadcastUpper :
		public Upper<BroadcastUpperCommand>
	{
	public:
		/**
		 * @brief Constructor
		 */
		BroadcastUpper(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
			Upper<BroadcastUpperCommand>(fun, config)
		{
		} // BroadcastUpper

		/**
		 * @brief Not implemented for broadcast ( assure(false, ....) )
		 */
		virtual wns::ldk::CommandPool*
		createReply(const wns::ldk::CommandPool* original) const;
	}; // BroadcastUpper
}
}

#endif // NOT defined GLUE_CONVERGENCE_UPPER_HPP
