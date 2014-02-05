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

#include <IP/trace/TraceCollector.hpp>

using namespace ip::trace;

TraceCollector::TraceCollector(std::string _filename):
	filename(_filename),
	counter(0),
	writeHeader(true)
{
}

void
TraceCollector::addPacketTrace(PacketTrace pt)
{
	this->ptc.insert(ptc.end(), pt);
	++counter;
	if(counter > 1000)
	{
		counter = 0;
		this->write();
	}
}

void
TraceCollector::write()
{

	std::ofstream theFile;
	if (this->writeHeader)
	{
		theFile.open(this->filename.c_str(), std::ios::out | std::ios::binary);

		this->writeFileHeader(theFile);

	}
	else
	{
		theFile.open(this->filename.c_str(), std::ios::out | std::ios::binary | std::ios::app);
	}

	for (PacketTraceContainer::iterator it = ptc.begin();
		 it != ptc.end();
		 ++it)
	{
		this->writePacket(theFile, *it);
	}

	theFile.close();
	ptc.clear();
}

void
TraceCollector::writeFileHeader(std::ofstream& theFile)
{
	if (this->writeHeader)
	{
		pcap_hdr_t fileHeader;

		fileHeader.magic_number = 0xA1B2C3D4;
		fileHeader.version_major = 2;
		fileHeader.version_minor = 4;
		fileHeader.thiszone = 0;
		fileHeader.sigfigs = 0;
		fileHeader.snaplen = 0x0000FFFF;
		fileHeader.network = 1;

		theFile.write((char*) &(fileHeader), sizeof(fileHeader));
		this->writeHeader = false;
	}
}

bool
TraceCollector::hasSomethingToWrite() const
{
	return (!ptc.empty());
}

void
TraceCollector::writePacket(std::ofstream& theFile, const PacketTrace& pt)
{
	mac_hdr_t macHeader;
	ip_hdr_t ipHeader;
//	tcp_hdr_t tcpHeader;
	packet_hdr_t packetHeader;

	macHeader.destination[5] = pt.destinationMAC.getInteger() & 0xF;
	macHeader.destination[4] = (pt.destinationMAC.getInteger() >> 8) & 0xF;
	macHeader.destination[3] = (pt.destinationMAC.getInteger() >> 16) & 0xF;
	macHeader.destination[2] = (pt.destinationMAC.getInteger() >> 24) & 0xF;
	macHeader.destination[1] = 0;
	macHeader.destination[0] = 0;
	macHeader.source[5] = pt.sourceMAC.getInteger() & 0xF;
	macHeader.source[4] = (pt.sourceMAC.getInteger() >> 8) & 0xF;
	macHeader.source[3] = (pt.sourceMAC.getInteger() >> 16) & 0xF;
	macHeader.source[2] = (pt.sourceMAC.getInteger() >> 24) & 0xF;
	macHeader.source[1] = 0;
	macHeader.source[0] = 0;
	macHeader.type =0x0008; // IP Payload

	ipHeader.versionAndLength = 4 << 4 | 5;
	ipHeader.typeOfService = 0;
	ipHeader.totalLength = reverse16(sizeof(ipHeader) + pt.payloadSize/8);
	ipHeader.identification = 0;
	ipHeader.flagsAndOffset = 0;
	ipHeader.ttl = pt.TTL;
	ipHeader.protocol = pt.protocol;
	ipHeader.checksum = 0;
	ipHeader.sourceAddress = reverse32(pt.sourceIP.getInteger());
	ipHeader.destinationAddress = reverse32(pt.destinationIP.getInteger());

	ipHeader.checksum = reverse16(ipChecksum(ipHeader));

	time_t t;

	time(&t);

	tm* systime = localtime(&t);

	time_t simTimeTmp = static_cast<time_t>(floor(pt.now));
	tm* simTime = localtime(&simTimeTmp);

	systime->tm_sec = simTime->tm_sec;
	systime->tm_min = simTime->tm_min;
	systime->tm_hour = simTime->tm_hour;

	time_t unixtime = mktime(systime);

	packetHeader.timestamp = unixtime;
	packetHeader.microseconds = static_cast<unsigned long int>(floor((pt.now - floor(pt.now)) * 1000000));
	packetHeader.includedNumOctets = sizeof(macHeader) + sizeof(ipHeader) + pt.payloadSize/8;
	packetHeader.origNumOctets = sizeof(macHeader) + sizeof(ipHeader) + pt.payloadSize/8;

	theFile.write((char*) &(packetHeader), sizeof(packetHeader));
	theFile.write((char*) &(macHeader), sizeof(macHeader));
	theFile.write((char*) &(ipHeader), sizeof(ipHeader));

	char payload = 'W';
	for (int i=0; i < pt.payloadSize/8; ++i)
	{
		theFile.write((char*) &(payload), sizeof(payload));
		switch(payload)
		{
		case 'W':
			payload = 'N';
			break;
		case 'N':
			payload = 'S';
			break;
		case 'S':
			payload = 'W';
			break;
		default:
			payload = 'W';
		}
	}
}

uint16_t
TraceCollector::reverse16(const uint16_t orig)
{
	return (orig & 0x00FF) << 8 | (orig & 0xFF00) >> 8;
}

unsigned long int
TraceCollector::reverse32(const unsigned long int orig)
{
	return ((orig & 0x000000FF) << 24 | (orig & 0x0000FF00) << 8 |
			(orig & 0x00FF0000) >> 8  | (orig & 0xFF000000) >> 24);
}

uint16_t
TraceCollector::ipChecksum(ip_hdr_t ipHeader)
{
  unsigned char* buff=(unsigned char*)&ipHeader;
  uint16_t len=20;
  unsigned long int sum = 0;
  for (int i=0;i<len;i=i+2) sum += (unsigned long int)(((buff[i]<<8)&0xFF00) + (buff[i+1]&0xFF));
  while (sum>>16) sum = (sum & 0xFFFF) + (sum >> 16);
  sum = ~sum;

  return (uint16_t) sum;
}
