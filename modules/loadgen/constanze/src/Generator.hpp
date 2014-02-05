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

#ifndef CONSTANZE_GENERATOR_HPP
#define CONSTANZE_GENERATOR_HPP

#include <WNS/events/CanTimeout.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/distribution/Distribution.hpp>
#include <WNS/osi/PDU.hpp>

namespace constanze
{
	class Binding;
	class StartTrigger;
	class StopTrigger;

	/**
	 * @brief Packet Generator Base Class
	 * takes care of bindings, start and stop trigger, logger and pyco
	 * the derived classes can focus on traffic generation
	 */
	class GeneratorBase
	{
	public:
		typedef wns::PyConfigViewCreator<GeneratorBase, GeneratorBase> Creator;
		typedef wns::StaticFactory<Creator> Factory;

		explicit GeneratorBase(const wns::pyconfig::View& _pyco);
		virtual	~GeneratorBase();

		/**
		 * @brief store binding in member and
		 * make startTrigger and stopTrigger with parameters from pyConfig
		 * the binding is made when the startTrigger event happens
		 */
		void registerBinding(constanze::Binding* _bind);

		/**
		 * @brief after binding is ready (at startTrigger time = offset),
		 * set timeout for the first packet
		 * and delete the startTrigger
		 */
		void bindingReady();

		/**
		 * @brief when the stopTrigger event happens, the next timeout event is cancelled
		 * and the stopTrigger is deleted
		 */
		void bindingReleased();

		/**
		 * @brief called when process should start generating events
		 */
		virtual void start() = 0; // to be overloaded by derived classes
		/**
		 * @brief called when process should stop generating events
		 */
		virtual void stop() = 0; // to be overloaded by derived classes

		/**
		 * @brief send PDU out downstack
		 */
		virtual void sendData(const wns::osi::PDUPtr& pdu);

		/**
		 * @brief called by SubGenerators or derived classes
		 */
		virtual void countPackets(const int packetSizeBits);

		virtual unsigned long int countedPackets() const {return packetCounter;}
		virtual unsigned long int countedBits() const {return bitCounter;}

	protected:

		/**
		 * @brief The Python configuration
		 */
		wns::pyconfig::View pyco;

		/**
		 * @brief Logger instance.
		 */
		wns::logger::Logger log;

		/**
		 * @brief Binding for plugging the Generator on top of any layer,
		 * eg., UDP or IP.
		 */
		constanze::Binding* binding;

	private:
		/**
		 * @brief Event-triggered handler for the START time
		 */
		constanze::StartTrigger* startTrigger;
		/**
		 * @brief Event-triggered handler for the STOP time
		 */
		constanze::StopTrigger* stopTrigger;

		/** @brief count packets */
		unsigned long int packetCounter;
		/** @brief count bits */
		unsigned long int bitCounter;
	};


	/**
	 * @brief Event for the start of traffic generation
	 */
	class StartTrigger:
		protected wns::events::CanTimeout
	{
	public:
		StartTrigger(
			constanze::Binding* _binding,
			constanze::GeneratorBase* _generator,
			double _startTime);

		virtual	~StartTrigger()
		{}

		void bindingReady();

	protected:
		/**
		 *  @brief called at startTime ("offset")
		 */
		virtual void onTimeout();

		constanze::Binding* binding;

		constanze::GeneratorBase* generator;

	}; // StartTrigger

	/**
	 * @brief Event for the end of traffic generation
	 */
	class StopTrigger:
		protected wns::events::CanTimeout
	{
	public:
		StopTrigger(
			constanze::Binding* _binding,
			constanze::GeneratorBase* _generator,
			double _stopTime);

		virtual	~StopTrigger()
		{}

		void bindingReady();

	protected:
		/**
		 * @brief called at stopTime
		 */
		virtual void onTimeout();

		constanze::Binding* binding;

		constanze::GeneratorBase* generator;
	}; // StopTrigger
} // constanze

#endif // NOT defined CONSTANZE_GENERATOR_HPP

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
