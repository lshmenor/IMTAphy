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

#include <IP/iptables/targets/DLLFlowIDTarget.hpp>
#include <IP/iptables/targets/DLLFlowIDTaggerTarget.hpp>
#include <IP/iptables/filters/AcceptsAllFilter.hpp>
#include <IP/iptables/PostroutingChain.hpp>

using namespace ip::iptables::targets;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
				     DLLFlowIDTarget,
				     TargetInterface,
				     "dllFlowIDTarget",
				     wns::PyConfigViewCreator);

DLLFlowIDTarget::DLLFlowIDTarget(const wns::pyconfig::View& _pyco):
  log(_pyco.get("logger"))
{
}

DLLFlowIDTarget::~DLLFlowIDTarget()
{
}

TargetResult
DLLFlowIDTarget::mangle(ip::IPCommand* ipHeader)
{
  MESSAGE_BEGIN(NORMAL, log, m, "");
  m << *ipHeader;
  MESSAGE_END();

  return CONT;
}

TargetResult
DLLFlowIDTarget::mangle(ip::IPCommand* ipHeader, wns::service::tl::ITCPHeader* tcpHeader)
{
  MESSAGE_BEGIN(NORMAL, log, m, "");
  m << "MANGLE Incoming Compound with DLLFlowID: " << ipHeader->local.dllFlowID;
  m << " for TCP-FlowID: " << tcpHeader->getFlowID();
  MESSAGE_END();

  if(!knowsFlowIDList.knows(ipHeader->local.dllFlowID))
    {
      MESSAGE_BEGIN(NORMAL, log, m, "");
      m << "Unknown DllFlowID : " << ipHeader->local.dllFlowID;
      m << " adding new Rule to PostroutingChain.";
      MESSAGE_END();
      knowsFlowIDList.insert(ipHeader->local.dllFlowID, tcpHeader->getFlowID());

      //add new rule to postroutingchain and add dll and tl flowids to
      // creating new target:
      ip::iptables::targets::DLLFlowIDTaggerTarget* target =
	new ip::iptables::targets::DLLFlowIDTaggerTarget(ipHeader->local.dllFlowID, tcpHeader->getFlowID());

      //creating AcceptsAllFilter:
      ip::iptables::filters::AcceptsAllFilter* filter = new ip::iptables::filters::AcceptsAllFilter();

      //creating with dllFlowID as ruleTag
      ip::iptables::Rule rule = ip::iptables::Rule(filter,
						   target,
						   ipHeader->local.dllFlowID);
      //adding Rule
      postroutingChain->addRule(rule);

      return CONT;
    }
  else
    {
      return CONT;
    }

  MESSAGE_BEGIN(NORMAL, log, m, "DLLFlowIDTarget::mangle()");
  m << *ipHeader;
  m << " TCP : " << tcpHeader->getFlowID();
  if (tcpHeader->getUrgentFlag())
    {
      m << " URG";
    }

  if (tcpHeader->getAckFlag())
    {
      m << " ACK";
    }
  if (tcpHeader->getPushFlag())
    {
      m << " PSH";
    }
  if (tcpHeader->getSynFlag())
    {
      m << " SYN";
    }
  if (tcpHeader->getFinFlag())
    {
      m << " FIN";
    }

  MESSAGE_END();

  return CONT;
}

TargetResult
DLLFlowIDTarget::mangle(ip::IPCommand* ipHeader, wns::service::tl::IUDPHeader* udpHeader)
{
  MESSAGE_BEGIN(NORMAL, log, m, "");
  m << "MANGLE Incoming Compound with DLLFlowID: " << ipHeader->local.dllFlowID;
  m << " for UDP-FlowID: " << udpHeader->getFlowID();
  MESSAGE_END();

  if(!knowsFlowIDList.knows(ipHeader->local.dllFlowID))
    {
      MESSAGE_BEGIN(NORMAL, log, m, "");
      m << "Unknown DllFlowID : " << ipHeader->local.dllFlowID;
      m << " adding new Rule to PostroutingChain.";
      MESSAGE_END();
      knowsFlowIDList.insert(ipHeader->local.dllFlowID, udpHeader->getFlowID());

      //add new rule to postroutingchain and add dll and tl flowids to
      // creating new target:
      ip::iptables::targets::DLLFlowIDTaggerTarget* target =
	new ip::iptables::targets::DLLFlowIDTaggerTarget(ipHeader->local.dllFlowID, udpHeader->getFlowID());

      //creating AcceptsAllFilter:
      ip::iptables::filters::AcceptsAllFilter* filter = new ip::iptables::filters::AcceptsAllFilter();

      //creating with dllFlowID as ruleTag
      ip::iptables::Rule rule = ip::iptables::Rule(filter,
						   target,
						   ipHeader->local.dllFlowID);
      //adding Rule
      postroutingChain->addRule(rule);

      return CONT;
    }
  else
    {
      return CONT;
    }

  MESSAGE_BEGIN(NORMAL, log, m, "DLLFlowIDTarget::mangle()");
  m << *ipHeader;
  m << " UDP : " << udpHeader->getFlowID();
  MESSAGE_END();
}

void
DLLFlowIDTarget::setRuleController(ip::iptables::IRuleControl* controller)
{
  postroutingChain = controller;
  assure(postroutingChain, "PostroutingChain not registered");

  ip::iptables::PostroutingChain* tmp = dynamic_cast<ip::iptables::PostroutingChain*>(postroutingChain);
  tmp->registerDLLFlowIDTarget(this);

  MESSAGE_BEGIN(NORMAL, log, m, "");
  m << "PostroutingChain set";
  MESSAGE_END();
}

bool
DLLFlowIDTarget::knowsTLFlowID(wns::service::tl::FlowID tlFlowID)
{
  bool foundTLFlowID = false;
  for (dllFlowIDList::const_iterator iter = knowsFlowIDList.begin();
       iter != knowsFlowIDList.end();
       ++iter)
    {
      if(iter->second == tlFlowID)
	{
	  foundTLFlowID = true;
	}
    }
  return foundTLFlowID;
}

wns::service::dll::FlowID
DLLFlowIDTarget::getDLLFlowID(wns::service::tl::FlowID tlFlowID)
{
  for (dllFlowIDList::const_iterator iter = knowsFlowIDList.begin();
       iter != knowsFlowIDList.end();
       ++iter)
    {
      if(iter->second == tlFlowID)
	{
	  return iter->first;
	}
    }
}

void
DLLFlowIDTarget::ruleErased(wns::service::dll::FlowID flowID)
{
  knowsFlowIDList.erase(flowID);
  MESSAGE_BEGIN(NORMAL, log, m, "FlowID erased from KnowsFlowIDList. FlowID: ");
  m << flowID;
  MESSAGE_END();
}
