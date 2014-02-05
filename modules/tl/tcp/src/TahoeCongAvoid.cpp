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

#include <TCP/TahoeCongAvoid.hpp>


using namespace tcp;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	TahoeCongAvoid,
	CongestionAvoidanceStrategy,
	"tcp.TahoeCongAvoid",
	wns::PyConfigViewCreator
	);


TahoeCongAvoid::TahoeCongAvoid(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	cwnd(pyco.get<unsigned long int>("congestionWindow")),
	cwnd_cnt(pyco.get<unsigned long int>("congestionWindowCounter")),
	logger(pyco.get("logger")),
	timeout(pyco.get<simTimeType>("retransmissionTimeout")),
	ndup(pyco.get<int>("numberDupACKs")),
	countDuplicateACKs()
{
	countDuplicateACKs.clear();
}

TahoeCongAvoid::~TahoeCongAvoid()
{
	countDuplicateACKs.clear();
}

void
TahoeCongAvoid::onSegmentLoss(segmentLoss reason, unsigned long int _ackNR)
{
	MESSAGE_SINGLE(NORMAL, logger, "onSegmentLoss called. Reason: " << printReason(reason));

	switch(reason)
	{
	case(TIMEOUT):
		countDuplicateACKs.clear();
		setWindowSize(1);

		break;

	case(DUPLICATE_ACK):
		if(!countDuplicateACKs.knows(_ackNR))
		{
			// increase if ndup duplicate acks occure
			// _in a row_
			countDuplicateACKs.clear();
			// 1st dup ack is 2nd ack of same nr in a row
			countDuplicateACKs.insert(_ackNR, 2);
		}
		else
		{
			int tmp = countDuplicateACKs.find(_ackNR);
			if(tmp < ndup)
			{
				tmp++;
				countDuplicateACKs.update(_ackNR, tmp);
			}
			else
			{
				unsigned long int curr_cwnd = getWindowSize();
				setWindowSize(curr_cwnd/2);
			}
		}

		break;

	default:
		break;
	}
}


void
TahoeCongAvoid::onRTTSample()
{
}

unsigned long int
TahoeCongAvoid::getWindowSize()
{
	return cwnd;
}

simTimeType
TahoeCongAvoid::getRetransmissionTimeout()
{
	MESSAGE_SINGLE(NORMAL, logger, "Retransmission timeout: " << timeout);
	return this->timeout;
}

void
TahoeCongAvoid::onSegmentAcknowledged()
{
	countDuplicateACKs.clear();

	if (cwnd_cnt >= cwnd)
	{
		cwnd++;
		cwnd_cnt = 0;
	}
	else
	{
		cwnd_cnt++;
	}
	MESSAGE_SINGLE(NORMAL, logger, "Segment acknowledged. Window size: " << cwnd);
}

void
TahoeCongAvoid::setWindowSize(unsigned long int new_cwnd)
{
	MESSAGE_SINGLE(NORMAL, logger, "Setting window size: " << new_cwnd << "(old value: " << cwnd << ")");
	cwnd = new_cwnd;
}


std::string
TahoeCongAvoid::printReason(segmentLoss reason)
{
	switch(reason)
	{
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


bool
TahoeCongAvoid::duplicateACKThresholdReached(unsigned long int _ackNR)
{
	if(countDuplicateACKs.knows(_ackNR))
		return countDuplicateACKs.find(_ackNR) == ndup;
	else
		return false;
}


void
TahoeCongAvoid::clearDuplicateACKCounter()
{
	countDuplicateACKs.clear();
}
