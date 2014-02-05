/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef WNS_LDK_ARQ_STOPANDWAITRC_HPP
#define WNS_LDK_ARQ_STOPANDWAITRC_HPP

#include <WNS/ldk/arq/ARQ.hpp>

#include <WNS/ldk/FunctionalUnitRC.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/IPortID.hpp>
#include <WNS/ldk/HasDownPort.hpp>
#include <WNS/ldk/HasUpPort.hpp>

#include <WNS/events/CanTimeout.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <string>

namespace wns { namespace ldk { namespace arq {


            class Data
                : public virtual IPortID
            {
            public:
                static const std::string name;
            };

            class Ack
                : public virtual IPortID
            {
            public:
                static const std::string name;
            };


            class StopAndWaitRCCommand
                : public ARQCommand
            {
            public:
                /**
                 * I - Information Frame (Data)
                 * RR - Receive Ready Frame (ACK)
                 */
                typedef enum {I, RR} FrameType;

                StopAndWaitRCCommand()
                {
                    peer.type = I;
                    peer.NS = 0;
                }

                struct {
                } local;

                struct {
                    // Typically 1 Bit in reality
                    FrameType type;
                    // Typically 1 Bit in reality
                    SequenceNumber NS;
                } peer;

                struct {} magic;

                // ARQCommand interface realization
                virtual bool
                isACK() const
                {
                    return peer.type == RR;
                }
            };


            class StopAndWaitRC
                : public FunctionalUnitRC< StopAndWaitRC >,
                  public CommandTypeSpecifier<StopAndWaitRCCommand>,
                  public HasUpPort< StopAndWaitRC >,
                  public HasDownPort< StopAndWaitRC, Port<Data> >,
                  public HasDownPort< StopAndWaitRC, Port<Ack> >,
                  public Cloneable< StopAndWaitRC >,
                  public events::CanTimeout
            {
            public:
                // FUNConfigCreator interface realisation
                StopAndWaitRC(fun::FUN* fuNet, const wns::pyconfig::View& config);

                virtual
                ~StopAndWaitRC();

                // SDU and PCI size calculation
                void
                calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

                template <typename PORTID>
                void
                doWakeup(PORTID);

                template <typename PORTID>
                void
                doOnData(const CompoundPtr& compound, PORTID);

                virtual bool
                doIsAccepting(const CompoundPtr& compound) const;

                virtual void
                doSendData(const CompoundPtr& compound);

            private:
                StopAndWaitRC();

                // CanTimeout interface realisation
                virtual void
                onTimeout();

                /**
                 * @brief Time between two transmissions of the same PDU.
                 */
                double resendTimeout;

                /**
                 * @brief Number of bits added to the header for each I-Frame
                 */
                int bitsPerIFrame;

                /**
                 * @brief Number of bits added to the header for each RR-Frame
                 */
                int bitsPerRRFrame;

                /**
                 * @brief Sequence number of the last packet sent.
                 */
                ARQCommand::SequenceNumber NS;

                /**
                 * @brief Sequence number of the last packet received.
                 */
                ARQCommand::SequenceNumber NR;

                /**
                 * @brief The last packet sent but not acknowledged yet.
                 *
                 * If the activeCompound is set to CompoundPtr(), this means no
                 * Compound is currently waiting for acknowledgment
                 */
                CompoundPtr activeCompound;

                /**
                 * @brief An ACK to be sent.
                 */
                CompoundPtr ackCompound;

                /**
                 * @brief Remember to send the activeCompound.
                 */
                bool sendNowData;


                logger::Logger logger;
            };

            /*            template <>
            void
            StopAndWaitRC::doWakeup(Port<Data>);

            template <>
            void
            StopAndWaitRC::doWakeup(Port<Ack>);

            template <>
            void
            StopAndWaitRC::doOnData(const CompoundPtr& compound, Port<Data>);

            template <>
            void
            StopAndWaitRC::doOnData(const CompoundPtr& compound, Port<Ack>);*/

        } //arq
    } //ldk
} //wns

#endif // NOT defined WNS_LDK_ARQ_STOPANDWAITRC_HPP

