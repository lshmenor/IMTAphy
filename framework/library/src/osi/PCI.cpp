/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WNS/osi/PCI.hpp>

#include <sstream>

using namespace wns::osi;

//! Default constructor

PCI::PCI(pduType aPDUType, const unsigned long int aPDULength) :
      p_pdu(aPDUType),
      p_pduLength(aPDULength),
      p_sduPtr(PDUPtr())
{
    p_pduId = p_pduIdCounter++;
}

PCI::PCI(const PCI& that) :
    //wns::RefCountable(),
    wns::IOutputStreamable(),
	p_pdu(that.p_pdu),
	p_pduId(that.p_pduId),
	p_pduLength(that.p_pduLength),
	p_sduPtr(that.getSDU())
{
}

//! Destructor
PCI::~PCI()
{
}

//! Normal output
void PCI::print(std::ostream& aStreamRef) const
{
    aStreamRef << "pdu          = " << p_pdu          << std::endl
               << "pduId        = " << p_pduId        << std::endl
               << "pduIdCounter = " << p_pduIdCounter << std::endl;
    aStreamRef << "p_pduLength  = " << p_pduLength    << std::endl;
}

std::string
PCI::doToString() const
{
    std::stringstream ss;
    ss << "pdu          = " << p_pdu          << std::endl
       << "pduId        = " << p_pduId        << std::endl
       << "pduIdCounter = " << p_pduIdCounter << std::endl;
    ss << "p_pduLength  = " << p_pduLength    << std::endl;
    return ss.str();
}

//! Set type of PDU
void PCI::setPDUType(pduType aPDUType)
{
    p_pdu = aPDUType;
}

//! Get type of PDU
pduType PCI::getPDUType() const
{
    return p_pdu;
}

//! Get unique identifier
unsigned long int PCI::getPDUId() const
{
    return p_pduId;
}

// Set length of PDU
void PCI::setPDULength(Bit aPDULength)
{
    p_pduLength = aPDULength;
}

// Get length of PDU
Bit PCI::getPDULength() const
{
    return getSize() + getSDUSize();
}

Bit
PCI::getSize() const
{
    return 0;
} // getSize

Bit
PCI::getSDUSize() const
{
    return p_pduLength;
} // getSDUSize

void
PCI::setSDU(PDUPtr& sdu)
{
    p_sduPtr = sdu;
} // setSDU

PDUPtr
PCI::getSDU() const
{
    return p_sduPtr;
} // getSDU

/*! Id counter, which is incremented whenever the default or the
   param constructor is used. */
unsigned long int PCI::p_pduIdCounter = 1;

/*
Local Variables:
mode: c++
folded-file: t
End:
*/


