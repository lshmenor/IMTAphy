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

#ifndef WNS_LDK_FLOWSEPARATOR_CREATORSTRATEGY_HPP
#define WNS_LDK_FLOWSEPARATOR_CREATORSTRATEGY_HPP

#include <WNS/ldk/FunctionalUnit.hpp>


namespace wns { namespace ldk { namespace flowseparator {

    /**
     * @brief Interface for New Instance creation.
     *
     */
    class CreatorStrategy
    {
    public:
        virtual
        ~CreatorStrategy()
        {
        }

        /** @brief
         * Return a freshly created FU
         */
        virtual FunctionalUnit*
        create() const = 0;

            /** @brief
             * Return a prototype FU
             */
            virtual FunctionalUnit*
            createPrototype() const = 0;
    };

    /**
     * @brief Create FUs for unknown flows using a prototype.
     *
     */
    class PrototypeCreator :
        public virtual CreatorStrategy
    {
    public:
        PrototypeCreator(fun::FUN* fuNet, const pyconfig::View& config);

        PrototypeCreator(fun::FUN* fuNet, std::string name, FunctionalUnit* prototype);

        FunctionalUnit*
        create() const;

        FunctionalUnit*
        createPrototype() const;

    private:
        FunctionalUnit* prototype;
    };

    /**
     * @brief Create FUs for unknown flows using the prototype config.
     *
     */
    class ConfigCreator :
        public virtual CreatorStrategy
    {
    public:
        ConfigCreator(fun::FUN* _fun, const pyconfig::View& _config);

        FunctionalUnit*
        create() const;

        FunctionalUnit*
        createPrototype() const;

    private:
        wns::ldk::fun::FUN* fun;
        wns::pyconfig::View config;
        std::string creatorName;
        std::string commandName;
    };

    typedef FUNConfigCreator<CreatorStrategy> CreatorStrategyCreator;
    typedef wns::StaticFactory<CreatorStrategyCreator> CreatorStrategyFactory;

} // namespace flowseparator
} // namespace ldk
} // namespace wns

#endif // WNS_LDK_FLOWSEPARATOR_CREATORSTRATEGY_HPP
