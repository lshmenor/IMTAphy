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


#ifndef CONSTANZE_BINDING_HPP
#define CONSTANZE_BINDING_HPP

#include <CONSTANZE/ConstanzeComponent.hpp>
#include <CONSTANZE/Generator.hpp>

#include <WNS/service/Service.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/node/component/Component.hpp>

#include <WNS/osi/PDU.hpp>

namespace constanze
{
	class Binding
	{
	public:

		typedef wns::PyConfigViewCreator<Binding, Binding> Creator;

		typedef wns::StaticFactory<Creator> Factory;

		virtual
		~Binding()
		{}

		virtual void
		registerComponent(wns::node::component::Component* _component) = 0;

		virtual void
		sendData(const wns::osi::PDUPtr& _data) = 0;

		virtual void
		initBinding(constanze::StartTrigger*) = 0;

		virtual void
		releaseBinding(constanze::StopTrigger*) = 0;

		virtual void
		registerListener(constanze::Listener* _listener) = 0;

		friend std::ostream&
		operator <<(std::ostream& os, const Binding& b)
		{
			os << b.printAddress();
			return os;
		}

	protected:
		virtual std::string
		printAddress() const = 0;

	}; //Binding

} //constanze

#endif // NOT defined CONSTANZE_BINDING_HPP
