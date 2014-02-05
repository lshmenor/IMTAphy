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

#ifndef GLUE_ROUTING_HPP
#define GLUE_ROUTING_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/Classifier.hpp>

#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue {

	namespace convergence {
		class UnicastUpper;
	} // convergence

	class RoutingCommand :
		public wns::ldk::Command
	{
	public:
 		struct {} local;
 		struct {} peer;
 		struct {} magic;
	};

	/**
	 * @brief Determines target address for Compounds to be sent from
	 * first Compound received
	 *
	 * This FU will take the first source address it sees from an
	 * incoming Compound and set this address as targetAddress. Every
	 * Compound that has no target address set will be tagged by this FU
	 * with the targetAddress.
	 *
	 * @note This FU is rather a hack. We (msg, swen) are currently working
	 * on a better solution ...
	 */
	class Routing :
		virtual public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<RoutingCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<Routing>
	{
	public:
		Routing(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config);

		virtual
		~Routing();

	private:
		virtual void
		doSendData(const wns::ldk::CompoundPtr& compound);

		virtual void
		doOnData(const wns::ldk::CompoundPtr& compound);

		virtual void
		onFUNCreated();

		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

		virtual void
		doWakeup()
		{
			getReceptor()->wakeup();
		} // doWakeup

		bool allowRouteChange;

		wns::ldk::CommandPool* routingCommandPool;

 		struct Friends
 		{
 			wns::ldk::FunctionalUnit* addressProviderFU;
 		} friends;

		wns::pyconfig::View config;
		wns::logger::Logger logger;
	};
} // glue

#endif // NOT defined GLUE_ROUTING_HPP


