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

#ifndef WNS_PCI_HPP
#define WNS_PCI_HPP


#include <WNS/osi/PDU.hpp>

#include <WNS/RefCountable.hpp>
#include <WNS/IOutputStreamable.hpp>
#include <WNS/SmartPtr.hpp>

#include <WNS/simulator/Bit.hpp>
#include <iostream>

namespace wns { namespace osi {

/*!\brief Class \bPCI: Protocol Control Information */
class PCI :
    virtual public wns::RefCountable,
    public wns::IOutputStreamable
{
    friend class PDU;

public:

    //# Public types and constants

    //! All possible types of protocol data units
    static const pduType unknown = 0;


    //# Constructors and destructor
	// Default constructor
    PCI(pduType aPDUType = unknown, const unsigned long int aPDULength = 0);

    // Default copy constructor is correct
    // Destructor
    virtual ~PCI();

    // Normal output
    virtual void print(std::ostream& aStreamRef = std::cout) const;

    //# Public members
    PCI(const PCI &that);

    // Set type of PDU
    void setPDUType(pduType aPDUType);
    // Get type of PDU
    pduType getPDUType() const;

    // Get unique identifier
    unsigned long int getPDUId() const;

    /**
     * @brief Set length of the SDU - DEPRECATED!
     *
     * @note This method is deprecated as of version 6.0.
     *
     * Derive from PCI instead and overwrite getSize()
     * and getSDUSize() instead.
     */
    virtual void setPDULength(Bit aPDULength);

    /**
     * @brief Return length of the SDU - DEPRECATED!
     *
     * @note This method is deprecated as of version 6.0.
     *
     * Use getSize() and getSDUSize() instead.
     */
    virtual Bit getPDULength() const;

    /**
     * @brief Return the size of the PCI in bits.
     *
     * Override this if you implement your own PCI.
     */
    virtual Bit getSize() const;

    /**
     * @brief Return the size of the SDU in bits.
     *
     * The default implementation delegates the request
     * to the sdu set by setSDU. This is perfect in most cases.
     * If you wish to implement segmentation, override this
     * method and return the size of the segment
     * transmitted within the surrounding PDU.
     *
     * Even if the SDU gets segmented, the SDU object will
     * be transmitted in every PDU. In that case, pdu->getPCI()->getSDUSize()
     * and pdu->getUserData()->getSize() will differ. The first will
     * return the size of the segment, the second will return
     * the size of the SDU before segmentation.
     */
    virtual Bit getSDUSize() const;

    /**
     * @brief Return the attached SDU.
     *
     */
    virtual PDUPtr getSDU() const;

#ifndef NDEBUG
	virtual size_t
	calcObjSize() const
	{
		return sizeof(*this);
	}
#endif

protected:
    /**
     * @brief Introduce the SDU to describe.
     *
     */
    virtual void 
    setSDU(PDUPtr& sdu);

private:

    virtual std::string
    doToString() const;

    //# Internal private members

    //! Type of PDU
    pduType p_pdu;

    //! All PCIs have a unique id.
    unsigned long int p_pduId;

    /* Id counter, which is incremented whenever the default or the
      param constructor is used. */
    static unsigned long int p_pduIdCounter;

    //! Length of PDU
    Bit p_pduLength;

    //! SDU
    PDUPtr p_sduPtr;
};

} // wns
} // osi

#endif  // WNS_PCI_HPP

/*
Local Variables:
mode: c++
folded-file: t
End:
*/


