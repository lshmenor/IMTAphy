/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#ifndef WNS_LDK_SOFTSAR_HPP
#define WNS_LDK_SOFTSAR_HPP

#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/ldk/Compound.hpp>

#include <WNS/ldk/sar/SAR.hpp>
#include <WNS/ldk/ErrorRateProviderInterface.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>

#include <WNS/pyconfig/View.hpp>

#include <list>

namespace wns { namespace ldk { namespace sar
{
	/**
	 * @brief to fulfill the "soft combining functionality, this class' Command
	 * is derived from PERProviderInterface
	 */

	class SoftCommand :
		public SARCommand,
		virtual public wns::ldk::ErrorRateProviderInterface
	{
		double PER;
		std::string info;
	public:
		SoftCommand();
		virtual double getErrorRate() const;
		void setPER(const double _PER);

		virtual std::string getInfo() const
		{ return info; }

		virtual void setInfo(const std::string& _info)
		{ info = _info; }
	};

	/**
	 * @brief Segmentation and Reassembly with a fixed size MTU.
	 * if segments with "soft information" are reassembled, this SAR FU can provide a
	 * PER in the local part of the Command. To obtain the PER of the segments
	 * it is reassemnling, this unit has to rely on the help of a friend,
	 * e.g. the MACr, which also has to derive its Command from PERProviderInterface
	 */
	class Soft :
		public wns::ldk::sar::SAR<SoftCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::ldk::Delayed<Soft>,
		public wns::Cloneable<Soft>
	{
	public:
		Soft(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config);
		~Soft();

		//
		// Completion of Processor interface
		//
		virtual void processIncoming(const wns::ldk::CompoundPtr& compound);

		// To obtain a pointer to our PER Provider
		virtual void onFUNCreated();

	private:
		typedef std::pair<wns::ldk::CompoundPtr, double> PERStorageEntry;
		typedef std::list<PERStorageEntry> PERStorageContainer;

		PERStorageContainer incoming;
		int fragmentNumber;

		std::string  PERProviderName;

		struct Friends {
			wns::ldk::FunctionalUnit* PERProvider;
		} friends;

		bool useProbe;
		std::string probeName;
		wns::probe::bus::ContextCollectorPtr perProbeBus;
	};
}}}

#endif // NOT defined WNS_LDK_SOFTSAR_HPP


