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

#ifndef WNS_LDK_DELAYED_HPP
#define WNS_LDK_DELAYED_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Receptor.hpp>

namespace wns { namespace ldk {

	/**
	 * @brief Interface to be implemented by users of the Delayed adapter.
	 * @ingroup compoundhandler
	 *
	 * When implementing functional units, you frequently have to implement functional units with
	 * some internal memory for compounds. To ease implementation of intra-layer flow
	 * control, Delayed implements the CompoundHandlerInterface, and offers a different
	 * interface:
	 * @li DelayedInterface::hasCapacity
	 * @li DelayedInterface::processIncoming
	 * @li DelayedInterface::processOutgoing
	 * @li DelayedInterface::hasSomethingToSend
	 * @li DelayedInterface::getSomethingToSend
	 *
	 * If you derive from Delayed, you have to provide all of the above methods.
	 * <br>
	 * There is one additional method of interest provided by Delayed:
	 * Delayed::tryToSend.
	 * You will need that method, in cases where your FunctionalUnit spontaneously changes
	 * state and wishes to send (usually after a timeout).
	 *
	 */
	struct DelayedInterface :
		public virtual FunctionalUnit
	{
		/**
		 * @brief Process compounds on uplink.
		 *
		 * Override this method to handle incoming compounds.
		 *
		 */
		virtual void processIncoming(const CompoundPtr& compound) = 0;

		/**
		 * @brief Process compounds on downlink.
		 *
		 * Override this method to handle outgoing compounds.
		 */
		virtual void processOutgoing(const CompoundPtr&) = 0;

		/**
		 * @brief Return true, if the FunctionalUnit is still accepting compounds.
		 *
		 * While hasCapacity returns false, processIncoming will never be called.
		 */
		virtual bool hasCapacity() const = 0;

		/**
		 * @brief Return a PDU that is ready to be sent or NULL, if none available.
		 *
		 */
		virtual const CompoundPtr hasSomethingToSend() const = 0;

		/**
		 * @brief Return the PDU that is ready to be sent.
		 *
		 * This method must never return NULL. This method will only be called after
		 * a call to hasSomethingToSend returned a non-null value. The FunctionalUnit
		 * must return the same PDU as returned by hasSomethingToSend.
		 */
		virtual CompoundPtr getSomethingToSend() = 0;
	};


	/**
	 * @brief Adapter implementing the CompoundHandlerInterface for functional units that have memory.
	 * @ingroup compoundhandler
	 *
	 * For documentation see DelayedInterface.
	 */
	template <typename USER>
	class Delayed :
		public virtual FunctionalUnit,
		public virtual DelayedInterface
	{
	public:
		Delayed()
				: inWakeup(false)
			{}


		/** Part of CompoundHandlerInterface implementation. */
		virtual void
		doSendData(const CompoundPtr& compound)
		{
			processOutgoing(compound);

			tryToSend();
		} // doSendData


		/** Part of CompoundHandlerInterface implementation. */
		virtual void
		doOnData(const CompoundPtr& compound)
		{
			processIncoming(compound);

			tryToSend();
		} // doOnData

	protected:
		/**
		 * @brief Try to send a single PDU.
		 *
		 * Use this method if the FunctionalUnit changes state spontanteously. That is, if
		 * it may have compounds ready to send while not in processIncoming/processOutgoing.
		 * <br>
		 * For example functional units that send compounds after a timeout have to inform the
		 * Delayed implementation that they are to be queried again for compounds to be sent.
		 */
		void
		tryToSend()
		{
			// first, we try to send as many compounds from our internal memory as possible. if there
			// is no PDU left to send, we wake up all our upper layers.
			// waking up upper layers may result in doSendData calls to this layer, which ends up
			// in another call to this method. if functionalUnit capacities are high, recursion depth
			// may become an issue.
			while(tryToSendOnce());

			if(!inWakeup && hasCapacity()) {
				inWakeup = true;
				getReceptor()->wakeup();
				inWakeup = false;
			}
		} // tryToSend

		/** Part of CompoundHandlerInterface implementation. */
		virtual bool
		doIsAccepting(const CompoundPtr& /* compound */) const
		{
			return hasCapacity();
		} // isAccepting


	private:
		//
		// CompoundHandlerInterface implementation
		//

		/** Part of CompoundHandlerInterface implementation. */
		virtual void
		doWakeup()
		{
			tryToSend();
		} // wakeup

		bool
		tryToSendOnce()
		{
			const CompoundPtr compound = hasSomethingToSend();
			if(compound == CompoundPtr())
				return false;

			if(!getConnector()->hasAcceptor(compound))
				return false;

			IConnectorReceptacle* target = getConnector()->getAcceptor(compound);
			target->sendData(getSomethingToSend());

			return true;
		} // tryToSendOnce

		bool inWakeup;
	};
}}


#endif // NOT defined WNS_LDK_DELAYED_HPP


