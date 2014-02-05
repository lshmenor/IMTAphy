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

#ifndef WNS_PDU_HPP
#define WNS_PDU_HPP


#include <WNS/RefCountable.hpp>
#include <WNS/IOutputStreamable.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/simulator/Bit.hpp>

#include <stdint.h>
#include <ostream>
#include <iostream>

namespace wns { namespace osi {

class PDU;
class PCI;
typedef wns::SmartPtr<PDU> PDUPtr;
/* We have to do that here not in PCI.hpp since typedefs can not be forward declared*/
typedef wns::SmartPtr<PCI> PCIPtr;

typedef unsigned long int pduType;
/**
 * @brief Protocol Data Unit
 */
class PDU :
    virtual public wns::RefCountable,
    public wns::IOutputStreamable
{
public:
	// Default constructor
	explicit
	PDU(PCI* aPCIPtr = NULL, PDU* anSDUPtr = NULL);


        // Copy constructor
	PDU(const PDU& aPDURef);

        // Destructor
	virtual ~PDU();

	// Normal output
	virtual void print(std::ostream& aStreamRef = std::cout) const;

	// Set protocol control information
	void setPCI(PCI* aPCIPtr);

	// Get protocol control information
	PCI* getPCI() const;

	// Set encoded higher layer information
	void setUserData(PDU* aUserDataPtr);

	// Get encoded higher layer information
	PDU* getUserData() const;

	// Set type of PDU
	void setPDUType(pduType aPDUType);

	// Get type of PDU
	pduType getPDUType() const;

	// Get unique identifier
	unsigned long int  getPDUId() const;

	// clone this PDU
	virtual PDU* clone();

	/**
	 * @brief Return size of the PDU as sum of the sizes of the containing PCI and SDU in bits.
	 */
	Bit getLengthInBits() const;

#ifndef NDEBUG
	static long int
	getExistingPDUs();

	static long int
	getMaxExistingPDUs();

	virtual size_t
	calcObjSize() const;
#endif

private:
	/**
	 * @brief Non-virutual interface to getLengthInBits
	 */
	virtual Bit doGetLengthInBits() const;
	//! \bProtocol \bControl \bInformation
	PCIPtr p_pciPtr;

	//! \bUser \bData
	PDUPtr p_userDataPtr;

    virtual std::string
    doToString() const;

#ifndef NDEBUG
	static long int existingPDUs;
	static long int maxExistingPDUs;
#endif
};

} // wns
} // osi

#endif  // NOT defined WNS_PDU_HPP

/*
  Local Variables:
  mode: c++
  folded-file: t
  End:
*/

