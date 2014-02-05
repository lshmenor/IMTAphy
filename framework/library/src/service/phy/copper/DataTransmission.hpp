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

#ifndef WNS_SERVICE_PHY_COPPER_DATATRANSMISSION_HPP
#define WNS_SERVICE_PHY_COPPER_DATATRANSMISSION_HPP

#include <WNS/service/Service.hpp>
#include <WNS/service/dll/Address.hpp>

#include <WNS/osi/PDU.hpp>

namespace wns { namespace service { namespace phy { namespace copper {

	/**
	 * @brief Interface for a wireline transmitter
	 */
	class DataTransmission :
		public virtual wns::service::Service
	{
	public:
		/**
		 * @brief Broadcast transmission of data
		 *
		 * @param[in] macAddress The Broadcast address
		 * @param[in] data Protocol Data Unit to be transmitted
		 */
		virtual void
		sendData(
			const wns::service::dll::BroadcastAddress& macAddress,
			const wns::osi::PDUPtr& data) = 0;

		/**
		 * @brief Unicast transmission of data
		 *
		 * @param[in] macAddress MAC address of the peer entitiy
		 * @param[in] data Protocol Data Unit to be transmitted
		 */
		virtual void
		sendData(
			const wns::service::dll::UnicastAddress& macAddress,
			const wns::osi::PDUPtr& data) = 0;

		/**
		 * @brief Stop an ongoing transmission
		 */
		virtual void
		cancelData(
			const wns::osi::PDUPtr& data) = 0;

		/**
		 * @brief Indicates if the wire is available for transmission
		 *
		 * @return false if transmission is ongoing, true otherwise
		 *
		 * @note Depending on the implementation this method might
		 * behave differently for the same state of the wire. Consider
		 * the following: At time "t" a transmission is started by
		 * transmitter A. A second transmitter (B) wants to transmit
		 * data, too, at the same time. Normally transmitter B would not
		 * be able to detect that the wire is already occupied by
		 * another transmission. However, in order to have the ability
		 * to test simple Data Link Layer implementations (that assume
		 * coillision free transmissions) it might be useful to return
		 * "true" in this case. Please check the copper implementation
		 * you're using for details.
		 */
		virtual bool
		isFree() const = 0;
	};

} // copper
} // phy
} // service
} // wns

#endif //WNS_SERVICE_PHY_COPPER_DATATRANSMISSION_HPP



