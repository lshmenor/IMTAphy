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

#include <WNS/module/Base.hpp>

#include <WNS/Exception.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>

#include <TCP/CumulativeACK.hpp>
#include <TCP/CongestionControl.hpp>
#include <TCP/TCPHeader.hpp>

using namespace tcp;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	CumulativeACK,
	wns::ldk::FunctionalUnit,
	"tcp.CumulativeACK",
	wns::ldk::FUNConfigCreator
	);


CumulativeACK::CumulativeACK(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _pyco):
	wns::ldk::CommandTypeSpecifier<CumulativeACKCommand>(_fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<CumulativeACK>(),
	pyco(_pyco),
	logger(pyco.get("logger")),
	fun(_fun),
	ccStrategy(NULL),
	advertisedWindowSize(pyco.get<unsigned long int>("advertisedWindow")),
	sequenceNR(0),
	ackNR(0),
	lastACKInOrder(0),
	receivingCompounds(),
	sendingCompounds(),
	timeoutSendingCompounds(),
	ackCompound(wns::ldk::CompoundPtr()),
    copiedACKCompound(wns::ldk::CompoundPtr()),
    tcpHeaderReader(NULL)
{
	MESSAGE_SINGLE(NORMAL, logger, "TCP::CumulativeACK instance created.");

	ccStrategy = new CongestionControl(pyco.get<wns::pyconfig::View>("congestionControl"));

	MESSAGE_SINGLE(NORMAL, logger, "Initializing timeout value for segment retransmissions.");

	retransmissionTimeout = ccStrategy->getRetransmissionTimeout();

	MESSAGE_SINGLE(NORMAL, logger, "Initializing window size.");

	assure(sendCredit() > sendingCompounds.size(), "Wrong initialization of send credit or number of sendingCompounds!");

    wns::probe::bus::ContextProviderCollection localcpc(&getFUN()->getLayer()->getContextProviderCollection());

    windowSizeContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "tcp.cumulativeACK.windowSize"));

    sendCreditContextCollector = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(localcpc, "tcp.cumulativeACK.sendCredit"));
}


CumulativeACK::~CumulativeACK()
{
	delete ccStrategy;

	receivingCompounds.clear();
	sendingCompounds.clear();
	timeoutSendingCompounds.clear();
}


bool
CumulativeACK::doIsAccepting(const wns::ldk::CompoundPtr& _compound __attribute__ ((unused))) const
{
	assure(_compound, "doIsAccepting called with invalid compound.");
	MESSAGE_SINGLE(NORMAL, logger, "doIsAccepting called.");

	return sendCredit() > 0;
}


void
CumulativeACK::onFUNCreated()
{
    tcpHeaderReader = getFUN()->getCommandReader("tcp.tcpHeader");
    assure(tcpHeaderReader, "No reader for the TCP Header available!");
}


