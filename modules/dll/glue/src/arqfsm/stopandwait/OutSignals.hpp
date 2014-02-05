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

#ifndef GLUE_ARQFSM_STOPANDWAIT_OUTSIGNALS_HPP
#define GLUE_ARQFSM_STOPANDWAIT_OUTSIGNALS_HPP

#include <WNS/ldk/Compound.hpp>

namespace glue { namespace arqfsm { namespace stopandwait {

	class OutSignals
	{
	public:
		virtual void
		sendsendData(const wns::ldk::CompoundPtr& compound) = 0;

		virtual void
		sendonData(const wns::ldk::CompoundPtr& compound) = 0;

		virtual void
		sendWakeup() = 0;

	protected:
		virtual
		~OutSignals()
		{}
	};

} // stopandwait
} // arqfsm
} // glue

#endif // GLUE_ARQFSM_STOPANDWAIT_OUTSIGNALS_HPP


