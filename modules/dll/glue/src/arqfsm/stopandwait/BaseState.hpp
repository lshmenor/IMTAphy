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

#ifndef GLUE_ARQFSM_STOPANDWAIT_BASESTATE_HPP
#define GLUE_ARQFSM_STOPANDWAIT_BASESTATE_HPP

#include <GLUE/arqfsm/stopandwait/FSMFU.hpp>
#include <GLUE/arqfsm/ARQBaseState.hpp>

#include <WNS/ldk/Compound.hpp>

namespace glue { namespace arqfsm { namespace stopandwait {

	class BaseState :
		public ARQBaseState<FSMFU, StopAndWaitCommand>
	{
	public:

		BaseState(FSMFU::BaseFSM* t, const std::string& stateName) :
			ARQBaseState<FSMFU, StopAndWaitCommand>(t, stateName)
		{} // BaseState

		virtual StateInterface*
		onTimeout() = 0;

	protected:
		virtual
		~BaseState()
		{} // ~BaseState

	private:
		virtual StateInterface*
		onI(const wns::ldk::CompoundPtr& compound, int sequenceNumber);
	};

} // stopandwait
} // arqfsm
} // glue

#endif // GLUE_ARQFSM_STOPANDWAIT_BASESTATE_HPP


