/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef GLUE_ARQFSM_OUTSIGNALS_HPP
#define GLUE_ARQFSM_OUTSIGNALS_HPP

#include <WNS/ldk/Compound.hpp>

namespace glue { namespace arqfsm {

	class OutSignals
	{
	public:
		virtual void
		sendSendData(wns::ldk::CompoundPtr compound) = 0;

		virtual void
		sendOnData(wns::ldk::CompoundPtr compound) = 0;

		virtual void
		sendWakeup() = 0;

	protected:
		virtual
		~OutSignals()
		{} // ~OutSignals
	};

} // arqfsm
} // glue

#endif // GLUE_ARQFSM_OUTSIGNALS_HPP


