/******************************************************************************
 * IPTables Rule                                                              *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 ******************************************************************************/

#ifndef IP_IPTABLES_RULE_HPP
#define IP_IPTABLES_RULE_HPP

#include <IP/iptables/filters/FilterInterface.hpp>
#include <IP/iptables/targets/TargetInterface.hpp>

namespace ip { namespace iptables {

	/**
	 * @brief Implementation of IPTables Rule class.
	 * A rule consists of a filter, a target and a RuleTag as an identifier for
	 * the rule.
	 */
	class Rule
	{
	public:
		typedef int RuleTag;

         /**
		 * @brief Constructor
		 */
		Rule(filters::FilterInterface*, targets::TargetInterface*, RuleTag);

         /**
		 * @brief Destructor
		 */
		~Rule();


		/**
		 * @brief "getFilter()" returns the rules filter.
		 */
		filters::FilterInterface*
		getFilter();

		/**
		 * @brief "getTarget()" returns the rules target.
		 */
		targets::TargetInterface*
		getTarget();

		/**
		 * @brief "getRuleTag()" returns the rules rulesTag.
		 */
		ip::iptables::Rule::RuleTag
		getRuleTag();

	private:
		filters::FilterInterface* filter;
		targets::TargetInterface* target;
		ip::iptables::Rule::RuleTag ruleTag;
	};


}// iptables
}// ip

#endif // IP_IPTABLES_RULE_HPP
