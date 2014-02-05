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

#ifndef WNS_LDK_FLOWSEPARATOR_NOTFOUNDSTRATEGY_HPP
#define WNS_LDK_FLOWSEPARATOR_NOTFOUNDSTRATEGY_HPP


#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Key.hpp>
#include <memory>

namespace wns { namespace ldk { namespace flowseparator {

    class CreatorStrategy;
    class FlowInfoProvider;

    /**
     * @brief Interface for FlowSeparator strategies for dealing with unknown flows.
     */
    class NotFoundStrategy
    {
    public:
        virtual
        ~NotFoundStrategy();

        /**
         * @brief Return a new FU for the given key
         *
         */
        virtual FunctionalUnit*
        ifNotFound(const ConstKeyPtr& key) const = 0;

        /** @brief enable dependency resolution
         */
        virtual void
        onFUNCreated() = 0;

            virtual FunctionalUnit*
            createPrototype() const = 0;

    }; // class NotFoundStrategy

    /**
     * @brief Do not create missing instances on the fly. Complain instead.
     *
     * @image html Complain.png "Sequence Diagram: Complain if no FU for Compound found" width=10cm
     */
    class Complain :
        public virtual NotFoundStrategy
    {
    public:
        Complain();

        Complain(fun::FUN* fuNet, const pyconfig::View& config);

        FunctionalUnit*
        ifNotFound(const ConstKeyPtr& key) const;

        FunctionalUnit*
        createPrototype() const;

        void
        onFUNCreated();
    };

    /**
     * @brief Autocreate missing instances.
     *
     * @image html CreateOnFirstCompound.png "Sequence Diagram: Create new FU if no FU for Compound found" width=10cm
     */
    class CreateOnFirstCompound :
        public virtual NotFoundStrategy
    {
    public:
        CreateOnFirstCompound(fun::FUN* fuNet, const pyconfig::View& config);
        CreateOnFirstCompound(CreatorStrategy* _creator);

        virtual
        ~CreateOnFirstCompound();

        FunctionalUnit* ifNotFound(const ConstKeyPtr& key) const;

        FunctionalUnit*
        createPrototype() const;

        void onFUNCreated();

    private:
        /**
         * @brief strategy for new fu creation
         */
        std::auto_ptr<CreatorStrategy> creator;
    };

    /**
     * @brief Only create missing instances when the compound
     * (i.e. the key) belongs to a valid flow
     *
     * @image html CreateOnValidFlow.png "Sequence Diagram: Create new FU if no FU for Compound found, but ask FlowManager in advance" width=10cm
     */
    class CreateOnValidFlow :
        public virtual NotFoundStrategy
    {
    public:
        CreateOnValidFlow(fun::FUN* fuNet, const pyconfig::View& config);

        CreateOnValidFlow(CreatorStrategy* _creator, FlowInfoProvider* _flowInfo);

        virtual
        ~CreateOnValidFlow();

        FunctionalUnit*
        createPrototype() const;

        FunctionalUnit*
        ifNotFound(const ConstKeyPtr& key) const;

        void
        onFUNCreated();
    private:
        /**
         * @brief needed to access the control service
         */
        fun::FUN* fun;

        /**
         * @brief strategy for new fu creation
         */
        std::auto_ptr<CreatorStrategy> creator;

        /** @brief handle to the service that has the flow
         * info.
         *
         * The handle is obtained from the
         * ControlServiceRegistry of the FUN via the 'flowInfoProviderName'
         */
        FlowInfoProvider* flowInfo;

        /**
         * @brief name under which to access the control
         * service that has the flow info
         */
        std::string flowInfoProviderName;
    };

    typedef FUNConfigCreator<NotFoundStrategy> NotFoundStrategyCreator;
    typedef wns::StaticFactory<NotFoundStrategyCreator> NotFoundStrategyFactory;

} // namespace flowseparator
} // namespace ldk
} // namespace wns

#endif // WNS_LDK_FLOWSEPARATOR_NOTFOUNDSTRATEGY_HPP
