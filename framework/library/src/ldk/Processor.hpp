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

#ifndef WNS_LDK_PROCESSOR_HPP
#define WNS_LDK_PROCESSOR_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Receptor.hpp>

namespace wns { namespace ldk {

	/**
	 * @brief Interface to be implemented by users of the Processor adapter.
	 * @ingroup compoundhandler
	 *
	 * To implement functional units that have no internal memory and do PDU mutation only,
	 * you may use the Processor adapter to get a more suitable interface.
	 * <br>
	 * @note functional units that drop compounds can't use the Processor adapter, since
	 *   every received PDU is expected to be delivered again. Use the Dropper
	 *   adapter instead.
	 *
	 */
	struct ProcessorInterface :
		virtual public FunctionalUnit
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
	};


	/**
	 * @brief Adapter implementing the CompoundHandlerInterface for processing-only functional units.
	 * @ingroup compoundhandler
	 *
	 * For documentation see ProcessorInterface.
	 */
	template <typename USER>
	class Processor :
		public virtual FunctionalUnit,
		public virtual ProcessorInterface
	{
	public:
		Processor() :
			inWakeup(false)
		{}

		virtual void
		doSendData(const CompoundPtr& compound)
		{
			processOutgoing(compound);

			getConnector()->getAcceptor(compound)->sendData(compound);
		} // doSendData


		virtual void
		doOnData(const CompoundPtr& compound)
		{
			processIncoming(compound);

			if(getDeliverer()->size())
				getDeliverer()->getAcceptor(compound)->onData(compound);
		} // doOnData


	protected:
		virtual bool
		doIsAccepting(const CompoundPtr& compound) const
		{
			return getConnector()->hasAcceptor(compound);
		} // isAccepting

	private:

		virtual void
		doWakeup()
		{
			if (!inWakeup)
			{
				inWakeup = true;
				getReceptor()->wakeup();
				inWakeup = false;
			}
		} // wakeup

		bool inWakeup;
	};
}}


#endif // NOT defined WNS_LDK_PROCESSOR_HPP


