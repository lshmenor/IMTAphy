/******************************************************************************
 * IPModule   Internet Protocol Implementation                                *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 ******************************************************************************/

#include <IP/iptables/filters/AcceptsAllFilter.hpp>
#include <IP/IPHeader.hpp>

#include <WNS/pyconfig/View.hpp>

using namespace ip::iptables::filters;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	AcceptsAllFilter,
	FilterInterface,
	"acceptsAllFilter",
	wns::PyConfigViewCreator);

AcceptsAllFilter::AcceptsAllFilter(const wns::pyconfig::View& _pyco):
	log(_pyco.get("logger"))
{
}

AcceptsAllFilter::AcceptsAllFilter()
{
}

bool
AcceptsAllFilter::fires(const IPCommand* ipHeader)
{
	assure(ipHeader, "There is no IPHeader set.");
	return true;
}

bool
AcceptsAllFilter::fires(const IPCommand* ipHeader, const wns::service::tl::IUDPHeader* udpHeader)
{
	assure(ipHeader, "There is no IPHeader set.");
	assure(udpHeader, "There is no UDPHeader set.");
	return true;
}

bool
AcceptsAllFilter::fires(const IPCommand* ipHeader, const wns::service::tl::ITCPHeader* tcpHeader)
{
	assure(ipHeader, "There is no IPHeader set.");
	assure(tcpHeader, "There is no TCPHeader set.");
	return true;
}