void
CumulativeACK::doOnData(const wns::ldk::CompoundPtr& _compound)
{
	assure(_compound, "doOnData called with an invalid compound.");

	CumulativeACKCommand* cumACKCmd = dynamic_cast<CumulativeACKCommand*>(getCommand(_compound->getCommandPool()));

	switch(cumACKCmd->peer.type)
	{
	case CumulativeACKCommand::I:
		MESSAGE_SINGLE(NORMAL, logger, "Received TCP segment with sequence-nr " << cumACKCmd->peer.sequenceNumber
			       << ". Expected sequence-nr.: " << ackNR);

		if (cumACKCmd->peer.sequenceNumber == ackNR)
		{
			// received segment is in order
			wns::ldk::CommandPool* ackPCI = createReply(_compound->getCommandPool());
			this->ackCompound = wns::ldk::CompoundPtr(new wns::ldk::Compound(ackPCI));
			CumulativeACKCommand* ackCommand = activateCommand(ackPCI);

			//catch DuplicateKeyValue exception
			try
			{
				receivingCompounds.insert(ackNR, _compound);
			}
			catch (wns::container::Registry<unsigned long int, wns::ldk::CompoundPtr>::DuplicateKeyValue)
			{
				// accept duplicate TCP segments
			}

			wns::ldk::CompoundPtr compound;

			while(receivingCompounds.knows(ackNR))
			{
				try
				{
					compound = receivingCompounds.find(ackNR);
				}
				catch (wns::Exception)
				{
					throw wns::Exception("No TCP segment available for sequence number " + ackNR);
				}

				// put to upper layer
				getDeliverer()->getAcceptor(compound)->onData(compound);

				receivingCompounds.erase(ackNR);

				// continue only if next sequence number has
				// been received yet
				ackNR++;
			}

			ackCommand->peer.type = CumulativeACKCommand::ACK;
			ackCommand->peer.ACKNumber = ackNR;
			ackCommand->peer.advertisedWindowSize = advertisedWindowSize - receivingCompounds.size();

			copiedACKCompound = ackCompound->copy();

            updateTCPHeader(this->ackCompound);
            
			MESSAGE_SINGLE(NORMAL, logger, "Sending ACK number: " << ackCommand->peer.ACKNumber);
			getConnector()->getAcceptor(this->ackCompound)->sendData(this->ackCompound);

			ackCompound = copiedACKCompound;

		}
		else if(cumACKCmd->peer.sequenceNumber < ackNR)
		{
			// duplication of an already acked segment; already
			// deleted from container
			assure(!receivingCompounds.knows(cumACKCmd->peer.sequenceNumber),
			       "Predecessor compound received. Already forwarded to upper FU.");

		}
		else
		{
			// received seqNr. > expected one -- buffering
			// received segment is not in order; send duplicate ACK
			// with highest in-order sequence number

			if (not receivingCompounds.knows(cumACKCmd->peer.sequenceNumber))
				receivingCompounds.insert(cumACKCmd->peer.sequenceNumber, _compound);

			if (!getFUN()->getProxy()->commandIsActivated(this->ackCompound->getCommandPool(), this))
				activateCommand(this->ackCompound->getCommandPool());

			CumulativeACKCommand* ackCommand __attribute__ ((unused)) = getCommand(this->ackCompound->getCommandPool());

			ackCommand->peer.type = CumulativeACKCommand::ACK;
			ackCommand->peer.ACKNumber = ackNR;
			ackCommand->peer.advertisedWindowSize = advertisedWindowSize - receivingCompounds.size();

			copiedACKCompound = ackCompound->copy();

            updateTCPHeader(this->ackCompound);
			// ackCompound has to be resent
			assure(ackCommand->peer.ACKNumber == ackNR, "Wrong acknowledgement number.");
			MESSAGE_SINGLE(NORMAL, logger, "Sending Duplicate ACK for segment " << ackCommand->peer.ACKNumber);
			getConnector()->getAcceptor(this->ackCompound)->sendData(this->ackCompound);

			ackCompound = copiedACKCompound;
		}
		break;

	case CumulativeACKCommand::ACK:
		MESSAGE_SINGLE(NORMAL, logger, "Received acknowledgement. ACK-Nr. " << cumACKCmd->peer.ACKNumber);

		// set the sending credit imposed by receiver
		// (sliding window mechanism)
		advertisedWindowSize = cumACKCmd->peer.advertisedWindowSize;

		// the ACK-nr might never be greater than sequence number of the next
		// segment to be sent
		assure(cumACKCmd->peer.ACKNumber <= sequenceNR,
		       "Invalid acknowledgement number!");

		if (cumACKCmd->peer.ACKNumber > lastACKInOrder)
		{
			/**
			 * @brief ACKNumber is the number of the _next_ segment
			 * expected. All segments up to this number - 1 have
			 * been successfully received.
			 * The next compounds to be deleted are ACKNumber-1 up
			 * to lastACKInOrder-1
			 */
			assure(sendingCompounds.knows(cumACKCmd->peer.ACKNumber-1) && timeoutSendingCompounds.knows(cumACKCmd->peer.ACKNumber-1),
			       "Sequence number " << cumACKCmd->peer.ACKNumber << " has already been acknowledged.");

			lastACKInOrder = cumACKCmd->peer.ACKNumber;

			// all segments up to ack-nr. - 1 successfully received
			// remove all predecessors
			unsigned long int tmp = lastACKInOrder - 1;
			while(sendingCompounds.knows(tmp))
			{
				// ack all unacked packets
				ccStrategy->onSegmentAcknowledged();

				try
				{
					timeoutSendingCompounds.find(tmp)->cancelTimeout();
				}
				catch (wns::Exception)
				{
					throw wns::Exception("Missing timeout event for TCP segment!");
				}
				sendingCompounds.erase(tmp);
				tmp--;
			}

			if (sendCredit() > 0)
				this->doWakeup();

		}
		else
		{
			// duplicate ACK segments have been acknowledged before
			assure(sendingCompounds.knows(cumACKCmd->peer.ACKNumber), "Invalid compound to be sent.");

			ccStrategy->onSegmentLoss(CongestionControl::DUPLICATE_ACK, cumACKCmd->peer.ACKNumber);

			if(ccStrategy->duplicateACKThresholdReached(cumACKCmd->peer.ACKNumber))
			{
				// fast retransmit
				MESSAGE_SINGLE(NORMAL, logger, "Fast retransmit for ACK-Nr. " << cumACKCmd->peer.ACKNumber);
				this->retransmitData(cumACKCmd->peer.ACKNumber);
			}
		}

		MESSAGE_SINGLE(NORMAL, logger, "Sliding window mechanism. Send credit: " << sendCredit());

		windowSizeContextCollector->put(ccStrategy->getWindowSize());
		sendCreditContextCollector->put(sendCredit());
		break;

	default:
		assure(false, "Unknown command type for TCP::CumulativeACK.");
		break;
	}
}



