/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef WNS_LDK_TOOLS_INSEQUENCECHECKER_HPP
#define WNS_LDK_TOOLS_INSEQUENCECHECKER_HPP

#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace wns { namespace ldk { namespace tools {

    class InSequenceCheckerCommand :
        public wns::ldk::Command
    {
    public:
        InSequenceCheckerCommand()
        {
            magic.sequenceID = 0;
        }

        struct {} local;
        struct {} peer;
        struct {
            unsigned int sequenceID;
        } magic;
    }; // InSequenceChecker

    /**
     * @brief The InSequenceChecker FU assures that all compounds of a
     * flow are delivered in the order they are sent.
     * @author Klaus Sambale <ksw@comnets.rwth-aachen.de>
     */
    class InSequenceChecker :
        public wns::ldk::Processor<InSequenceChecker>,
        public wns::ldk::CommandTypeSpecifier<InSequenceCheckerCommand>,
        public wns::ldk::HasReceptor<>,
        public wns::ldk::HasConnector<>,
        public wns::ldk::HasDeliverer<>,
        public wns::Cloneable<InSequenceChecker>
    {
    public:
        /**
         * @brief Constructor
         */
        explicit
        InSequenceChecker(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

        /**
         * @brief Destructor
         */
        virtual
        ~InSequenceChecker();

    private:
        /**
         * @name FunctionalUnit interface
         */
        //@{
        virtual void
        processOutgoing(const wns::ldk::CompoundPtr&);

        virtual void
        processIncoming(const wns::ldk::CompoundPtr&);
        //@}

        unsigned int sequenceIDOutgoing;
        unsigned int sequenceIDExpected;

        wns::logger::Logger logger;
    };

} // tools
} // ldk
} // wns
#endif // NOT defined WNS_LDK_TOOLS_INSEQUENCECHECKER_HPP


