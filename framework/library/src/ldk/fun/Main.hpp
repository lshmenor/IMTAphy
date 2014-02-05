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

#ifndef WNS_LDK_FUN_MAIN_HPP
#define WNS_LDK_FUN_MAIN_HPP

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/node/Interface.hpp>

#include <map>
#include <string>

namespace wns { namespace logger {
    class Logger;
}
}

namespace wns { namespace ldk {
    class ILayer;
}
}

namespace wns { namespace ldk { namespace fun {

    /**
     * @brief FUN - Functional Unit Network
     *
     * @sa @ref LayerDevelopmentPage
     *
     */
    class Main :
        public virtual FUN
    {
    public:
        typedef std::map<std::string, FunctionalUnit*> FunctionalUnitMap;

        /**
         * @deprecated Don't use this constructor any more!
         */
        Main(ILayer* layer);

        /**
         * @brief This is the new default constructor for a FUN.
         *
         * @todo: ksw,msg
         * - change all calls of the deprecated constructor to calls of the new one
         */
        Main(ILayer* layer, const wns::pyconfig::View& _config);

        virtual ~Main();

        //
        // FUN interface realization
        //

        // FUN construction
        virtual void addFunctionalUnit(const std::string& commandName,
                           const std::string& functionalUnitName,
                           FunctionalUnit* functionalUnit);
        virtual void addFunctionalUnit(const std::string& name, FunctionalUnit* functionalUnit);
        virtual void removeFunctionalUnit(const std::string& name);

        virtual void connectFunctionalUnit(
                    const std::string& upperName, const std::string& lowerName,
                    const std::string& srcPort = "SinglePort", const std::string& dstPort = "SinglePort");

        virtual void upConnectFunctionalUnit(
                    const std::string& upperName, const std::string& lowerName,
                    const std::string& srcPort = "SinglePort", const std::string& dstPort = "SinglePort");

        virtual void downConnectFunctionalUnit(
                    const std::string& upperName, const std::string& lowerName,
                    const std::string& srcPort = "SinglePort", const std::string& dstPort = "SinglePort");

        // virtual void reconfigureFUN(const wns::pyconfig::View& reconfig);

        // FU access
        virtual FunctionalUnit* getFunctionalUnit(const std::string& name) const;
        virtual bool knowsFunctionalUnit(const std::string& name) const;

        // getter
        virtual CommandProxy* getProxy() const;
        virtual ILayer* getLayer() const;
        virtual std::string getName() const;
        virtual LinkHandlerInterface* getLinkHandler() const;
        virtual CommandReaderInterface* getCommandReader(const std::string& commandName) const;

        //
        // delayed configuration with optional verbosity
        //
        void onFUNCreated(wns::logger::Logger* logger = NULL);

    	virtual void onShutdown();

    private:
        ILayer* layer;
        CommandProxy* proxy;
        FunctionalUnitMap fuMap;
        LinkHandlerInterface* linkHandler;
        wns::logger::Logger logger;
    };

}
}
}



#endif // NOT defined WNS_LDK_FUN_MAIN_HPP


