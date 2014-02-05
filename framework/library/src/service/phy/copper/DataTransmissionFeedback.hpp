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

#ifndef WNS_SERVICE_PHY_COPPER_DATATRANSMISSIONFEEDBACK_HPP
#define WNS_SERVICE_PHY_COPPER_DATATRANSMISSIONFEEDBACK_HPP

#include <WNS/Subject.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/service/Service.hpp>


namespace wns { namespace service { namespace phy { namespace copper {

	class DataTransmissionFeedbackInterface
	{
	public:
		/**
		 * @brief Destructor
		 */
		virtual
		~DataTransmissionFeedbackInterface()
		{}

		/**
		 * @brief called from PHY implementation if data has been
		 * transmitted
		 *
		 * @note transmission may or may not have been successful
		 *
		 * @note if the transmission is (manually) canceled for
		 * whatsoever reason, this method is not called.
		 *
		 * @param pdu PDU that has just been sent
		 */
		virtual void
		onDataSent(wns::osi::PDUPtr pdu) = 0;
	};

	class DataTransmissionFeedback :
		virtual public wns::service::Service,
		virtual public wns::Subject<DataTransmissionFeedbackInterface>
	{
	};

} // copper
} // phy
} // service
} // wns

#endif // WNS_SERVICE_PHY_DATATRANSMISSIONFEEDBACK_HPP


