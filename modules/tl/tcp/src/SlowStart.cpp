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

#include <TCP/SlowStart.hpp>


using namespace tcp;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	SlowStart,
	SlowStartStrategy,
	"tcp.SlowStart",
	wns::PyConfigViewCreator
	);


SlowStart::SlowStart(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	cwnd(pyco.get<int>("congestionWindow")),
	ssthresh(pyco.get<unsigned long int>("ssthresh")),
	logger(pyco.get("logger")),
	timeout(pyco.get<simTimeType>("retransmissionTimeout"))
{
}


SlowStart::~SlowStart()
{
}


void
SlowStart::onSegmentLoss(segmentLoss reason, unsigned long int /*_ackNR*/)
{
	MESSAGE_SINGLE(NORMAL, logger, "onSegmentLoss called. Reason: " << printReason(reason));

	switch(reason)
	{
	case TIMEOUT:
		reCalcThreshold();
		setWindowSize(1);
		break;

	case DUPLICATE_ACK:
		reCalcThreshold();
		setWindowSize(cwnd/2);
		break;

	default:
		break;
	}
}


void
SlowStart::onRTTSample()
{
}


unsigned long int
SlowStart::getWindowSize()
{
	return cwnd;
}


simTimeType
SlowStart::getRetransmissionTimeout()
{
	MESSAGE_SINGLE(NORMAL, logger, "Timeout for retransmission: " << timeout);
	return this->timeout;
}


void
SlowStart::onSegmentAcknowledged()
{
	cwnd++;
	MESSAGE_SINGLE(NORMAL, logger, "Segment acknowledged. Window size: " << cwnd);
}


void
SlowStart::setWindowSize(unsigned long int new_cwnd)
{
	MESSAGE_SINGLE(NORMAL, logger, "Setting window size: " << new_cwnd << "(old value: " << cwnd << ")");
	cwnd = new_cwnd;
}


bool
SlowStart::duplicateACKThresholdReached(unsigned long int /*_ackNR*/)
{
	// threshold for fast retransmit; implemented in tahoe congestion avoidance
	assure(false, "SlowStart::duplicateACKThresholdReached should never be called!");
	return false;
}


void
SlowStart::clearDuplicateACKCounter()
{// nothing to do
}


unsigned long int
SlowStart::getSlowStartThreshold()
{
	MESSAGE_SINGLE(NORMAL, logger, "SlowStart threshold: " << ssthresh);
	return this->ssthresh;
}


void
SlowStart::reCalcThreshold()
{
	unsigned long int prior_ssthresh __attribute__ ((unused)) = ssthresh;
	this->ssthresh = max(getWindowSize()>>1, 2);
	MESSAGE_SINGLE(NORMAL, logger, "Recalculate SlowStart threshold: " << ssthresh << "(old value: " << prior_ssthresh << ").");
}


std::string
SlowStart::printReason(segmentLoss reason)
{
	switch(reason){
	case TIMEOUT:
		return "TIMEOUT";
		break;
	case DUPLICATE_ACK:
		return "DUPLICATE_ACK";
		break;
	default:
		assure(false, "Unknown segment loss!");
		return "";
		break;
	}
}


unsigned long int
SlowStart::max(unsigned long int x, unsigned long int y)
{
	return (x<y) ? y : x;
}
