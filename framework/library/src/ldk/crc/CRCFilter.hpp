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

#ifndef WNS_LDK_CRC_CRCFILTER_HPP
#define WNS_LDK_CRC_CRCFILTER_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <list>

namespace wns { namespace ldk { namespace crc {

    class CRC;

    /**
     * @brief The CRCFilter FU avoids dropping of pre-defined compound
     * types
     *
     * The CRCFilter FU may only be used together with a CRC FU operating in
     * marking mode. It is recommended to insert the CRCFilter FU right
     * above the CRC FU. The CRCFilter FU drops each compound that failed the
     * CRCheck of the CRC FU \b and does not contain at least one activated
     * command of one FU listed in the configuration of the CRCFilter
     * FU. This way, it is possible to simulate error-free transmissions of
     * pre-defined types of compounds, e.g. compounds carrying control
     * information, over erroneous transmission links that avoids the
     * necessity to implement error handling procedures.
     *
     * @note:
     * <ul>
     * <li> The error-freeness is guaranteed only at that level of the
     * connection the CRC and the CRCFilter FU are working, that means
     * e.g. if the CRC/CRCFilter is used in the IP layer and there is
     * already another crc in the DLL only that compounds may be kept by the
     * CRCFilter that would have been filtered by the CRC in the IP layer. Hence,
     * the transmission of the pre-defined compound types would not be
     * error-free from a node-wide point of view.
     * <li> Between the CRCFilter FU and the FUs listed in its configuration
     * to determine whether a compound shall be discarded there \b must \b
     * not be any FUs that wrap compounds like an SAR or Concatenation FU
     * because then the commands of the listed FUs would be invisible to the
     * CRCFilter FU and \b all compounds which failed the CRCheck would be
     * dropped.
     * </ul>
     */
    class CRCFilter :
        public fu::Plain<CRCFilter>,
        virtual public SuspendableInterface,
        public SuspendSupport
    {
        /**
         * @brief Container for FUs
         */
        typedef std::vector<wns::ldk::FunctionalUnit*> FUContainer;

    public:
        /**
         * @brief Constructor
         */
        explicit
        CRCFilter(fun::FUN* fuNet, const wns::pyconfig::View& _config);

        /**
         * @brief Destructor
         */
        virtual
        ~CRCFilter()
        {
        };

    private:
        /**
         * @name FunctionalUnit interface
         */
        //@{
        virtual void
        doSendData(const CompoundPtr& compound)
        {
            getConnector()->getAcceptor(compound)->sendData(compound);
        } // doSendData

        virtual void
        doOnData(const CompoundPtr& compound);

        virtual bool
        doIsAccepting(const CompoundPtr& compound) const
        {
            return getConnector()->hasAcceptor(compound);
        } // doIsAccepting

        virtual void
        doWakeup()
        {
            getReceptor()->wakeup();
        } // wakeup

        virtual void
        onFUNCreated();

        virtual bool
        onSuspend() const
        {
            return true;
        } // onSuspend
        //@}

        /**
         * @brief CRC FU whose command is used to decide whether the
         * transmission of the compound was successful or failed
         */
        struct Friends
        {
            CRC* crc;
        } friends;

        /**
         * @brief Compounds that contain at least one activated command
         * of one of these FUs won't be filtered out
         */
        FUContainer fus;

        wns::pyconfig::View config;
        logger::Logger logger;
    };

} // crc
} // ldk
} // wns

#endif // NOT defined WNS_LDK_CRC_CRCFILTER_HPP


