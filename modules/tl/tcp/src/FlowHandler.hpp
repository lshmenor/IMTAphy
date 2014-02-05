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

#ifndef TCP_FLOWHANDLER_HPP
#define TCP_FLOWHANDLER_HPP

#include <WNS/service/dll/Handler.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/service/tl/FlowID.hpp>
#include <TCP/Service.hpp>

namespace tcp {

	class Service;
	/**
	 * @brief DLL FlowHandler interface.
	 *
	 * DLL components need a callback to notify the establishment of a flow
	 * to higher layers. Users of
	 * @ref wns::service::dll::FlowEstablishmentAndRelease can register such
	 * a callback by calling
	 * @ref wns::service::dll::FlowEstablishmentAndRelease::registerFlowHandler
	 * and passing an implementation of @ref wns::service::dll::FlowHandler
	 */
	class FlowHandler :
		public wns::service::dll::FlowHandler
	{
	public:

		std::string serviceName;
		wns::logger::Logger logger;
		 /**
		 * @brief Constructor
		 */
		FlowHandler();

		/**
		 * @brief Destructor
		 */
		~FlowHandler() {};

		/**
		 * @brief Receive notificiation about flow establishment from
		 * lower DLL layer.
		 */
		void
		onFlowEstablished(wns::service::tl::FlowID flowID, int _dllFlowID);

		/**
		 * @brief Receive notificiation about flow-changes from
		 * lower DLL layer. (new DllFlowID at HandOver)
		 */
		void
		onFlowChanged(wns::service::tl::FlowID flowID, int _dllFlowID);

		void
		setTLService(tcp::Service* _tlService);
	private:
		tcp::Service* tlService;
	};

} // tcp


#endif //  TCP_FLOWHANDLER_HPP
