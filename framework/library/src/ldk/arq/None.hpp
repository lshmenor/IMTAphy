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

#ifndef WNS_LDK_ARQ_NONE_HPP
#define WNS_LDK_ARQ_NONE_HPP

#include <WNS/ldk/arq/ARQ.hpp>
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Forwarding.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>

#include <WNS/pyconfig/View.hpp>

namespace wns { namespace ldk { namespace arq {

    class NoneCommand :
        public ARQCommand
    {
    public:
        virtual bool
        isACK() const
        {
            return false;
        }

        struct
        {
        }
        local;

        struct
        {
        }
        peer;

        struct
        {
        }
        magic;
    };

    /*
     * @brief Forwarding only FunctionalUnit conforming to the ARQ interface
     */
    class None :
        public ARQ,
        public wns::ldk::fu::Plain<None, NoneCommand>,
        public Delayed<None>,
        virtual public SuspendableInterface,
        public SuspendSupport
    {
    public:
        None(fun::FUN* fuNet, const wns::pyconfig::View& config) :
                ARQ(config),
                wns::ldk::fu::Plain<None, NoneCommand>(fuNet),
                Delayed<None>(),
                SuspendSupport(fuNet, config),

                compound()
        {
        }


        // Delayed interface realisation
        virtual bool hasCapacity() const
        {
            return !compound;
        } // hasCapacity

        virtual void
        processOutgoing(const CompoundPtr& _compound)
        {
            assure(hasCapacity(), "yeye, something went wrong.");

            compound = _compound;
            activateCommand(compound->getCommandPool());
        } // processOutgoing

        // virtual const CompoundPtr hasSomethingToSend() const; // implemented by ARQ
        // virtual CompoundPtr getSomethingToSend(); // implemented by ARQ

        virtual void
        processIncoming(const CompoundPtr& compound)
        {
            getDeliverer()->getAcceptor(compound)->onData(compound);
        } // processIncoming


        // ARQ interface realization
        virtual const wns::ldk::CompoundPtr hasACK() const
        {
            return CompoundPtr();
        } // hasACK

        virtual const wns::ldk::CompoundPtr hasData() const
        {
            return compound;
        } // hasData

        virtual wns::ldk::CompoundPtr getACK()
        {
            return CompoundPtr();
        } // getACK;

        virtual wns::ldk::CompoundPtr getData()
        {
            CompoundPtr it = compound;
            compound = CompoundPtr();
            return it;
        } // getData

    private:
        virtual bool onSuspend() const
        {
            return !compound;
        } // onSuspend

        CompoundPtr compound;
    };

}
}
}

#endif // NOT defined WNS_LDK_ARQ_NONE_HPP



