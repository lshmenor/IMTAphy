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

#ifndef WNS_LDK_ARQ_HPP
#define WNS_LDK_ARQ_HPP

#include <WNS/ldk/arq/statuscollector/Interface.hpp>

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/FUNConfigCreator.hpp>
#include <WNS/ldk/Delayed.hpp>

#include <WNS/Birthmark.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>

namespace wns { namespace ldk { namespace arq {

    /**
     * @brief Interface for Compounds specified by ARQ implementations.
     *
     * ARQ implementations /should/ derive their Command from this class to
     * support deffered coupling to ARQ.
     */
    class ARQCommand :
        public Command
    {
    public:
        ARQCommand() :
            localTransmissionCounter(0)
        {
        }

        /**
         * @brief SequenceNumber Type
         *
         * @note This sequence number does not wrap around, as normal
         * sequence numbers in a real ARQ implementation would do.
         */
        typedef long long int SequenceNumber;

        virtual bool
        isACK() const = 0;

        size_t localTransmissionCounter;
    };

    /**
     * @brief Base class for ARQ implementations.
     *
     * ARQ supports the implementer of ARQ algorithms in multiple ways:
     * <ol>
     * <li> it is a class decorator, implementing hasSomethingToSend() and
     *    getSomethingToSend() required by DelayedInterface.
     *
     *    Their implementation respects the additional interface of changing
     *    the preference of acknowledgement Compounds over user data Compounds.
     *    This additional interface can be used via direct coupling by other
     *    FUs.  PiggyBacker for example is based on this interface.
     *
     *    The default is to prefer acknowledgement compounds.
     *
     *    The external interface for changing the preference consists of the
     *    methods preferACK(bool) and preferACK(). The inheritance interface
     *    consists of the methods hasACK(), hasData(), getACK(), and getData().
     * </li>
     * <li> it provides a facility to probe retransmission counters.  the
     *    interface for retransmission accounting consists of
     *    countTransmission(const CompoundPtr&), transmissions(const CompoundPtr&) and
     *    eraseFromCounter(const CompoundPtr&).
     * </li>
     * </ol>
     */
    class ARQ :
        virtual public FunctionalUnit,
        virtual public DelayedInterface
    {
    public:
        ARQ(const wns::pyconfig::View& config) :
            isPreferingACK(true)
        {
            pyconfig::View statusCollectorConfig(config, "arqStatusCollector");
            std::string statusCollectorName = statusCollectorConfig.get<std::string>("__plugin__");
            this->statusCollector = statuscollector::StatusCollectorFactory::creator(statusCollectorName)->create(statusCollectorConfig);
        }

        virtual
        ~ARQ()
        {
            delete this->statusCollector;
        }

        /**
         * @brief Set whether acknowledgement Compounds are preferred
         * over user data Compounds.
         */
        void
        preferACK(bool _isPreferingACK)
        {
            isPreferingACK = _isPreferingACK;
        }

        /**
         * @brief Return whether acknowledgement Compounds are preferred
         * over user data Compounds.
         */
        bool
        preferACK() const
        {
            return isPreferingACK;
        }

        //
        // partial Delayed interface implementation
        //

        virtual const CompoundPtr
        hasSomethingToSend() const
        {
            CompoundPtr it;
            if(preferACK())
            {
                it = hasACK();
                if(!it)
                {
                    it = hasData();
                }
            }
            else
            {
                it = hasData();
                if(!it)
                {
                    it = hasACK();
                }
            }
            return it;
        } // hasSomethingToSend()

        virtual CompoundPtr
        getSomethingToSend()
        {
            assure(hasSomethingToSend(),
                   "hasSomethingToSend has not been called to check wheter there is something to send.");

            if(preferACK())
            {
                if(hasACK())
                {
                    return getACK();
                }
                return getData();
            }
            else
            {
                if(hasData())
                {
                    return getData();
                }
                return getACK();
            }
        } // getSomethingToSend

        /**
         * @brief Getter for the current success rate of the
         * transmissions, as measured by the arqStatusCollector
         */
        double
        getSuccessRate(const CompoundPtr& compound)
        {
            return(this->statusCollector->getSuccessRate(compound));
        }

        /**
         * @brief Reset the current statistics of the arqStatusCollector
         */
        void
        resetStatusCollector()
        {
            this->statusCollector->reset();
        }

    protected:
        /**
         * @brief Return an acknowledgement Compound that should be
         * considered for transmission.
         *
         * Every implementation should return CompoundPtr() in case
         * there is no user data Compound scheduled for transmission.
         */
        virtual const CompoundPtr
        hasACK() const = 0;

        /**
         * @brief Return a user data Compound that should be considered
         * for transmission.
         *
         * Every implementation should return CompoundPtr() in case
         * there is no user data Compound scheduled for transmission.
         */
        virtual const CompoundPtr
        hasData() const = 0;

        /**
         * @brief Return an acknowledgement Compound for transmission.
         *
         * getACK will only get called after a call to hasACK. Every
         * implementation of this interface /must/ return the same
         * Compound for subsequent hasACK/getACK calls.
         */
        virtual CompoundPtr
        getACK() = 0;

        /**
         * @brief Return a user data Compound for transmission.
         *
         * getData will only get called after a call to hasData. Every
         * implementation of this interface /must/ return the same
         * Compound for subsequent hasData/getData calls.
         */
        virtual CompoundPtr
        getData() = 0;

        statuscollector::Interface* statusCollector;

//      /**
//       * @brief Returns the distance between two sequence numbres
//       *
//       * @intern This method enables different implementations of
//       * sequence numbers. For the implementation we're currently
//       * using (long long int) the method is very easy. For real sequence
//       * numbers, that warp around, it's a little bit harder (see
//       * commented code below).
//       */
//      static int
//      distance(
//          const ARQCommand::SequenceNumber& a,
//          const ARQCommand::SequenceNumber& b)
//      {
//          assure(a>=0, "Negative Sequence Number");
//          assure(b>=0, "Negative Sequence Number");

//          return a - b;

//          // assure(windowSize>=2, "Bad windowSize");
//          // assure(sequenceNumberSize>= 2*windowSize, "Too small modDivisor");
//          // return (a - b + sequenceNumberSize) % sequenceNumberSize;
//      }

    private:
        bool isPreferingACK;
    };

    typedef FUNConfigCreator<ARQ> ARQCreator;
    typedef wns::StaticFactory<ARQCreator> ARQFactory;


}
}
}

#endif // NOT defined WNS_LDK_ARQ_HPP