void
CumulativeACK::doSendData(const wns::ldk::CompoundPtr& _compound)
{
	assure(_compound, "doSendData called with an invalid compound");

		CumulativeACKCommand* cumACKCmd = activateCommand(_compound->getCommandPool());

		cumACKCmd->peer.sequenceNumber = sequenceNR;
		cumACKCmd->peer.type = CumulativeACKCommand::I;

		// create copy for possible retransmissions
		wns::ldk::CompoundPtr compound = _compound->copy();

		assure(!sendingCompounds.knows(sequenceNR), "Invalid compound to be sent.");

	if(isAccepting(_compound))
	{
		try
		{
			sendingCompounds.insert(sequenceNR, compound);
			// set timeout
			timeoutSendingCompounds.insert(sequenceNR, new Timeout(this, sequenceNR, ccStrategy->getRetransmissionTimeout()));
		}
		catch(wns::Exception)
		{
			throw wns::Exception("TCP segment already sent for sequence number: " + sequenceNR);
		}

        updateTCPHeader(_compound);
		MESSAGE_SINGLE(NORMAL, logger, "Sending data. Sequence number: " << sequenceNR);
		getConnector()->getAcceptor(_compound)->sendData(_compound);

		sequenceNR++;	
	}
	else
	{
		// buffer each compound
	}
}


void
CumulativeACK::retransmitData(const unsigned long int seqNr)
{
	wns::ldk::CompoundPtr _compound;
	try
	{
		_compound = sendingCompounds.find(seqNr);
	}
	catch (wns::Exception)
	{
		throw wns::Exception("No tcp segment available for sequence number " + ackNR);
	}

	wns::ldk::CompoundPtr compound = _compound->copy();

	CumulativeACKCommand* cmd __attribute__ ((unused)) = getCommand(_compound->getCommandPool());

	try
	{
		sendingCompounds.update(seqNr, compound);
		timeoutSendingCompounds.erase(seqNr);
		timeoutSendingCompounds.insert(seqNr, new Timeout(this, seqNr, ccStrategy->getRetransmissionTimeout()));
	}
	catch (wns::Exception)
	{
		throw wns::Exception("TCP segment not available!");
	}

	// compound already buffered, therefore always accepting
	MESSAGE_SINGLE(NORMAL, logger, "Retransmitting data. Sequence number: " << cmd->peer.sequenceNumber);

    updateTCPHeader(_compound);
	getConnector()->getAcceptor(_compound)->sendData(_compound);
}


void
CumulativeACK::doWakeup()
{
	MESSAGE_SINGLE(NORMAL, logger, "doWakeup called. Forwarding to upper FU.");

	if (sendCredit() > 0)
		getReceptor()->wakeup();
}


unsigned long int
CumulativeACK::min(const unsigned long int x, const unsigned long int y) const
{
	return (x<=y) ? x : y;
}


unsigned long int
CumulativeACK::sendCredit() const
{
	if (ccStrategy->getWindowSize() > sendingCompounds.size())
		return (min(ccStrategy->getWindowSize()-sendingCompounds.size(),advertisedWindowSize));
	else
		return 0;
}

void
CumulativeACK::updateTCPHeader(const wns::ldk::CompoundPtr& _compound)
{
    CumulativeACKCommand* cumACKCmd = dynamic_cast<CumulativeACKCommand*>(getCommand(_compound->getCommandPool()));

    assure(cumACKCmd!=NULL, "Not a Cumulative ACK Command!");
    assure(tcpHeaderReader!=NULL, "No reader for the TCP Header available!");

    TCPCommand* tcpHeader = tcpHeaderReader->readCommand<TCPCommand>(
        _compound->getCommandPool());

    assure(tcpHeader!=NULL, "No valid TCP Header found!");

    tcpHeader->peer.ack = cumACKCmd->peer.type == CumulativeACKCommand::I ? false : true;
}
