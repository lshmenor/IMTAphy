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

#ifndef IP_IPTABLES_FILTERS_ACCEPTSALLFILTER_HPP
#define IP_IPTABLES_FILTERS_ACCEPTSALLFILTER_HPP

#include <IP/iptables/filters/FilterInterface.hpp>

namespace ip { namespace iptables { namespace filters {

	class AcceptsAllFilter:
		public FilterInterface
	{
	public:
		AcceptsAllFilter(const wns::pyconfig::View& _pyco);
		AcceptsAllFilter();

        virtual bool
        fires(const IPCommand*);

		virtual bool
        fires(const IPCommand*, const wns::service::tl::IUDPHeader*);

        virtual bool
        fires(const IPCommand*, const wns::service::tl::ITCPHeader*);

	private:
		wns::logger::Logger log;
	};
} // filters
} // iptables
} // ip
#endif //IP_IPTABLES_FILTERS_ACCEPTSALLFILTER_HPP
