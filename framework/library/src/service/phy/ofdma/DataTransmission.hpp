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

#ifndef WNS_SERVICE_PHY_OFDMA_DATATRANSMISSION_HPP
#define WNS_SERVICE_PHY_OFDMA_DATATRANSMISSION_HPP

#include <WNS/node/Interface.hpp>
#include <WNS/CandI.hpp>
#include <WNS/service/Service.hpp>
#include <WNS/service/phy/ofdma/Pattern.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/PowerRatio.hpp>
#include <map>

namespace wns { namespace service { namespace phy { namespace ofdma {

    /**
	 * @brief Values OFDMA Phy
	 */
    struct Tune
    {
        double frequency;
        double bandwidth;
        int numberOfSubCarrier;

        bool operator==(const wns::service::phy::ofdma::Tune& other) const{
            return ( (frequency == other.frequency) &&
                     (bandwidth == other.bandwidth) &&
                     (numberOfSubCarrier == other.numberOfSubCarrier));
        }

        Tune& operator=(const wns::service::phy::ofdma::Tune& other){
            this->frequency = other.frequency;
            this->bandwidth = other.bandwidth;
            this->numberOfSubCarrier = other.numberOfSubCarrier;
            return (*this);
        }
    };

    struct BFIdu
    {
        osi::PDUPtr pdu;
        node::Interface* recipient;
        int subBand;
        PatternPtr pattern;
    };

    /** @brief Common Interface for Non-beamforming and beamforming Transmission/Reception */
    class TransmissionBase :
        public virtual service::Service
    {
    public:
        virtual
        ~TransmissionBase(){}

        /**
		 * @brief stop a transmission of the given osi::PDUPtr
		 */
        virtual void
        stopTransmission(osi::PDUPtr pdu, int subBand) = 0;

        /**
         * @brief informs whether station is currently receiving data or not
         */
        virtual bool
        isReceiving() const = 0;

        /**
         * @brief returns all active transmissions (for debugging)
         */
        virtual std::string
        printActiveTransmissions() const = 0;
    };

    /** @brief Interface for Non-beamforming Transmission */
    class NonBFTransmission :
        public virtual TransmissionBase
    {
    public:
        virtual
        ~NonBFTransmission(){}

        /**
		 * @brief start unicast transmission
		 */
        virtual void
        startUnicast(osi::PDUPtr pdu,
                     wns::node::Interface* recipient,
                     int subBand,
                     wns::Power requestedTxPower,
                     int numberOfSpatialStreams) = 0;

        /**
		 * @brief start unicast transmission
		 */
        virtual void
        startUnicast(osi::PDUPtr pdu,
                     wns::node::Interface* recipient,
                     int subBand,
                     wns::Power requestedTxPower,
                     wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr) = 0;

        /**
		 * @brief start broadcast transmission
		 */
        virtual void
        startBroadcast(osi::PDUPtr pdu,
                       int subBand,
                       wns::Power requestedTxPower,
                       int numberOfSpatialStreams) = 0;
        /**
		 * @brief start broadcast transmission
		 */
        virtual void
        startBroadcast(osi::PDUPtr pdu,
                       int subBand,
                       wns::Power requestedTxPower,
                       wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr) = 0;
    };

    /** @brief Interface for beamforming Transmission */
    class BFTransmission :
        public virtual  TransmissionBase
    {
    public:
        virtual
        ~BFTransmission(){}

        virtual void
        startTransmission(wns::osi::PDUPtr pdu,
                          wns::node::Interface* recipient,
                          int subBand,
                          PatternPtr pattern,
                          wns::Power requestedTxPower,
                          int numberOfSpatialStreams) = 0;

        virtual void
        startTransmission(wns::osi::PDUPtr pdu,
                          wns::node::Interface* recipient,
                          int subBand,
                          PatternPtr pattern,
                          wns::Power requestedTxPower,
                          wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr) = 0;

    };

