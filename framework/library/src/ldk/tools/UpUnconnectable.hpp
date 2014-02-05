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

#ifndef WNS_LDK_TOOLS_UNCONNECTABLE_HPP
#define WNS_LDK_TOOLS_UNCONNECTABLE_HPP

#include <sstream>
#include <WNS/ldk/FunctionalUnit.hpp>

namespace wns { namespace ldk { namespace tools {

	/**
	 * @brief Disables the connection mechanism from upside.
	 *
	 * Derive from this class if you want to disable the connection
	 * mechanism from upside. An Exception ist thrown if the Functional Unit
	 * is getting connected from upside.
	 */
	class UpUnconnectable :
		public virtual FunctionalUnit,
                public virtual CompoundHandlerInterface<FunctionalUnit>,
		public virtual HasDelivererInterface
	{
		virtual Deliverer*
		getDeliverer() const
		{
			throw wns::Exception("UpUnconnectable Functional Units do not have a deliverer");
		}

		virtual FunctionalUnit*
		whenConnecting()
		{
			std::ostringstream ss;
			ss << "Misconfiguration of Functional Unit " << this->getName() << std::endl;
			ss << "You must not connect to this Functional Unit from upside.";
			throw wns::Exception(ss.str());
			return 0;
		}

		virtual bool
		doIsAccepting(const CompoundPtr&) const
		{
			throw wns::Exception("Impossible to call isAccepting in UpUnconnectable FU");
		}

		virtual void
		doSendData(const CompoundPtr&)
		{
			throw wns::Exception("Impossible to call sendData in UpUnconnectable FU");
		}
	};
}}}

#endif



