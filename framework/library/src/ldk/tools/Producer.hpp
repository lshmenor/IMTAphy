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

#ifndef WNS_LDK_TOOLS_PRODUCER_HPP
#define WNS_LDK_TOOLS_PRODUCER_HPP

#include <WNS/ldk/fun/FUN.hpp>

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/pyconfig/View.hpp>

#include <list>

namespace wns { namespace ldk { namespace tools {

	/**
	 * @brief Creates compounds and tries to deliver them.
	 *
	 * Use this FunctionalUnit for testing on top of your test stack.
	 * <br>
	 * Make sure, that noone calls doSendData of a Producer. Producer is a FunctionalUnit
	 * to sit on top of a stack.
	 * <br>
	 * @todo currently, Producer creates Compounds with a fresh Command. But
	 * it would be nicer to have policies for the creation of Compounds. Another
	 * useful creational policy could follow the prototype pattern: Give
	 * the producer a PDU to make copies of... Or maybe something even
	 * smarter?
	 *
	 */
	class Producer :
		public virtual FunctionalUnit,
		public CommandTypeSpecifier<>,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>,
		public Cloneable<Producer>
	{
	public:
		typedef std::list<CompoundPtr> ContainerType;

		Producer(fun::FUN* fuNet) :
				CommandTypeSpecifier<>(fuNet),
				HasReceptor<>(),
				HasConnector<>(),
				HasDeliverer<>(),
				Cloneable<Producer>(),

				sent(),
				buffer()
			{
				allocate();
			}

		~Producer()
			{
			}

		virtual void
		doSendData(const CompoundPtr& /* compound */)
		{
			assure(false, "sendData of Producer called");
		} // doSendData


		virtual void
		doOnData(const CompoundPtr& compound)
		{
			getDeliverer()->getAcceptor(compound)->onData(compound);
			wakeup();
		} // doOnData

		unsigned long int sent;

	private:
		//
		// compound handler interface
		//
		virtual bool
		doIsAccepting(const CompoundPtr& /* compound */) const
		{
			return false;
		} // isAccepting

		virtual void
		doWakeup()
		{
			while(getConnector()->hasAcceptor(buffer)) {
				CompoundPtr it = buffer;
				allocate();
				getConnector()->getAcceptor(it)->sendData(it);
				++sent;
			}
		} // wakeup

		void allocate()
		{
			buffer = getFUN()->createCompound();
		} // allocate

		CompoundPtr buffer;
	};

}}}


#endif // NOT defined WNS_LDK_TOOLS_PRODUCER_HPP