	/** @brief Interface for modifying all RF related settings */
	class RFSettings :
		public virtual service::Service
	{
	public:
		virtual
		~RFSettings(){}

		/** @brief set Tx Tuning */
		virtual void
		setTxTune(const Tune& txTune) = 0;

		/** @brief set Rx Tuning */
		virtual void
		setRxTune(const Tune& rxTune) = 0;

		/** @brief retrieve currently set Rx Tuning */
		virtual Tune
		getRxTune() const = 0;

		/** @brief retrieve currently set Tx Tuning */
		virtual Tune
		getTxTune() const = 0;

 		/** @brief retrieve info about the used power per subband */
 		virtual wns::Power
 		getMaxPowerPerSubband() const = 0;

		/** @brief retrieve info about the max total output power (sum
		 * of all subbands) */
		virtual wns::Power
		getMaxOutputPower() const = 0;

		/** @brief retrieve info about the EIRP restriction */
		virtual bool
		isEIRPLimited() const = 0;

		/** @brief method for fast swapping of Rx/Tx frequencies, e.g. in an FDD relay */
		virtual void
		setTxRxSwap(bool reverse) = 0;
	};

	/** @brief Interface for SINR estimation in the beamforming case */
	class SINREstimation :
		public virtual service::Service
	{
	public:
		virtual
		~SINREstimation(){}

		/** @brief estimate SINR in the beamforming Rx case */
		virtual std::map<wns::node::Interface*, wns::Ratio>
		calculateSINRsRx(const std::vector<node::Interface*>& combination,
				 wns::Power Iinter) = 0;

		/** @brief estimate SINR in the beamforming Tx case */
		virtual std::map<wns::node::Interface*, wns::Ratio>
		calculateSINRsTx(const std::map<wns::node::Interface*, wns::Power>& Station2NoisePlusIintercell,
				 wns::Power x_friendlyness,
				 wns::Power txPower) = 0;

		/** @brief estimate C and I separately in the beamforming Rx case */
		virtual std::map<wns::node::Interface*, wns::CandI>
		calculateCandIsRx(const std::vector<node::Interface*>& /* combination */,
				  wns::Power /* Iinter */) = 0;

		/** @brief estimate C and I separately in the beamforming Tx case */
		virtual std::map<wns::node::Interface*, wns::CandI>
		calculateCandIsTx(const std::map<wns::node::Interface*, wns::Power>& /* Station2NoisePlusIintercell */,
				  wns::Power /* x_friendlyness */,
				  wns::Power /* txPower */) = 0;

		virtual void
		setPowerReceivedForStation(wns::node::Interface* stack, wns::Power _rxPower) = 0;

		virtual void
		setTxPowerForStation(wns::node::Interface* stack, wns::Power _txPower) = 0;
	};

	/** @brief Interface for steering the beams */
	class BeamForming :
		public virtual service::Service
	{
	public:
		virtual
		~BeamForming(){}

		virtual PatternPtr
		calculateAndSetBeam(wns::node::Interface *id,
				    const std::vector<node::Interface*>& undesired,
				    wns::Power IinterPlusNoise) = 0;
		virtual double
		estimateDoA(wns::node::Interface *id) = 0;

		virtual void
		setCurrentReceivePatterns(std::map<wns::node::Interface*, PatternPtr>) = 0;

		virtual void
		insertReceivePattern(wns::node::Interface*, PatternPtr) = 0;

		virtual void
		removeReceivePattern(wns::node::Interface*) = 0;
	};

	/** @brief aggregates the beamforming interfaces */
	class BFInterface :
		public virtual BFTransmission,
		public virtual SINREstimation,
		public virtual BeamForming,
		public virtual RFSettings
	{
	public:
		virtual
		~BFInterface(){}
		//
	};

	/** @brief aggregates the non-beamforming interfaces */
	class NonBFInterface :
		public virtual NonBFTransmission,
		public virtual RFSettings
	{
	public:
		virtual
		~NonBFInterface(){}
		//
	};

	/**
	 * @brief OFDMA Phy Data Transmission Service, aggregates everything (for backward compatibility)
	 */
	class DataTransmission :
		public virtual BFInterface,
		public virtual NonBFInterface
	{
	public:
		virtual
		~DataTransmission(){}
	};

} // ofdma
} // phy
} // service
} // wns


#endif // WNS_SERVICE_PHY_OFDMA_DATATRANSMISSION



