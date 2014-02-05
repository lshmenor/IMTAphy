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

#ifndef WNS_LDK_SEQUENTLYCALLINGLINKHANDLER_HPP
#define WNS_LDK_SEQUENTLYCALLINGLINKHANDLER_HPP

#include <WNS/ldk/LinkHandlerInterface.hpp>
#include <WNS/ldk/Compound.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <list>

namespace wns { namespace ldk {

	class FunctionalUnit;

	class SequentlyCallingLinkHandler :
		virtual public LinkHandlerInterface
	{
		struct FUCompound
		{
			FUCompound() :
				fu(NULL),
				compound(wns::ldk::CompoundPtr())
			{} // FUCompound

			FUCompound(FunctionalUnit* _fu, const CompoundPtr& _compound) :
				fu(_fu),
				compound(_compound)
			{} // FUCompound

			FunctionalUnit* fu;
			CompoundPtr compound;
		};

		typedef std::list<FUCompound> FUCompoundContainer;
		typedef std::list<FunctionalUnit*> FUContainer;

	public:
		SequentlyCallingLinkHandler(const wns::pyconfig::View& _config);

		virtual bool
		isAcceptingForwarded(FunctionalUnit* fu, const CompoundPtr& compound);

		virtual void
		sendDataForwarded(FunctionalUnit* fu, const CompoundPtr& compound);

		virtual void
		wakeupForwarded(FunctionalUnit* fu);

		virtual void
		onDataForwarded(FunctionalUnit* fu, const CompoundPtr& compound);

	private:
		void
		sendDataHandler(FunctionalUnit* fu, const CompoundPtr& compound);

		void
		wakeupHandler(FunctionalUnit* fu);

		void
		onDataHandler(FunctionalUnit* fu, const CompoundPtr& compound);

		void
		mainHandler();

		bool inAction;

		FUContainer pendingCompoundsContainingFUs;

		int sendDataPending;
		FUCompound sendDataFUCompound;

		FUContainer wakeupFUs;
		bool inWakeup;
		FUContainer wakeupFUsInWakeup;

		FUCompoundContainer onDataFUCompounds;

		bool traceCompoundJourney;

		wns::logger::Logger isAcceptingLogger;
		wns::logger::Logger sendDataLogger;
		wns::logger::Logger wakeupLogger;
		wns::logger::Logger onDataLogger;
	};

} // ldk
} // wns

#endif // NOT defined WNS_LDK_SEQUENTLYCALLINGLINKHANDLER_HPP


