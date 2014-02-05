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

#ifndef WNS_LDK_FUN_SUB_HPP
#define WNS_LDK_FUN_SUB_HPP

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/node/Interface.hpp>
#include <WNS/Cloneable.hpp>

#include <map>
#include <string>


namespace wns { namespace ldk { namespace fun {

    /**
     * @brief Sub - A Functional Unit Network in a Functional Unit Network in a ...
     *
     * @sa @ref LayerDevelopmentPage
     *
     */
    class Sub :
        public virtual FUN
    {
    public:
        typedef std::map<std::string, FunctionalUnit*> FunctionalUnitMap;

        Sub(fun::FUN* fuNet);
        virtual ~Sub();

        //
        // FUN interface realization
        //

        // FUN construction
        virtual void addFunctionalUnit(const std::string& commandName, const std::string& functionalUnitName, FunctionalUnit* functionalUnit);
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

        //      virtual void reconfigureFUN(const wns::pyconfig::View& reconfig);
        virtual void removeFUsFromCommandPool();

        // FU access
        virtual FunctionalUnit* getFunctionalUnit(const std::string& name) const;
        virtual bool knowsFunctionalUnit(const std::string& name) const;

        // setter
        virtual void setNameParentFU(std::string _name);

        // getter
        virtual CommandProxy* getProxy() const;
        virtual ILayer* getLayer() const;
        virtual std::string getName() const;
        virtual LinkHandlerInterface* getLinkHandler() const;
        virtual CommandReaderInterface* getCommandReader(const std::string& commandName) const;
        virtual std::string getNameParentFU() const;

        // delayed configuration
        void onFUNCreated();

		virtual void onShutdown();

        //
        // new stuff
        //
        fun::FUN*
        getParent()
        {
            return parent;
        } // getParent

            //		fun::Sub* clone() const;

    private:
        virtual bool _knowsFunctionalUnit(const std::string& name) const;
            /*
            template<RECEPTACLETYPE>
            void
            translateLink(Link<RECEPTACLETYPE>* link,
                          const std::map<RECEPTACLETYPE*,RECEPTACLETYPE*>& translate);
            */
        fun::FUN* parent;
        ILayer* layer;
        CommandProxy* proxy;
        FunctionalUnitMap fuMap;
        LinkHandlerInterface* linkHandler;
        std::string nameParentFU;
    };

}
}
}



#endif // NOT defined WNS_LDK_FUN_SUB_HPP


