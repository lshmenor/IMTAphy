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

#include <IP/iptables/Rule.hpp>
#include <IP/iptables/filters/FilterInterface.hpp>
#include <IP/iptables/targets/TargetInterface.hpp>

using namespace ip::iptables;

Rule::Rule(filters::FilterInterface* _filter, targets::TargetInterface* _target, RuleTag _ruleTag):
	filter(_filter),
	target(_target),
	ruleTag(_ruleTag)
{
}

Rule::~Rule()
{
}

filters::FilterInterface*
Rule::getFilter()
{
	return filter;
}

targets::TargetInterface*
Rule::getTarget()
{
	return target;
}

ip::iptables::Rule::RuleTag
Rule::getRuleTag()
{
	return ruleTag;
}
