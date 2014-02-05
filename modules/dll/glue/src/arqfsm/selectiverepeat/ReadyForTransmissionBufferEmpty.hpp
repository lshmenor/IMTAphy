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

#ifndef GLUE_ARQFSM_SELECTIVEREPEAT_READYFORTRANSMISSIONBUFFEREMPTY_HPP
#define GLUE_ARQFSM_SELECTIVEREPEAT_READYFORTRANSMISSIONBUFFEREMPTY_HPP

#include <GLUE/arqfsm/selectiverepeat/BaseState.hpp>

namespace glue { namespace arqfsm { namespace selectiverepeat {

	class ReadyForTransmissionBufferEmpty :
		public BaseState
	{
	public :
		ReadyForTransmissionBufferEmpty(FSMFU::BaseFSM* t) :
			BaseState(t, "glue_arqfsm_selectiverepeat_ReadyForTransmissionBufferEmpty")
		{} // ReadyForTransmissionBufferEmpty

	private:
		virtual void
		onInitState();

		virtual StateInterface*
		onSendData(const wns::ldk::CompoundPtr& compound);

		virtual StateInterface*
		onACK(int sequenceNumber);

		virtual StateInterface*
		onWakeup();

		virtual void
		onIsAccepting(bool& accepting) const;

		virtual StateInterface*
		onTimeout(const wns::ldk::CompoundPtr& compoundBox);
	};

} // selectiverepeat
} // arqfsm
} // glue

#endif // GLUE_ARQFSM_SELECTIVEREPEAT_READYFORTRANSMISSIONBUFFEREMPTY_HPP


