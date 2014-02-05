/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#ifndef TCP_IPDATAHANDLER_HPP
#define TCP_IPDATAHANDLER_HPP

#include <WNS/service/nl/DataHandler.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>

namespace tcp {

	class UpperCommand;

	/**
	 * @brief Lower end of TCP (towards IP)
	 */
	class IPDataHandler :
		virtual public wns::service::nl::DataHandler
	{
	public:
		explicit
		IPDataHandler();

		virtual void
		onData(
			wns::service::nl::Address _sourceIP,
			const wns::osi::PDUPtr& _pdu);

		void
		setLowerConvergence(wns::ldk::FunctionalUnit* _lowerConvergence);

		wns::ldk::FunctionalUnit*
		getLowerConvergence() const;

	private:
		/**
		 * @brief My friends. Currently depending on the UpperConvergence command.
		 */
		struct Friends{
			wns::ldk::FunctionalUnit* uc;
		} friends;

		wns::ldk::FunctionalUnit* lowerConvergence;
	};
} // namespace tcp


#endif // NOT defined TCP_IPDATAHANDLER_HPP
