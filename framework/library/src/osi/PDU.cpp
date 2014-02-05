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

#include <WNS/osi/PDU.hpp>
#include <WNS/osi/PCI.hpp>

#include <sstream>


using namespace wns::osi;

#ifndef NDEBUG
long int PDU::existingPDUs = 0;
long int PDU::maxExistingPDUs = 0;
#endif

PDU::PDU(PCI* aPCIPtr, PDU* anSDUPtr) :
    p_pciPtr(PCIPtr()),
    p_userDataPtr(PDUPtr())
{
    if(aPCIPtr != NULL)
        p_pciPtr = PCIPtr(aPCIPtr);

    if(anSDUPtr != NULL)
        p_userDataPtr = PDUPtr(anSDUPtr);

	if(p_pciPtr != NULL)
		p_pciPtr->setSDU(p_userDataPtr);

#ifndef NDEBUG
	++existingPDUs;
	maxExistingPDUs = std::max(maxExistingPDUs, existingPDUs);
#endif // NDEBUG
}

PDU::PDU(const PDU& aPDURef) :
    //wns::RefCountable(),
    wns::IOutputStreamable(),
	p_pciPtr(aPDURef.p_pciPtr),
	p_userDataPtr(aPDURef.p_userDataPtr)
{
#ifndef NDEBUG
	++existingPDUs;
	maxExistingPDUs = std::max(maxExistingPDUs, existingPDUs);
#endif // NDEBUG
}

PDU::~PDU()
{
#ifndef NDEBUG
	--existingPDUs;
#endif // NDEBUG
}

void PDU::print(std::ostream& aStreamRef) const
{
	aStreamRef << "p_pciPtr      = " << p_pciPtr      << std::endl
		   << "p_userDataPtr = " << p_userDataPtr << std::endl;
}

std::string
PDU::doToString() const
{
    std::stringstream ss;
    ss << "p_pciPtr      = " << p_pciPtr      << std::endl;
    ss << "p_userDataPtr = " << p_userDataPtr << std::endl;
    return ss.str();
}

void PDU::setPCI(PCI* aPCIPtr)
{
	p_pciPtr = PCIPtr(aPCIPtr);
	if(p_pciPtr != NULL)
		p_pciPtr->setSDU(p_userDataPtr);
}

PCI* PDU::getPCI() const
{
	return p_pciPtr.getPtr();
}

void PDU::setUserData(PDU* aUserDataPtr)
{
    if(aUserDataPtr != NULL)
        p_userDataPtr = PDUPtr(aUserDataPtr);
    else
        p_userDataPtr = PDUPtr();

	if(p_pciPtr != NULL)
		p_pciPtr->setSDU(p_userDataPtr);
}

PDU* PDU::getUserData() const
{
	return p_userDataPtr.getPtr();
}

void PDU::setPDUType(pduType aPDUType)
{
	assure(p_pciPtr != NULL, "Can't set type of PDU without PCI!");
	p_pciPtr->setPDUType(aPDUType);
}

pduType PDU::getPDUType() const
{
	assure(p_pciPtr != NULL, "Can't get type of PDU without PCI!");
	return p_pciPtr->getPDUType();
}

unsigned long int PDU::getPDUId() const
{
	assure(p_pciPtr != NULL, "Can't get id of PDU without PCI!");
	return p_pciPtr->getPDUId();
}

PDU* PDU::clone()
{
	return new PDU(*this);
}

Bit
PDU::getLengthInBits() const
{
    assure(doGetLengthInBits() >= 0, "Length must be greater than zero");
	return doGetLengthInBits();
} // getSize

Bit
PDU::doGetLengthInBits() const
{
    assure(false, "doGetLengthInBits not implementted! Implement in derived class!");
	return 0;
}

#ifndef NDEBUG

long int PDU::getExistingPDUs()
{
	return existingPDUs;
}

long int PDU::getMaxExistingPDUs()
{
	return maxExistingPDUs;
}

size_t
PDU::calcObjSize() const
{
	// Init with container size
	size_t sum = sizeof( *this );
	if (p_pciPtr != NULL)
		// add PCI size
		sum += p_pciPtr->calcObjSize();
	if (p_userDataPtr != NULL)
		// add SDU size
		sum += p_userDataPtr->calcObjSize();
	return sum;
}

#endif // not defined NDEBUG

/*
  Local Variables:
  mode: c++
  folded-file: t
  End:
*/

