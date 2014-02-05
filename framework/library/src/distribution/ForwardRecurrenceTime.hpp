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

#ifndef WNS_DISTRIBUTION_FRT_HPP
#define WNS_DISTRIBUTION_FRT_HPP

#include <WNS/distribution/Distribution.hpp>
#include <WNS/distribution/Uniform.hpp>

namespace wns { namespace distribution {

    /**
     * @brief this function gives back a random value
     * that is suitable as a forward recurrence time
     * when the random value given as an argument
     * is drawn from the original distribution.
     * The first interarrival time should be calculated like this.
     */
    inline double
    forwardRecurrenceTime(double randomValue)
    {
        StandardUniform dis;
        return randomValue * dis();
    }

}
}

#endif // NOT defined WNS_DISTRIBUTION_FRT_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/

