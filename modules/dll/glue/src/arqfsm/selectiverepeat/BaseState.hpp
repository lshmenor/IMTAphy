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

#ifndef GLUE_ARQFSM_SELECTIVEREPEAT_BASESTATE_HPP
#define GLUE_ARQFSM_SELECTIVEREPEAT_BASESTATE_HPP

#include <GLUE/arqfsm/selectiverepeat/FSMFU.hpp>
#include <GLUE/arqfsm/ARQBaseState.hpp>

#include <WNS/ldk/Compound.hpp>

namespace glue { namespace arqfsm { namespace selectiverepeat {

	class BaseState :
		public ARQBaseState<FSMFU, SelectiveRepeatCommand>
	{
	public:
		BaseState(FSMFU::BaseFSM* t, const std::string& stateName) :
			ARQBaseState<FSMFU, SelectiveRepeatCommand>(t, stateName)
		{} // BaseState

		virtual
		~BaseState()
		{} // ~BaseState

		virtual void
		onInitState()
		{} // onInitState

		virtual void
		onExitState()
		{} // onExitState

		virtual StateInterface*
		onTimeout(const wns::ldk::CompoundPtr& compound) = 0;

		virtual void
		deliverCompounds();

	protected:
		virtual void
		tryToSendACKs();

		virtual void
		tryToSendIs();

	private:
		virtual StateInterface*
		onI(const wns::ldk::CompoundPtr& compound, int sequenceNumber);

		bool
		pendingACK(int sequenceNumber);
	};

} // selectiverepeat
} // arqfsm
} // glue

#endif // GLUE_ARQFSM_SELECTIVEREPEAT_BASESTATE_HPP


