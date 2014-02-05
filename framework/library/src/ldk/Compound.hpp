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

#ifndef WNS_LDK_COMPOUND_HPP
#define WNS_LDK_COMPOUND_HPP

#include <WNS/ReferenceModifier.hpp>
#include <WNS/ldk/CommandPool.hpp>
//#include <WNS/ldk/CommandProxy.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/Birthmark.hpp>
#include <WNS/Assure.hpp>
#include <WNS/TypeInfo.hpp>

/*
 * for compound journeys (JOURNEY)
 * Journey records the path of a compound through FUs.
 * this is expensive and should be made optional.
 * to find all changes due to this journey path, search for
 * the tag JOURNEY (within libwns and glue.)
 */
#include <list>
#include <cstdio>

namespace wns { namespace ldk {

	class FunctionalUnit;

	struct Visit
	{
		Visit(simTimeType _t, const std::string& _location) :
				t(_t),
				location(_location)
		{}

		simTimeType t;
		std::string location;

		bool
		operator==(const Visit& other) const
		{
			return (t == other.t) && (location == other.location);
		}
	};

	/**
	 * @brief Basic transmission unit within a fun::FUN.
	 */
	class Compound :
		public virtual HasBirthmark,
		public wns::osi::PDU
	{
	public:
		typedef std::list<Visit> JourneyContainer;

		Compound(CommandPool* commandPool = NULL, const wns::osi::PDUPtr& sdu = wns::osi::PDUPtr()) :
			wns::osi::PDU(commandPool, sdu.getPtr()),
			fu(NULL)
		{}

		CommandPool*
		getCommandPool() const
		{
			assure(PDU::getPCI(), "PCI is NULL.");
			assure(dynamic_cast<CommandPool*>(PDU::getPCI()), "PCI is not of the desired type.");
			return static_cast<CommandPool*>(PDU::getPCI());
		} // getCommandPool

		osi::PDUPtr
		getData() const
		{
			if(getUserData()) {
				return osi::PDUPtr(getUserData());
			} else {
				return osi::PDUPtr();
			}
		} // getData

		SmartPtr<Compound>
		copy()
		{
			return SmartPtr<Compound>(clone());
		} // copy

		void
		visit(const FunctionalUnit* fu) const; // JOURNEY

		std::string
		dumpJourney() const		// JOURNEY
		{
			std::stringstream ss;

			for(JourneyContainer::iterator it = journey.begin();
				it != journey.end();
				++it) {
				ss << "\n(";
				ss.width(11);
				ss.setf(std::ios_base::fixed, std::ios_base::floatfield);
				ss.precision(7);
				ss << std::right << it->t << ") " << it->location.c_str();
			}

			return ss.str();

		} // dumpJourney

		const JourneyContainer&
		getJourney() const
		{
			return journey;
		}

		void
		setCallingFU(FunctionalUnit* _fu)
		{
			fu = _fu;
		}

		FunctionalUnit*
		getCallingFU()
		{
			return fu;
		}

		virtual Compound*
		clone()
		{
			Compound* result = new Compound(new CommandPool(*this->getCommandPool()), getData());
			result->setBirthmark(this->getBirthmark());
			result->journey = journey;
			return result;
		}

#ifndef NDEBUG
		size_t
		calcObjSize() const
		{
			size_t sum = sizeof( *this );
			sum += getCommandPool()->calcObjSize();
			if (getUserData() != NULL)
				sum += getData()->calcObjSize();
			return sum;
		}
#endif

	private:
		virtual Bit
		doGetLengthInBits() const
		{
			Bit commandPoolSize;
			Bit dataSize;

			getCommandPool()->calculateSizes(commandPoolSize, dataSize);

			return commandPoolSize + dataSize;
		} // getLengthInBits


		mutable JourneyContainer journey; // JOURNEY
		FunctionalUnit* fu;
	};

	typedef SmartPtr<Compound> CompoundPtr;

#ifdef WNS_SMARTPTR_DEBUGGING
#define CompoundPtr(...) CompoundPtr(__FILE__, __LINE__, ##__VA_ARGS__)
#endif

}}


#endif // NOT defined WNS_LDK_COMPOUND_HPP



