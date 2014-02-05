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

#ifndef WNS_SERVICE_PHY_COPPER_CARRIERSENSING_HPP
#define WNS_SERVICE_PHY_COPPER_CARRIERSENSING_HPP

namespace wns { namespace service { namespace phy { namespace copper {

	class CarrierSensing
	{
	public:
		/**
		 * @brief Destructor
		 */
		virtual
		~CarrierSensing()
		{}

		/**
		 * @brief called from PHY implementation if carrier is idle
		 * after it has been busy
		 */
		virtual void
		onCarrierIdle() = 0;

		/**
		 * @brief called from PHY implementation if carrier is busy
		 * after it has been idle
		 */
		virtual void
		onCarrierBusy() = 0;

		/**
		 * @brief called from PHY implementation if collision happened
		 */
		virtual void
		onCollision() = 0;
	};

} // copper
} // phy
} // service
} // wns

#endif // WNS_SERVICE_PHY_CARRIERSENSING_HPP


