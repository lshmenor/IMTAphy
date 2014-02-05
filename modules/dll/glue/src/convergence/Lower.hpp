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

#ifndef GLUE_CONVERGENCE_LOWER_HPP
#define GLUE_CONVERGENCE_LOWER_HPP

#include <GLUE/Component.hpp>
#include <GLUE/BERProvider.hpp>

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/ErrorRateProviderInterface.hpp>

namespace glue { namespace convergence {

	class LowerCommand :
		public wns::ldk::Command,
		virtual public wns::ldk::ErrorRateProviderInterface
	{
		friend class Lower;
	public:
		double getErrorRate() const
		{
			return local.per;
		}

		struct {
			double per;
		} local;
		struct {} peer;
		struct {
            wns::simulator::Time txStartTime;
            StationType senderType;
        } magic;
	};

	class Lower :
		virtual public wns::ldk::FunctionalUnit,
		public glue::BERProvider
	{
	public:
		virtual void
		setNotificationService(wns::service::Service* phy) = 0;

		virtual wns::service::Service*
		getNotificationService() const = 0;

		virtual void
		setDataTransmissionService(wns::service::Service* phy) = 0;

		virtual wns::service::Service*
		getDataTransmissionService() const = 0;

 		virtual void
		setMACAddress(const wns::service::dll::UnicastAddress& address) = 0;
	};


} // convergence
} // glue


#endif // NOT defined GLUE_CONVERGENCE_LOWER_HPP


