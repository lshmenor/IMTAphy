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

#ifndef CONSTANZE_LISTENER_HPP
#define CONSTANZE_LISTENER_HPP

#include <CONSTANZE/ConstanzePDU.hpp>
#include <CONSTANZE/Binding.hpp>
#include <CONSTANZE/Measurement.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

namespace constanze
{
	/** @brief ContextProvider to provide an arbitrary string EXACTLY ONE TIME */
	class OneLiner :
		public wns::probe::bus::ContextProvider
	{
		std::string key_;
		std::string value_;
		mutable bool newValue_;
	public:
		OneLiner(const std::string& key) : key_(key), value_(), newValue_(false) {}

		virtual void
		set(const std::string& value)
			{
				newValue_ = true;
				value_ = value;
			}

		const std::string&
		getKey() const
			{
				return key_;
			}
    private:
		virtual void
		doVisit(wns::probe::bus::IContext& c) const
		{
			if (newValue_ == true)
				{
					c.insertString(key_, value_);
					newValue_ = false;
				}
		}
	};


	/** @brief this class receives traffic from the peer */
	class Listener
	  // : public Probe
	{
	public:
		Listener( const wns::pyconfig::View& _pyco, wns::node::component::Interface* component);

		~Listener();

		/** @brief handle incoming traffic from the peer */
		void
		onData(const wns::osi::PDUPtr& _data);

		void
		registerBinding(constanze::Binding* _binding);

		virtual std::string printAddress() const;

		friend std::ostream&
		operator <<(std::ostream& os, const Listener& l)
		{
			os << l.printAddress();
			return os;
		}

	private:
		/** @brief Python view */
		wns::pyconfig::View pyco;

		/** @brief Logger */
		wns::logger::Logger log;

		/** @brief domainName */
		std::string domainName;

		/** @brief binding to a socket with ip address */
		constanze::Binding* binding;

		/**
		 * @brief class for doing measurements based on rate(time)
		 */
		Measurement* measurement;

		/**
		 * @brief control step identification
		 */
		int stateLevel;

		/**
		 * @brief windowed probes calculate rate values averaged over this time
		 */
		double windowSize;

		/**
                 * @brief if true, do estimation of MMPP states. Toggle on in Node.py
                 */
		bool doMMPPestimation;

		/** @brief vector of observations [wez] */
		/** @todo put into Measurement.hpp [rs] */
		std::vector<int> *observationVector;

		/** @brief Probe Front-ends */
		wns::probe::bus::ContextCollectorPtr sizeProbe;
		wns::probe::bus::ContextCollectorPtr bitsRateProbe;
		wns::probe::bus::ContextCollectorPtr packetRateProbe;
		wns::probe::bus::ContextCollectorPtr bitsWindowedRateProbe;
		wns::probe::bus::ContextCollectorPtr packetWindowedRateProbe;
		wns::probe::bus::ContextCollectorPtr delayProbe;

		/** @brief helpers for text putting into probes */
		OneLiner bitsWindowedResult;
		OneLiner packetWindowedResult;

		/** @brief count packets */
		long unsigned int packetCounter;
		/** @brief count bits */
		long unsigned int bitCounter;
		/** @brief count packets in current window */
		long unsigned int windowedPacketCounter;
		/** @brief count bits in current window */
		long unsigned int windowedBitCounter;
		/** @brief time at which the averaging window ends */
		simTimeType currentWindowEnd;
	};
}

#endif // NOT defined CONSTANZE_LISTENER_HPP

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
