/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef CONSTANZE_COMPONENT_HPP
#define CONSTANZE_COMPONENT_HPP

#include <CONSTANZE/Generator.hpp>

#include <WNS/node/component/Component.hpp>
#include <WNS/logger/Logger.hpp>

#include <list>

namespace constanze {

	class GeneratorBase;
	class Binding;
	class Listener;

	/**
	 * @brief Constanze Component (layer above IP,UDP,TCP,SimpleTL,Glue)
	 */
	class ConstanzeComponent :
		virtual public wns::node::component::Component
	{
	public:
		/**
		 * @brief ConstanzeComponent constructor
		 *
		 * @param[in] _node The parent node.
		 *
		 * @param[in] _pyco Component configuration of type
		 * constanze.ConstanzeComponent
		 */
		ConstanzeComponent(
			wns::node::Interface* _node,
			const wns::pyconfig::View& _pyco);

		/**
		 * @brief ConstanzeComponent destructor
		 */
		virtual ~ConstanzeComponent();

		/**
		 * @brief Connect to required components within the same node.
		 */
		virtual void
		onNodeCreated();

		/**
		 * @brief Connect to components in other nodes
		 */
		virtual void
		onWorldCreated();

		/**
		 * @brief Clean up before EventScheduler really stops
		 */
		virtual void
		onShutdown();

	private:
		/**
		 * @brief Services are added here
		 */
		virtual void
		doStartup();

		/**
		 * @brief All generators created are put to this list.
		 */
		std::list<constanze::GeneratorBase*> listOfGenerators;

		/**
		 * @brief Bindings created for the generators.
		 */
		std::list<constanze::Binding*> listOfGeneratorBindings;

		/**
		 * @brief All listeners created are put to this list.
		 */
		std::list<constanze::Listener*> listOfListeners;

		/**
		 * @brief Bindings created for the listeners.
		 */
		std::list<constanze::Binding*> listOfListenerBindings;

		/**
		 * @brief The components configuration.
		 */
		wns::pyconfig::View pyco;

		/**
		 * @brief The Logger instance.
		 */
		wns::logger::Logger log;
	};
}

#endif // _CONSTANZE_COMPONENT_HPP

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
