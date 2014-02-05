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

#ifndef GLUE_TRIGGER_TRIGGER_HPP
#define GLUE_TRIGGER_TRIGGER_HPP

#include <GLUE/BERConsumer.hpp>

#include <GLUE/trigger/FunctionalUnitLight.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace wns { namespace ldk { namespace sar {

	class Fixed;
} // sar
} // ldk
} // wns

namespace glue {

	class BERProvider;

	namespace trigger {

	class Trigger :
		virtual public glue::trigger::FunctionalUnitLight,
		public glue::BERConsumer
	{
	public:
		Trigger(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config);

		virtual
		~Trigger();

		virtual void
		onFUNCreated();

		virtual void
		onBERProviderDeleted();

		virtual void
		onNewMeasurement(double BER, int packetSize);

	private:

		bool connected;

		struct Friends
		{
			BERProvider* berProvider;
			wns::ldk::sar::Fixed* sar;
		} friends;

		wns::pyconfig::View config;
		wns::logger::Logger logger;

		int packetHeaderSize;
	}; // Trigger

} // trigger
} // glue

#endif // NOT defined GLUE_TRIGGER_TRIGGER_HPP


