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

#ifndef WNS_LDK_SAR_DYNAMICSAR_HPP
#define WNS_LDK_SAR_DYNAMICSAR_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/RefCountable.hpp>

#include <list>

namespace wns { namespace search {
        class ISearch;
        }

    namespace ldk { namespace sar {

            class DynamicSARCommand
                : public Command
            {
            public:
                struct SegmentationInfo
                    : public RefCountable
                {
                    SegmentationInfo()
                        : RefCountable(),
                          numberSegments(0),
                          receivedSegments()
                    {
                    }

                    int numberSegments;
                    std::list<int> receivedSegments;
                };

                typedef SmartPtr<SegmentationInfo> SegmentationInfoPtr;

                DynamicSARCommand()
                {
                    local.segmentNumber = 0;
                    magic.segmentSize = 1;
                    magic.segInfoPtr = SegmentationInfoPtr();
                }

                struct {
                    int segmentNumber;
                } local;

                struct
                {
                }
                peer;

                struct
                {
                    int segmentSize;
                    SegmentationInfoPtr segInfoPtr;
                } magic;

            };


            /**
             * @brief DynamicSAR implementation of the FU interface.
             *
             */
            class DynamicSAR
                : public fu::Plain<DynamicSAR, DynamicSARCommand>
            {
            public:
                // FUNConfigCreator interface realisation
                DynamicSAR(fun::FUN* fuNet, const wns::pyconfig::View& config);
                ~DynamicSAR();

                // SDU and PCI size calculation
                void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

                bool
                compare(Bit currentSegmentSize);

            private:
                virtual void
                doSendData(const CompoundPtr& compound);

                virtual void
                doOnData(const CompoundPtr& compound);

                virtual bool
                doIsAccepting(const CompoundPtr& compound) const;

                virtual void
                doWakeup();

                void
                sendSegments();

                Bit maxSegmentSize_;
                wns::search::ISearch* searchAlgo_;

                CompoundPtr currentCompound_;
                Bit currentCompoundSize_;
                Bit currentCompoundSentSize_;
                int segmentNumber_;
                int test;
                DynamicSARCommand::SegmentationInfoPtr currentSegInfoPtr_;

                logger::Logger logger_;

            };

        } // sar
    } // ldk
} // wns

#endif // NOT defined WNS_LDK_SAR_DYNAMICSAR_HPP


