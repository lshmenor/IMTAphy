/******************************************************************************
 * DataHandler for Transport Layer Connections Interface                      *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/ *
 ******************************************************************************/

#ifndef WNS_SERVICE_TL_DATAHANDLER_HPP
#define WNS_SERVICE_TL_DATAHANDLER_HPP

#include <WNS/osi/PDU.hpp>

namespace wns { namespace service { namespace tl {

	/**
	 * @brief DataHandler interface.
	 *
	 * Once a connection is setup the DataHandler is registered with the
	 * connection and is used to receive incoming data from the transport.
	 */
	class DataHandler
	{
	public:
		/**
		 * @brief Destructor.
		 */
		virtual
		~DataHandler()
		{}

		/**
		 * @brief Called when new data arrived.
		 *
		 * @param[in] _pdu Newly arrived data.
		 */
		virtual void
		onData(const wns::osi::PDUPtr& _pdu) = 0;
	};
} // tl
} // service
} // wns


#endif //WNS_SERVICE_TL_DATAHANDLER_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
