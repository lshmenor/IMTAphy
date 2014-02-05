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

#ifndef WNS_LDK_TOOLS_BRIDGE_HPP
#define WNS_LDK_TOOLS_BRIDGE_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

namespace wns { namespace ldk { namespace tools {

	namespace bridge
	{
		class Forwarder :
			public virtual FunctionalUnit,
			public wns::ldk::CommandTypeSpecifier<>,
			public wns::ldk::HasReceptor<>,
			public wns::ldk::HasConnector<>,
			public wns::ldk::HasDeliverer<>,
			public Cloneable<Forwarder>
		{
		public:
			Forwarder(fun::FUN* fuNet, double _loss = 0.0) :
					wns::ldk::CommandTypeSpecifier<>(fuNet),
					wns::ldk::HasReceptor<>(),
					wns::ldk::HasConnector<>(),
					wns::ldk::HasDeliverer<>(),
					Cloneable<Forwarder>(),

					other(NULL),
					loss(_loss)
			{}

			void
			link(Forwarder* _other)
			{
				other = _other;
			}

			virtual void
			doOnData(const wns::ldk::CompoundPtr& compound)
			{
				if((rand() / (RAND_MAX + 1.0)) < loss) {
					return;
				}

				// there must always be an accepting lower functionalUnit.
				// no buffering here.
				other->getConnector()->getAcceptor(compound)->sendData(compound);
			} // doOnData


			virtual void
			doSendData(const wns::ldk::CompoundPtr& compound)
			{
				if((rand() / (RAND_MAX + 1.0)) < loss) {
					return;
				}

				other->getDeliverer()->getAcceptor(compound)->onData(compound);
			} // doSendData

			void
			setLoss(double _loss)
			{
				assure(_loss >= 0.0 && _loss <= 1.0, "Invalid loss value.");

				loss = _loss;
			}

		private:
			virtual bool
			doIsAccepting(const wns::ldk::CompoundPtr& /* compound */ ) const
			{
				return true;
			}

			virtual void
			doWakeup()
			{}

			FunctionalUnit* other;
			double loss;
		};
	}


	/**
	 * @brief (Possibly lossy) double-ended bridge.
	 *
	 * A pair of two functional units, that forward compounds, where double-ended means:
	 * @li Each FunctionalUnit injects the compounds received via doOnData to the other FunctionalUnit
	 *     using doSendData.
	 * @li Each FunctionalUnit injects the compounds received via doSendData to the other FunctionalUnit
	 *     using doOnData.
	 *
	 * Use this FunctionalUnit as shortcut during testing. You may as well find it useful
	 * to configure the Brige to be lossy.
	 */
	class Bridge
	{
	public:
		/**
		 * A bridge consists of two functional units, which may even be part of different
		 * FUNs. We imagine one of these functional units to be on the left side and one on
		 * the right side.
		 * <br>
		 * To create the two functional units, you have to provide the FUNs the functional units
		 * shall be part of.
		 * <br>
		 * Additionally you may choose to set a packet loss.
		 */
		Bridge(fun::FUN* leftFUN, fun::FUN* rightFUN, double loss = 0.0);
		virtual ~Bridge();

		/**
		 * @brief Return the 'left' FunctionalUnit.
		 *
		 */
		bridge::Forwarder* getLeft() const
			{
				return a;
			}

		/**
		 * @brief Return the 'right' FunctionalUnit.
		 *
		 */
		bridge::Forwarder* getRight() const
			{
				return b;
			}

		/**
		 * @brief Set the link quality.
		 *
		 * Link quality is set in terms of packet loss ratio.
		 */
		void setLoss(double loss)
			{
				a->setLoss(loss);
				b->setLoss(loss);
			}

	private:
		bridge::Forwarder *a;
		bridge::Forwarder *b;
	};

}}}


#endif // NOT defined WNS_LDK_TOOLS_BRIDGE_HPP


