/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef WNS_LDK_TOOLS_CONSUMER_HPP
#define WNS_LDK_TOOLS_CONSUMER_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Forwarding.hpp>

#include <WNS/pyconfig/View.hpp>

#include <list>

namespace wns { namespace ldk { namespace tools {

	/**
	 * @brief The packet-eater.
	 *
	 * Drops packets in uplink and/or downlink. Use this during testing at the
	 * bottom or top of a stack where you don't care about the actual compounds
	 * received/sent (IF you care, you should have a look at the Stub).
	 *
	 */
	class Consumer :
		public virtual FunctionalUnit,
		public CommandTypeSpecifier<>,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>,
		public Cloneable<Consumer>
	{
	public:
		typedef std::list<CompoundPtr> ContainerType;

		Consumer(fun::FUN* fuNet, bool _incoming = false, bool _outgoing = false) :
				CommandTypeSpecifier<>(fuNet),
				HasReceptor<>(),
				HasConnector<>(),
				HasDeliverer<>(),
				Cloneable<Consumer>(),

				incoming(_incoming),
				outgoing(_outgoing)
			{}

		Consumer(fun::FUN*, const wns::pyconfig::View&);

		/**
		 * @brief Configure the Consumer whether to drop incoming packets.
		 *
		 */
		void
		consumeIncoming(bool choice = true)
		{
			incoming = choice;
		} // consumeIncoming


		/**
		 * @brief Configure the Consumer whether to drop outgoing packets.
		 *
		 */
		void
		consumeOutgoing(bool choice = true)
		{
			outgoing = choice;
		} // consumeOutgoing

		virtual void
		doSendData(const CompoundPtr& compound)
		{
			if(outgoing) {
				return;
			}

			getConnector()->getAcceptor(compound)->sendData(compound);
		} // doSendData


		virtual void
		doOnData(const CompoundPtr& compound)
		{
			if(incoming) {
				return;
			}

			getDeliverer()->getAcceptor(compound)->onData(compound);
		} // doOnData

	private:

		//
		// compound handler interface
		//
		virtual bool
		doIsAccepting(const CompoundPtr& compound) const
		{
			if(outgoing)
				return true;

			return getConnector()->hasAcceptor(compound);
		} // isAccepting

		virtual void
		doWakeup()
		{
			getReceptor()->wakeup();
		} // wakeup

		bool incoming;
		bool outgoing;
	};

}}}


#endif // NOT defined WNS_LDK_TOOLS_CONSUMER_HPP


