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

#ifndef GLUE_TRIGGER_FUNCTIONALUNITLIGHT_HPP
#define GLUE_TRIGGER_FUNCTIONALUNITLIGHT_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/Cloneable.hpp>
#include <WNS/Exception.hpp>

namespace glue { namespace trigger {

	class FunctionalUnitLight :
		virtual public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<FunctionalUnitLight>
	{
	public:
		FunctionalUnitLight(wns::ldk::fun::FUN* fun, const wns::pyconfig::View&) :
			wns::ldk::CommandTypeSpecifier<>(fun),
			wns::ldk::HasReceptor<>(),
			wns::ldk::HasConnector<>(),
			wns::ldk::HasDeliverer<>(),
			wns::Cloneable<FunctionalUnitLight>()
		{}

		virtual void
		doSendData(const wns::ldk::CompoundPtr&)
		{
			throw wns::Exception("sendData(...) of FunctionalUnitLight must not be called.");
		} // doSendData

		virtual void
		doOnData(const wns::ldk::CompoundPtr&)
		{
			throw wns::Exception("onData(...) of FunctionalUnitLight must not be called.");
		} // doOnData

	private:
		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr&) const
		{
			throw wns::Exception("isAccepting(...) of FunctionalUnitLight must not be called.");
			return false;
		} // isAccepting

		virtual void
		doWakeup()
		{
			throw wns::Exception("wakeup(...) of FunctionalUnitLight must not be called.");
		} // wakeup
	}; // FunctionalUnitLight

} // trigger
} // glue

#endif // NOT defined GLUE_TRIGGER_FUNCTIONALUNITLIGHT_HPP


