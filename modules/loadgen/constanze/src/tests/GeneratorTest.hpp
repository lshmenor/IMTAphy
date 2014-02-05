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

#ifndef CONSTANZE_GENERATORTEST_HPP
#define CONSTANZE_GENERATORTEST_HPP

#include <CONSTANZE/GeneratorPP.hpp>
#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/SubGenerator.hpp>
#include <CONSTANZE/Binding.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>
#include <WNS/node/tests/Stub.hpp>
#include <WNS/CppUnit.hpp>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>
#include <iomanip>

namespace constanze { namespace tests {

	/** @brief helper class for GeneratorTest */
	class BindingStub:
		public constanze::Binding
	{
	public:
        BindingStub();
        BindingStub(const wns::pyconfig::View& _pyco);
		virtual ~BindingStub();
		virtual void registerComponent(wns::node::component::Component* _component);
		virtual void sendData(const wns::osi::PDUPtr& _data);
		virtual void initBinding(constanze::StartTrigger* _startTrigger);
		virtual void releaseBinding(constanze::StopTrigger* _stopTrigger);
		virtual void registerListener(constanze::Listener*);
		unsigned long int SentDataCounter;
		unsigned int initBindingCounter;
		unsigned int releaseBindingCounter;
	protected:
		//wns::node::component::Component* component;
		virtual std::string printAddress() const;
	};

} // tests
} // constanze

#endif // NOT defined CONSTANZE_GENERATORTEST_HPP


