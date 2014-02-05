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

#ifndef WNS_LDK_MULTIPLEXER_OPCODEKEY_HPP
#define WNS_LDK_MULTIPLEXER_OPCODEKEY_HPP

#include <WNS/ldk/Key.hpp>


namespace wns { namespace ldk { namespace multiplexer {

    class OpcodeProvider;
    class OpcodeKeyBuilder;

    class OpcodeKey :
        public Key
    {
    public:
        OpcodeKey(const OpcodeKeyBuilder* builder, const CompoundPtr& compound);

        virtual bool operator<(const Key& _other) const;
        std::string str() const;

    private:
        int opcode;
    };


    class OpcodeKeyBuilder :
        public KeyBuilder
    {
        friend class OpcodeKey;

    public:
        OpcodeKeyBuilder(const fun::FUN* fuNet, const pyconfig::View& config);
        virtual void onFUNCreated();

        virtual ConstKeyPtr operator() (const CompoundPtr& compound, int /* direction */) const;

    private:
        const fun::FUN* fuNet;
        pyconfig::View config;

        struct Friends
        {
            OpcodeProvider* opcodeProvider;
        }
        friends;
    };


}
}
}

#endif // NOT defined WNS_LDK_MULTIPLEXER_OPCODEKEY_HPP


