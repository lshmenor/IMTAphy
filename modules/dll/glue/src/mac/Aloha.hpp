/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef GLUE_MAC_CSMACA_HPP
#define GLUE_MAC_CSMACA_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/distribution/Uniform.hpp>

namespace glue {

	class AlohaCommand :
		public wns::ldk::Command
	{
	public:
		struct {
		} local;

		struct {
		} peer;

		struct {
		} magic;
	};

	class Aloha :
		public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<AlohaCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<Aloha>
	{
	public:

		Aloha(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

		virtual
		~Aloha();

	private:
		// FunctionalUnit / CompoundHandlerInterface
		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

		virtual void
		doSendData(const wns::ldk::CompoundPtr& compound);

		virtual void
		doWakeup();

		virtual void
		doOnData(const wns::ldk::CompoundPtr& compound);

		virtual void
		allowTransmissionAfterElapsedBackoff();

		wns::logger::Logger logger;

                bool allowTransmission;

		wns::simulator::Time maxWait;

		wns::distribution::Uniform uniform;
	};

}

#endif // GLUE_MAC_CSMACA_HPP
