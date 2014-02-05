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

#ifndef WNS_SERVICE_PHY_COPPER_NOTIFICATION_HPP
#define WNS_SERVICE_PHY_COPPER_NOTIFICATION_HPP

#include <WNS/service/Service.hpp>
#include <WNS/service/dll/Address.hpp>

#include <WNS/service/phy/copper/CarrierSensing.hpp>
#include <WNS/service/phy/copper/Handler.hpp>

#include <WNS/Subject.hpp>
#include <WNS/Observer.hpp>

namespace wns { namespace service { namespace phy { namespace copper {

	class Notification :
		virtual public wns::service::Service,
		virtual public wns::Subject<CarrierSensing>,
		virtual public wns::Subject<Handler>
	{
	public:
		/**
		 * @brief This is used to set the address we want the copper to
		 * listen to.
		 *
		 * In reality this is of course not handled this way. There, the
		 * MAC is filtering the packets. But for efficiency reasons this
		 * "hint" is provided to the PHY.
		 */
		virtual void
		setDLLUnicastAddress(const wns::service::dll::UnicastAddress& macAddress) = 0;
	};

} // copper
} // phy
} // service
} // wns


#endif // WNS_SERVICE_PHY_COPPER_NOTIFICATION_HPP


