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


/**
 * @namespace wns::ldk
 * @ingroup ldk
 * @brief Layer Development Kit
 *
 */

/**
 * @defgroup ldk Layer Development Kit
 * @ingroup libwns
 *
 * LDK is a set of tools for building protocol layers out of smaller units, the
 * functional units (FUs). In the sub namespaces of LDK you will find many
 * ready-to-use FUs realizing different protocol functions. Before writing your
 * own FU, have a look at what is already there.
 *
 * Other introductions to building/using FUs:
 * @li @ref FunctionalUnitPage "How to implement a FunctionalUnit"
 * @li @ref BuildingSub "Example for How to implement a FunctionalUnit"
 */

/**
 * @defgroup ldkaspects The 5 aspects of a FunctionalUnit
 * @ingroup ldk
 * @brief FU Mechanics - What makes a FU a FU?
 *
 */

/**
 * @namespace wns::ldk::arq
 * @ingroup ldk
 * @brief ARQ - Automatic Repeat reQuest implementations for ldk.
 *
 */

/**
 * @namespace wns::ldk::buffer
 * @ingroup ldk
 * @brief Buffer - Various implementations of buffering functional units.
 *
 */

/**
 * @namespace wns::ldk::command
 * @ingroup ldk
 * @brief Command - Commond control plane functional units (FlowControl,...)
 *
 */

/**
 * @namespace wns::ldk::concatenation
 * @ingroup ldk
 * @brief Concatenation - Concat several compounds.
 *
 */

/**
 * @namespace wns::ldk::crc
 * @ingroup ldk
 * @brief CRC - Cyclic Redundancy Check implementation for ldk.
 *
 */

/**
 * @namespace wns::ldk::fcf
 * @ingroup ldk
 * @brief FCF - Frame Configuration Framework.
 *
 */

/**
 * @namespace wns::ldk::fun
 * @ingroup ldk
 * @brief FUN - FUN implementations (Main and Sub FUNs).
 *
 */

/**
 * @namespace wns::ldk::helper
 * @ingroup ldk
 * @brief Classes used by ldk internally.
 *
 */

/**
 * @namespace wns::ldk::multiplexer
 * @ingroup ldk
 * @brief Multiplexer - FUs supporting the configuration of branching FUNs.
 *
 */

/**
 * @namespace wns::ldk::probe
 * @ingroup ldk
 * @brief Probes - Probing FUs. Add those to your FUN for measurements.
 *
 */

/**
 * @namespace wns::ldk::sar
 * @ingroup ldk
 * @brief SAR - Segmentation And Reassembly implementations for ldk.
 *
 */

/**
 * @namespace wns::ldk::tools
 * @ingroup ldk
 * @brief Swiss army knife set of ready to use functional units.
 *
 */

#ifndef WNS_LDK_H
#define WNS_LDK_H

#define NAMESPACE_BEGIN(ns) namespace ns {
#define NAMESPACE_END }

#endif


