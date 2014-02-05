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

#ifndef WNS_SERVICE_PHY_PHYMODE_PHYMODEINTERFACE_HPP
#define WNS_SERVICE_PHY_PHYMODE_PHYMODEINTERFACE_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/IOutputStreamable.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/RefCountable.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/simulator/Time.hpp>

#include <string>

namespace wns { namespace service { namespace phy { namespace phymode {

        typedef unsigned int Modulation;
        typedef unsigned int Coding;

        /**
		@brief Base Class for encapsulating Modulation and Coding -> PHY Mode
		wns::service::phy::phymode::PhyModeInterface
	*/
        class PhyModeInterface :
            public virtual wns::IOutputStreamable,
            public virtual wns::RefCountable,
            public virtual wns::CloneableInterface
        {
        public:
            virtual ~PhyModeInterface() {}

            /** @brief true if PhyMode is set (not undefined) */
            virtual bool isValid() const = 0;

            /** @brief true if dataRate can be calculated. False if something is missing. */
            virtual bool dataRateIsValid() const = 0;

            /** @brief test PhyModes for equality */
            friend bool operator== ( const PhyModeInterface& p1, const PhyModeInterface& p2 )
            { return (p1.toInt() == p2.toInt()); }
            friend bool operator!= ( const PhyModeInterface& p1, const PhyModeInterface& p2 )
            { return !(p1==p2); }

            /** @brief operator< used for sorting/ordering in maps,registries.
                This does not mean "better" PhyMode in the sense of bitrate or robustness.
                For that purpose, use PhyModeIsBetter(a,b)
            */
            friend bool operator< ( const PhyModeInterface& p1, const PhyModeInterface& p2 )
            { return (p1.toInt() < p2.toInt()); }
            //{ return (this->getBitsPerSymbol() < right.getBitsPerSymbol()); } // other semantic
            friend bool operator> ( const PhyModeInterface& p1, const PhyModeInterface& p2 )
            { return !(p1<p2); }

            /** @brief Compares PhyModes in terms of bits per modulation symbol */
            friend bool PhyModeIsBetter( const PhyModeInterface& p1, const PhyModeInterface& p2 )
            { return (p1.getBitsPerSymbol()>p2.getBitsPerSymbol()); }

            /** @brief calculate capacity[bits] for an arbitrary duration */
            virtual unsigned int
            getBitCapacityFractional(wns::simulator::Time duration) const = 0;

            /** @brief datarate in [bits per second] */
            /** symbolRate and subCarriersPerSubChannel must be known in advance */
            virtual double getDataRate() const = 0;

            /** @brief MI = f(SINR) (mutual information) */
            virtual double getSINR2MI(const wns::Ratio& sinr) const = 0;
            /** @brief MIB = f(SINR) (mutual information per bit, [0..1]) */
            virtual double getSINR2MIB(const wns::Ratio& sinr) const = 0;
            /** @brief PER = f(MIB,bits), blockLength bits (=payload[net], not CWL[gross])*/
            virtual double getMI2PER(const double mib, unsigned int bits) const = 0;
            /** @brief PER = f(SINR,bits), blockLength bits (=payload[net], not CWL[gross]) */
            virtual double getSINR2PER(const wns::Ratio& sinr, unsigned int bits) const = 0;
            /** @brief SINR = f(MIB) */
            virtual wns::Ratio getMIB2SINR(const double& mib) const = 0;

            /** @brief access method for the modulation&coding (string) */
            virtual std::string getString() const = 0;

            /** @brief true, if string matches to the PhyMode */
            virtual bool nameMatches(const std::string& name) const = 0;

            /** @brief access method for the modulation */
            virtual Modulation getModulation() const = 0;

            /** @brief access method for the coding */
            virtual Coding getCoding() const = 0;

            /** @brief bit rate per OFDM symbol */
            virtual double getBitsPerSymbol() const = 0;

        private:
            /** @brief Convert PHY Mode (M&C) to one int (used for ordering) */
            virtual unsigned int toInt() const = 0;
        };

        /** @brief when a pointer to a PhyMode is needed, use this one (SmartPtr)
            which takes care of memory deallocation itself.
	 */
        typedef wns::SmartPtr<const wns::service::phy::phymode::PhyModeInterface> PhyModeInterfacePtr;
        /** @brief a const reference to the PhyMode can be used anywhere.
		The	PhyMode objects are typically kept (in a list) in the PhyModeMapper,
		which is responsible for the proper memory handling.
	 */
        typedef const wns::service::phy::phymode::PhyModeInterface& PhyModeInterfaceConstRef;

        typedef wns::PyConfigViewCreator<PhyModeInterface, PhyModeInterface> PhyModeCreator;
        typedef wns::StaticFactory<PhyModeCreator> PhyModeFactory;

        /** @brief only for use in PhyModeMapper which needs to modify PhyMode */
        inline PhyModeInterface*
        createPhyModeNonConst(const wns::pyconfig::View& config)
        {
            std::string nameInPhyModeFactory = config.get<std::string>("nameInPhyModeFactory");
            return PhyModeFactory::creator(nameInPhyModeFactory)->create(config);
        }

        /** @brief use this to create a PhyModeInterfacePtr (SmartPtr) for a given Python specification */
        inline PhyModeInterfacePtr
        createPhyMode(const wns::pyconfig::View& config)
        {
            std::string nameInPhyModeFactory = config.get<std::string>("nameInPhyModeFactory");
            return PhyModeInterfacePtr(PhyModeFactory::creator(nameInPhyModeFactory)->create(config));
        }

        /** @brief Helper method to yield a reference to an empty (Undefined) PhyMode Object */
        inline const PhyModeInterface&
        emptyPhyMode()
        {
            typedef wns::Creator<PhyModeInterface> DefaultPhyModeCreator;
            typedef wns::StaticFactory<DefaultPhyModeCreator> DefaultPhyModeFactory;
            // done only once for the whole simulation:
            static const PhyModeInterface* empty = DefaultPhyModeFactory::creator("rise.PhyMode.Empty")->create();
            return *empty;
        }

        /** @brief Helper method to yield a PhyModeInterfacePtr (SmartPtr) to an empty (Undefined) PhyMode Object */
        inline PhyModeInterfacePtr
        emptyPhyModePtr()
        {
            typedef wns::Creator<PhyModeInterface> DefaultPhyModeCreator;
            typedef wns::StaticFactory<DefaultPhyModeCreator> DefaultPhyModeFactory;
            // done only once for the whole simulation:
            static PhyModeInterface* empty = DefaultPhyModeFactory::creator("rise.PhyMode.Empty")->create();
            static PhyModeInterfacePtr emptyPtr = PhyModeInterfacePtr(empty);
            return emptyPtr;
        }

} // phymode
} // phy
} // service
} // wns

namespace wns {
  // for convenience: wns::PhyModePtr
  typedef wns::service::phy::phymode::PhyModeInterfacePtr PhyModePtr;
}

#endif // WNS_SERVICE_PHY_PHYMODE_PHYMODEINTERFACE_HPP
