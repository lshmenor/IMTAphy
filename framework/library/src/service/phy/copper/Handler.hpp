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

#ifndef WNS_SERVICE_PHY_COPPER_HANDLER_HPP
#define WNS_SERVICE_PHY_COPPER_HANDLER_HPP

#include <WNS/osi/PDU.hpp>

namespace wns { namespace service { namespace phy { namespace copper {

	/**
	 * Triggers that may be issued by copper
	 *
	 * Each trigger here is implemented as a class with a respective method,
	 * which may be called by copper PHY and must be handled by the
	 * implementation in DLL.
	 */

	/**
	 * @brief Interface for wireline receiver
	 *
	 * Phy Copper components need a callback to deliver incoming data.
	 * Users of @ref wns::node::component::phy::copper::Service can register
	 * such a callback by calling @ref
	 * wns::node::component::phy::copper::Service::registerDataHandler and
	 * passing an implementation of @ref
	 * wns::node::component::phy::copper::DataHandler
	 */
	class Handler
	{
	public:
		virtual
		~Handler()
		{}

		/**
		 * @brief Receive data from lower PHY layer.
		 *
		 * @param[in] data Data that is delivered to the DataHandler.
		 * @param[in] ber Bit Error Rate of this PDU
		 * @param[in] collision Collision on the wire?
		 *
		 * @note BER and collision on the wire are handled
		 * seperately. If a higher layer detects a collision, it should
		 * throw away the packet ...
		 */
		virtual void
		onData(
			const wns::osi::PDUPtr& data,
			double ber,
			bool collision) = 0;

	};

} // copper
} // phy
} // service
} // wns


#endif // WNS_SERVICE_PHY_COPPER_HANDLER_HPP


