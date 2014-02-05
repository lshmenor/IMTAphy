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

#ifndef IP_IPTABLES_IRULECONTROL_HPP
#define IP_IPTABLES_IRULECONTROL_HPP

#include <IP/iptables/Rule.hpp>

#include <list>

namespace ip {
	namespace iptables
	{
		class IRuleControl
		{
		public:
			virtual
			~IRuleControl() {}

			virtual	void
			addRule(ip::iptables::Rule) = 0;

			virtual	void
			removeRules(ip::iptables::Rule::RuleTag) = 0;

			virtual bool
			hasRules(ip::iptables::Rule::RuleTag) = 0;
		};


	}// iptables
}// ip

#endif // IP_IPTABLES_IRULECONTROL_HPP
