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

#ifndef WNS_LDK_HARQ_SOFTCOMBINING_CONTAINER_HPP
#define WNS_LDK_HARQ_SOFTCOMBINING_CONTAINER_HPP

#include <WNS/Exception.hpp>
#include <WNS/container/Registry.hpp>

#include <list>
#include <vector>

namespace wns { namespace ldk { namespace harq { namespace softcombining {

     template <class T>
     class Container
     {
     public:

         typedef std::list<T> EntryList;

         typedef std::vector<EntryList> EntryListVector;

         typedef typename wns::container::Registry<int, EntryListVector> EntryContainer;

         class InvalidRV :
             public Exception
         {
         public:
             ~InvalidRV() throw() {}
         };

        class InvalidPositionInTB :
            public Exception
        {
        public:
            ~InvalidPositionInTB() throw() {};
        };

         Container()
         {
             numRVs_ = 0;
         }

         Container(int numRVs)
         {
             if (numRVs < 0)
             {
                throw typename Container::InvalidRV();
             }
             numRVs_ = numRVs;
         }

         void
         clear()
         {
             receivedEntries_.clear();
         }


         int
         getNumRVs() const
         {
             return numRVs_;
         }

         std::list<int>
         getAvailablePosInTB() const
         {
             std::list<int> r;

             typename EntryContainer::const_iterator it;

             for(it = receivedEntries_.begin(); it!=receivedEntries_.end(); ++it)
             {
                 r.push_back(it->first);
             }

             return r;
         }

         EntryList
         getEntriesForRV(int posInTB, int rv) const
         {
             if (!receivedEntries_.knows(posInTB))
             {
                 throw typename Container::InvalidPositionInTB();
             }

             checkIfValidRV(rv);

             return receivedEntries_.find(posInTB)[rv];
         }

         void
         appendEntryForRV(int posInTB, int rv, T compound)
         {
             checkIfValidRV(rv);

             if (!receivedEntries_.knows(posInTB))
             {
                 receivedEntries_.insert(posInTB, EntryListVector(numRVs_));
             }
             receivedEntries_.find(posInTB)[rv].push_back(compound);
         }

     private:

         void
         checkIfValidRV(int rv) const
         {
             if (rv < 0 || rv >= getNumRVs())
             {
                 throw typename Container::InvalidRV();
             }
         }

         EntryContainer receivedEntries_;

         int numRVs_;
     };

} // softcombining
} // harq
} // ldk
} // wns

#endif // WNS_LDK_HARQ_SOFTCOMBINING_CONTAINER_HPP
