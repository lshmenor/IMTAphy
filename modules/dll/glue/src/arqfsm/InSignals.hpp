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

#ifndef GLUE_ARQFSM_INSIGNALS_HPP
#define GLUE_ARQFSM_INSIGNALS_HPP

#include <WNS/ldk/Compound.hpp>

namespace glue { namespace arqfsm {

	template <typename STATEINTERFACE>
	class InSignals
	{
	public:
		virtual STATEINTERFACE*
		onSendData(const wns::ldk::CompoundPtr& compound) = 0;

		virtual STATEINTERFACE*
		onI(const wns::ldk::CompoundPtr& compound, int sequenceNumber) = 0;

		virtual STATEINTERFACE*
		onACK(int sequenceNumber) = 0;

		virtual STATEINTERFACE*
		onWakeup() = 0;

		virtual void
		onIsAccepting(bool& accepting) const = 0;

	protected:
		virtual
		~InSignals()
		{} // ~InSignals
	};

} // arqfsm
} // glue

#endif // GLUE_ARQFSM_INSIGNALS_HPP


