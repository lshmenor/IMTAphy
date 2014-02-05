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

#ifndef WNS_LDK_CONCATENATION_HPP
#define WNS_LDK_CONCATENATION_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Delayed.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <list>

namespace wns { namespace ldk { namespace concatenation {

    class ConcatenationCommand :
        public Command
    {
    public:
        struct
        {
        }
        local;
        struct
        {
            /// Concatenated compounds
            std::vector<CompoundPtr> compounds;
        }
        peer;
        struct
        {
        }
        magic;

        ConcatenationCommand()
        {
            peer.compounds = std::vector<CompoundPtr>();
        }
        ~ConcatenationCommand()
        {
            peer.compounds.clear();
        }
    };

    /**
     * @brief Concatenation of multiple compounds
     *
     */
    class Concatenation :
        public fu::Plain<Concatenation, ConcatenationCommand>,
        public Delayed<Concatenation>
    {
    public:
        Concatenation(fun::FUN* fuNet, const wns::pyconfig::View& config);
        ~Concatenation();

        /// Processor interface
        virtual void processIncoming(const CompoundPtr& compound);
        virtual bool hasCapacity() const;
        virtual void processOutgoing(const CompoundPtr& compound);
        virtual const CompoundPtr hasSomethingToSend() const;
        virtual CompoundPtr getSomethingToSend();

        /// SDU and PCI size calculation
        virtual void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

    protected:
        /// create new container with first compound entry
        CompoundPtr createContainer(const CompoundPtr& firstEntry);

        /// calculate the required padding towards a given boundary
        Bit calculatePadding(Bit pduSize, Bit paddingBoundary) const;

        /// maximum size of the container compound
        const Bit maxSize;

        /// current SDU size of the container compound in the buffer
        Bit currentSDUSize;

        /// current PCI size of the container compound in the buffer
        Bit currentCommandPoolSize;

        /// the container compound
        CompoundPtr currentCompound;
        // If the last compound is accepted while not sufficent bufferspace is available, then it will be store in full
        CompoundPtr nextCompound;

        /// maximum number of entries per container compound
        int maxEntries;
        /// current number of entries in the container compound
        int currentEntries;

        /// Size of the PCI of the container compound with > 1 entries
        const Bit numBitsIfConcatenated;
        /// Number of PCI bits per entry in the container compound
        const Bit numBitsPerEntry;
        /// Size of the PCI of the container with 1 entry
        const Bit numBitsIfNotConcatenated;

        /// count the size of each PCI of each entry
        const bool countPCISizeOfEntries;
        /// pad entries towards a multiple of a given bit boundary
        const Bit entryPaddingBoundary;

        logger::Logger logger;
    };
}
}
}

#endif // NOT defined WNS_LDK_CONCATENATION_HPP


