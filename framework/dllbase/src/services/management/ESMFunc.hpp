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

#ifndef DLL_SERVICES_MANAGEMENT_ESMFUNC_HPP
#define DLL_SERVICES_MANAGEMENT_ESMFUNC_HPP

#include <WNS/PowerRatio.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/ldk/PyConfigCreator.hpp>

#include <WNS/service/phy/phymode/PhyModeMapperInterface.hpp>

#include<set>

namespace wns {
	namespace ldk {
		class ManagementServiceRegistry;
	}
}

namespace dll { namespace services { namespace management {

	class IESMFunc 
	{
	public:
        virtual wns::Ratio
        operator()(const std::list<wns::Ratio>&) = 0;
	};

	typedef wns::ldk::PyConfigCreator<IESMFunc> IESMFuncCreator;
	typedef wns::StaticFactory<IESMFuncCreator> IESMFuncFactory;

    class ESMFunc :
        public IESMFunc
    {
    public:
        ESMFunc(const wns::pyconfig::View&);

    protected:
        wns::service::phy::phymode::PhyModeMapperInterface* pmm_;
    };

    class ESMStub:
        public IESMFunc
    {
    public:
        ESMStub(const wns::pyconfig::View&);

        virtual wns::Ratio
        operator()(const std::list<wns::Ratio>&);
    };

    class LinearESM :
        public ESMFunc
    {
    public:
        LinearESM(const wns::pyconfig::View&);

        virtual wns::Ratio
        operator()(const std::list<wns::Ratio>&);

    };

    class LogESM :
        public ESMFunc
    {
    public:
        LogESM(const wns::pyconfig::View&);

        virtual wns::Ratio
        operator()(const std::list<wns::Ratio>&);

    };

    class MIESM :
        public ESMFunc
    {
    public:
        MIESM(const wns::pyconfig::View&);

        virtual wns::Ratio
        operator()(const std::list<wns::Ratio>&);

    private:
        typedef std::map<unsigned int, 
            wns::service::phy::phymode::PhyModeInterfacePtr> ModMap;

        ModMap phyModePerModulation_;
    };
                    

}}}
#endif // NOT DEFINED DLL_SERVICES_MANAGEMENT_IESMFUNC_HPP

