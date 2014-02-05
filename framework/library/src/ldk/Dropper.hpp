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

#ifndef WNS_LDK_DROPPER_HPP
#define WNS_LDK_DROPPER_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Receptor.hpp>

namespace wns { namespace ldk {

	/**
	 * @brief Interface to be implemented by users of the Dropper adapter.
	 * @ingroup compoundhandler
	 *
	 * To implement functional units that have no internal memory and do PDU mutation
	 * and dropping only, you may use the Dropper adapter to get a more suitable
	 * interface.
	 * <br>
	 * @note functional units that inject compounds always need to have internal memory.
	 */
	struct DropperInterface :
		virtual public FunctionalUnit
	{
		/**
		 * @brief Decide whether to drop a PDU.
		 *
		 */
		virtual bool wouldDrop(const CompoundPtr& compound) const = 0;

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
		virtual void processOutgoing(const CompoundPtr& compound) = 0;

		/**
		 * @brief Process compounds before dropping them.
		 *
		 * Override this method to deal with dropped compounds.
		 */
		virtual void processDropping(const CompoundPtr& compound) = 0;
	};


	/**
	 * @brief Adapter implementing the CompoundHandlerInterface for processing-only functional units.
	 * @ingroup compoundhandler
	 *
	 * For documentation see DropperInterface.
	 */
	template <typename USER>
	class Dropper :
		public virtual FunctionalUnit,
		public virtual DropperInterface
	{
	public:

		virtual void
		doSendData(const CompoundPtr& compound)
		{
			assure(isAccepting(compound), "sendData called although the FunctionalUnit is not accepting!");
			assure(getConnector()->hasAcceptor(compound), "noone accepts the PDU i accepted.");

			if(wouldDrop(compound)) {
				processDropping(compound);
			} else {
				processOutgoing(compound);

				getConnector()->getAcceptor(compound)->sendData(compound);
			}
		} // doSendData


		virtual void
		doOnData(const CompoundPtr& compound)
		{
			processIncoming(compound);

			if(getDeliverer()->size())
				getDeliverer()->getAcceptor(compound)->onData(compound);
		} // doOnData


	private:
		virtual bool
		doIsAccepting(const CompoundPtr& compound) const
		{
			return wouldDrop(compound)
				|| getConnector()->hasAcceptor(compound);
		} // isAccepting


		virtual void
		doWakeup()
		{
			getReceptor()->wakeup();
		} // wakeup
	};
}}


#endif // NOT defined WNS_LDK_DROPPER_HPP


