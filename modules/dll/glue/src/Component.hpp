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

#ifndef GLUE_COMPONENT_HPP
#define GLUE_COMPONENT_HPP

#include <WNS/node/component/Component.hpp>

#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/service/dll/Handler.hpp>

#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/CommandTypeSpecifierInterface.hpp>
#include <WNS/ldk/CompoundHandlerInterface.hpp>
#include <WNS/ldk/Layer.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Enum.hpp>

#include <vector>

namespace glue { namespace convergence {
	class UnicastUpper;
	class BroadcastUpper;
	class Lower;
} // convergence
} // glue

namespace glue {

    /**
     * @brief Definition of station types
     */
    ENUM_BEGIN(StationTypes);
    ENUM(router,  1);
    ENUM(client, 2);
    ENUM_END();

    typedef int StationType;


	/**
	 * @brief Offers a DLL service to higher layers
	 *
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 *
	 * This component will not offer the service in terms of providing the
	 * respective interfaces and implementations. It will rather
	 * setup/configure and number of convergence units that are responsible
	 * for this task.
	 */
	class Component :
		virtual public wns::ldk::Layer,
		public wns::node::component::Component
	{
	public:
		/**
		 * @brief Constructor
		 */
		Component(
			wns::node::Interface* _node,
			const wns::pyconfig::View& _config);

		/**
		 * @brief Destructor
		 */
		virtual
		~Component();

		/**
		 * @name Component Interface
		 * @brief Implements wns::node::component::Interface
		 */
		//@{
		/**
		 * @brief Implements wns::node::component::Interface::onNodeCreated()
		 */
		virtual void
		onNodeCreated();

		/**
		 * @brief Implements wns::node::component::Interface::onWorldCreated()
		 */
		virtual void
		onWorldCreated();

		/**
		 * @brief Implements wns::node::component::Interface::onShutdown()
		 */
		virtual void
		onShutdown();

        /**
            * @brief Indicate the station type, i.e. access point or user terminal.
            *
            * The station type IDs correspond to the enum StationType.
            */
        StationType getStationType() const
        {
            return stationType_;
        }

		//@}
	protected:

		/**
		 * @brief The main FUN of this component
		 */
		wns::ldk::fun::Main* fun;

		/**
		 * @brief Announces the services
		 */
		virtual void
		doStartup();

	private:
		/**
		 * @brief Disallow copy constructor
		 */
		Component(const Component&);

		/**
		 * @brief Disallow assignment
		 */
		Component& operator=(const Component&);

		/**
		 * @brief Component configuration
		 */
		wns::pyconfig::View config;

		/**
		 * @brief Service Access Point for Unicast Transmission
		 */
		convergence::UnicastUpper* unicastUpperConvergence;

		/**
		 * @brief Service Access Point for Broadcast Transmission
		 */
		convergence::BroadcastUpper* broadcastUpperConvergence;

		/**
		 * @brief Convergence to PHY
		 */
		convergence::Lower* lowerConvergence;

		/**
		 * @brief MAC address of this DLL
		 */
		wns::service::dll::UnicastAddress sourceMACAddress;

        /**
          * @brief Station type
          */
        StationType stationType_;

		/**
		 * @brief Logging
		 */
		wns::logger::Logger logger;
	}; // Component

} // glue


#endif // NOT defined GLUE_COMPONENT_HPP


