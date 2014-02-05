/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
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

#ifndef _PHYMODESTUB_HPP
#define _PHYMODESTUB_HPP

#include <string>
#include <WNS/Enum.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>

//namespace rise { namespace plmapping {
namespace wns { namespace scheduler { namespace tests {

	/** @brief typedef to correspond with the ENUM Modulations */
	typedef wns::service::phy::phymode::Modulation Modulation;
	typedef wns::service::phy::phymode::Coding Coding;

	const int UNDEFINED_PHYMODE    = 0;
	const Modulation UNDEFINED_MODULATION = 0;
	const Coding UNDEFINED_CODING     = 0;
	const Modulation MAX_MODULATIONS      = 8;
	const int MOD_BITS             = 6;
	const int MOD_MASK             = 63;

	/**
		@brief the integer equivalents match the number of bits per modulation
		symbol (baud). This is used to correctly return the MI per Bit
	*/
	ENUM_BEGIN(Modulations);
	//ENUM(UNDEFINED_MODULATION,  0);
	ENUM(BPSK,  1);
	ENUM(QPSK,  2);
	ENUM(QAM8,  3);
	ENUM(QAM16, 4);
	ENUM(QAM32, 5);
	ENUM(QAM64, 6);
	ENUM(QAM128,7);
	ENUM(QAM256,8);
	//ENUM(MAX_MODULATIONS,9); // look at #define MAX_MODULATIONS above!
	ENUM_END();

	/**
		@brief Class encapsulating Modulation and Coding -> PHY Mode. For
		Interoperation with other modules (e.g.) the schedulers, which represent
		the PHYModes with an int only, this class provides the possibility to
		map each combination of Modulation and coding to a unique integer and
		vice versa. The rightmost 4 Bits of the integer represent the coding,
		allowing for up to 16 different coding schemes. the other bits represent
		the modulation.
		@author Rainer Schoenen (rs@comnets.rwth-aachen.de)
	*/
	/** rise::plmapping::PhyMode */
	class PhyMode :
		public wns::service::phy::phymode::PhyModeInterface,
		public wns::Cloneable<PhyMode>,
        virtual public IOutputStreamable
	{
	private:
		/** @brief Helper object for the conversion from/to strings, ints and codeRates */
		//rise::plmapping::CoderMapping* coderMapper;
		/** @brief modulation for this phymode */
		Modulation modulation; // int
		/** @brief coding for this phymode */
		Coding coding;
		double codeRate;
		std::string codingString;
		/** @brief helper values for performance calculations */
		bool subCarriersPerSubChannelKnown;
		unsigned int subCarriersPerSubChannel;
		bool symbolDurationKnown;
		simTimeType symbolDuration; // (symbolLength+cpLength) = 1/symbolRate
		bool dataRateKnown;
		double dataRate;
	public:
		/** @brief Exception thrown when features used with undefined PhyMode */
		class UndefinedException :
			public wns::Exception
		{
		public:
			UndefinedException() :
				wns::Exception("Undefined PhyMode")	{}
                       ~UndefinedException() throw() {}
		};

		/** @brief Construct empty=undefined PhyMode. Means: "feature not used" */
		explicit // explicit forbids implicit type conversion from int
		PhyMode();

		/** @brief Construct PHY Mode from PyConfig	*/
		explicit
		PhyMode(const wns::pyconfig::View& config);

		/** @brief Construct PHY Mode from int. Used for conversion. */
		//explicit PhyMode(int intPhyMode);

		virtual ~PhyMode();

		virtual void setSubCarriersPerSubChannel(unsigned int _subCarriersPerSubChannel);
		virtual void setSymbolDuration(simTimeType _symbolDuration);
		virtual void cloneParameters(const PhyMode& other);
		virtual bool isValid() const;
		/** @brief this PhyMode can offer a data Rate */
		virtual bool dataRateIsValid() const;

		/** @brief access method for the code rate (double) */
		virtual double getCodeRate() const;

		/** @brief bit rate per OFDM symbol */
		virtual double getBitsPerSymbol() const;

		/** @brief number of bits that fit into certain number of OFDM symbols */
		//virtual unsigned int getBitCapacity(double numberOfOFDMSymbols) const;

		/** @brief datarate in [bits per second] */
		/** symbolRate = 1.0/symbolDuration */
		/** subCarriersPerSubChannel */
		virtual double getDataRate(double _symbolRate, unsigned int _subCarriersPerSubChannel=1) const;
		/** @brief datarate in [bits per second] */
		virtual double getDataRate() const;

		/** @brief MI = f(SINR) (mutual information) */
		virtual double getSINR2MI(const wns::Ratio& sinr) const;
		/** @brief MIB = f(SINR) (mutual information per bit, [0..1]) */
		virtual double getSINR2MIB(const wns::Ratio& sinr) const;
		/** @brief PER = f(MI,bits), blockLength bits (=payload[net], not CWL[gross])*/
		virtual double getMI2PER(const double mi, unsigned int bits) const;
		/** @brief PER = f(SINR,bits), blockLength bits (=payload[net], not CWL[gross]) */
	    virtual double getSINR2PER(const wns::Ratio& sinr, unsigned int bits) const;
        virtual wns::Ratio getMIB2SINR(const double& mib) const;
		/** @brief calculate capacity[bits] of a phyResource */
		virtual unsigned int
		getBitCapacityFractional(simTimeType duration) const;

		/** @brief calculate capacity[bits] of a burst of certain duration */
		/** @see use like phyCapacity.getBitCapacity(...) in PhyUser.cpp */
		/** here the correct OFDM symbol boundaries are taken into account */
		//virtual unsigned int
		//getBitCapacity(simTimeType duration) const;

		/** @brief access method for the modulation (string) */
		virtual std::string	getModulationString() const;
		/** @brief access method for the coding (string) */
		virtual std::string	getCodingString() const;
		/** @brief access method for the modulation&coding (string) */
		virtual std::string getString() const;
		/** @brief true, if string matches to the PhyMode */
		virtual bool nameMatches(const std::string& name) const;

		/** @brief Convert PHY Mode (M&C) to one int */
		virtual unsigned int toInt() const;

		/** @brief access method for the modulation */
		virtual Modulation getModulation() const { return modulation;}

		/** @brief access method for the coding */
		virtual Coding getCoding() const { return coding;}

		/** @brief give a const reference to an empty PhyMode */
		static inline const PhyMode& emptyPhyMode() {
			static PhyMode emptyPhyMode = PhyMode();
			return emptyPhyMode;
		}

	protected:
        virtual std::string doToString() const;

		/** @brief try to calculate dataRate */
		virtual void calculateDataRate();

		//static int staticCounter;
	}; // class definition ends

}}}
#endif // RISE_PLMAPPING_PHYMODE_HPP
