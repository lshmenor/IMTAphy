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

#include <DLL/services/management/InterferenceCache.hpp>
#include <DLL/Layer2.hpp>
#include <DLL/StationManager.hpp>

#include <WNS/node/Interface.hpp>
#include <WNS/service/dll/StationTypes.hpp>
#include <WNS/Average.hpp>

#include <cmath>


STATIC_FACTORY_REGISTER_WITH_CREATOR(
	dll::services::management::InterferenceCache,
	wns::ldk::ManagementServiceInterface,
	"dll.services.management.InterferenceCache",
	wns::ldk::MSRConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	dll::services::management::InterferenceCache::ConstantValue,
	dll::services::management::InterferenceCache::NotFoundStrategy,
	"dll.services.management.InterferenceCache.ConstantValue",
	wns::ldk::PyConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	dll::services::management::InterferenceCache::Complain,
	dll::services::management::InterferenceCache::NotFoundStrategy,
	"dll.services.management.InterferenceCache.Complain",
	wns::ldk::PyConfigCreator);

using namespace dll::services::management;

InterferenceCache::ConstantValue::ConstantValue(const wns::pyconfig::View& config)
{
	averageCarrier = config.get<wns::Power>("averageCarrier");
	averageInterference = config.get<wns::Power>("averageInterference");
	deviationCarrier = config.get<wns::Power>("deviationCarrier");
	deviationInterference = config.get<wns::Power>("deviationInterference");
	averagePathloss = config.get<wns::Ratio>("averagePathloss");
}


InterferenceCache::InterferenceCache( wns::ldk::ManagementServiceRegistry* msr, const wns::pyconfig::View& config )
	: wns::ldk::ManagementService( msr ),
	  alphaLocal_(config.get<double>("alphaLocal")),
	  alphaRemote_(config.get<double>("alphaRemote")),
      initialized_(false),
      serviceName_(config.get<std::string>("serviceName")),
	  logger(config.get("logger"))
{

	wns::pyconfig::View notFoundView(config, "notFoundStrategy");

	notFoundStrategy.reset
		(wns::StaticFactory< wns::ldk::PyConfigCreator<dll::services::management::InterferenceCache::NotFoundStrategy> >
		 ::creator(notFoundView.get<std::string>("__plugin__"))->create(notFoundView));

	wns::pyconfig::View esmFuncView(config, "esmFunc");
    esmFunc_ = IESMFuncFactory::creator(
        esmFuncView.get<std::string>("__plugin__"))->create(esmFuncView);


	MESSAGE_SINGLE(NORMAL, logger, "Created InterferenceCache Service");


}

void InterferenceCache::storeMeasurements( wns::node::Interface* node, 
    wns::service::phy::power::PowerMeasurementPtr rxPowerMeasurement, 
    ValueOrigin origin, int subBand)
{
    userSubbands_[node->getNodeID()].insert(subBand);

	wns::Power carrier = rxPowerMeasurement->getRxPower();
	wns::Power interference = rxPowerMeasurement->getOmniInterferencePower();
	wns::Ratio pathloss = rxPowerMeasurement->getLoss();
	InterferenceCacheKey key(node, subBand);
	double alpha;
	if (origin == Local)
		alpha = alphaLocal_;
	else
		alpha = alphaRemote_;

	Node2Power::iterator n2cait =
		node2CarrierAverage_.find( key );
	if ( n2cait == node2CarrierAverage_.end() )
		node2CarrierAverage_[key] = carrier;
	else
		node2CarrierAverage_[key] =
			node2CarrierAverage_[key] * (1.0 - alpha) + carrier * alpha;
	Node2Double::iterator n2caseit =
		node2CarrierSqExp_.find( key );
	if ( n2caseit == node2CarrierSqExp_.end() )
		node2CarrierSqExp_[key] = carrier.get_mW() * carrier.get_mW();
	else
		node2CarrierSqExp_[key] =
			(1.0 - alpha) * node2CarrierSqExp_[key] +  carrier.get_mW() * carrier.get_mW() * alpha;
	Node2Power::iterator n2iait =
		node2InterferenceAverage_.find( key );
	if ( n2iait == node2InterferenceAverage_.end() )
		node2InterferenceAverage_[key] = interference;
	else
		node2InterferenceAverage_[key] =
			 node2InterferenceAverage_[key] * (1.0 - alpha) + interference * alpha;

	Node2Double::iterator n2iaseit =
		node2InterferenceSqExp_.find( key );
	if ( n2iaseit == node2InterferenceSqExp_.end() )
		node2InterferenceSqExp_[key] = interference.get_mW() * interference.get_mW();
	else
		node2InterferenceSqExp_[key] =
			 node2InterferenceSqExp_[key] * (1.0 - alpha) + interference.get_mW() * interference.get_mW() * alpha ;
	Node2Double::iterator n2pit =
		node2pathloss.find( key );
	if ( n2pit == node2pathloss.end() )
		node2pathloss[key] = pathloss.get_factor();
	else
		node2pathloss[key] = (1.0 - alpha) * node2pathloss[key] + alpha * pathloss.get_factor();

	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing C for ");
	m << node->getName() << ": " << carrier << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing I for ");
	m << node->getName() << ": " << interference << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing Pathloss for ");
	m << node->getName() << ": " << pathloss << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
}

void InterferenceCache::storeCarrier( wns::node::Interface* node, const wns::Power& carrier, ValueOrigin origin, int subBand )
{
	InterferenceCacheKey key(node, subBand);

	double alpha;
	if (origin == Local)
		alpha = alphaLocal_;
	else
		alpha = alphaRemote_;

	Node2Power::iterator n2cait =
		node2CarrierAverage_.find( key );
	if ( n2cait == node2CarrierAverage_.end() )
		node2CarrierAverage_[key] = carrier;
	else
		node2CarrierAverage_[key] =
			node2CarrierAverage_[key] * (1.0 - alpha) + carrier * alpha;

	Node2Double::iterator n2caseit =
		node2CarrierSqExp_.find( key );
	if ( n2caseit == node2CarrierSqExp_.end() )
		node2CarrierSqExp_[key] = carrier.get_mW() * carrier.get_mW();
	else
		node2CarrierSqExp_[key] =
			(1.0 - alpha) * node2CarrierSqExp_[key] +  carrier.get_mW() * carrier.get_mW() * alpha;

	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing C for ");
	m << node->getName() << ": " << carrier << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
}

void InterferenceCache::storeInterference( wns::node::Interface* node, 
    const wns::Power& interference, ValueOrigin origin, int subBand, int timeSlot )
{
	InterferenceCacheKey key(node, subBand, timeSlot);

	double alpha;
	if (origin == Local)
		alpha = alphaLocal_;
	else
		alpha = alphaRemote_;

	Node2Power::iterator n2iait =
		node2InterferenceAverage_.find( key );
	if ( n2iait == node2InterferenceAverage_.end() )
		node2InterferenceAverage_[key] = interference;
	else
		node2InterferenceAverage_[key] =
			 node2InterferenceAverage_[key] * (1.0 - alpha) + interference * alpha;

	Node2Double::iterator n2iaseit =
		node2InterferenceSqExp_.find( key );
	if ( n2iaseit == node2InterferenceSqExp_.end() )
		node2InterferenceSqExp_[key] = interference.get_mW() * interference.get_mW();
	else
		node2InterferenceSqExp_[key] =
			 node2InterferenceSqExp_[key] * (1.0 - alpha) + interference.get_mW() * interference.get_mW() * alpha ;

	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing I for ");
	m << node->getName() << ": " << interference << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
}

void InterferenceCache::storePathloss( wns::node::Interface* node, const wns::Ratio& pathloss, ValueOrigin origin, int subBand )
{
	InterferenceCacheKey key(node, subBand);

	double alpha;
	if (origin == Local)
		alpha = alphaLocal_;
	else
		alpha = alphaRemote_;

	Node2Double::iterator n2pit =
		node2pathloss.find( key );
	if ( n2pit == node2pathloss.end() )
		node2pathloss[key] = pathloss.get_factor();
	else
		node2pathloss[key] = (1.0 - alpha) * node2pathloss[key] + alpha * pathloss.get_factor();

	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing Pathloss for ");
	m << node->getName() << ": " << pathloss << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
}

wns::Power InterferenceCache::getAveragedCarrier( wns::node::Interface* node, int subBand, int timeSlot ) const
{
    InterferenceCacheKey key(node, subBand, timeSlot);

	Node2Power::const_iterator it = node2CarrierAverage_.find( key );
	if ( it == node2CarrierAverage_.end() )
		return notFoundStrategy->notFoundAverageCarrier();
    MESSAGE_SINGLE(VERBOSE,logger,"Getting C for " << node->getName() <<": " << it->second );
    return it->second;
}

wns::Power InterferenceCache::getAveragedInterference( wns::node::Interface* node, int subBand, int timeSlot ) const
{
	InterferenceCacheKey key(node, subBand, timeSlot);

	Node2Power::const_iterator it = node2InterferenceAverage_.find( key );
	if ( it == node2InterferenceAverage_.end() )
		return notFoundStrategy->notFoundAverageInterference();
    MESSAGE_SINGLE(VERBOSE,logger,"Getting I for " << node->getName() <<": " << it->second );
	return it->second;
}

wns::Ratio InterferenceCache::getAveragedPathloss( wns::node::Interface* node, int subBand, int timeSlot ) const
{
	InterferenceCacheKey key(node, subBand, timeSlot);

	Node2Double::const_iterator itpl = node2pathloss.find(key);

	if ( itpl == node2pathloss.end() )
		return notFoundStrategy->notFoundAveragePathloss();

	return wns::Ratio::from_factor(itpl->second);
}

wns::Power InterferenceCache::getPerSCAveragedCarrier( wns::node::Interface* node) const
{
    if (userSubbands_.find(node->getNodeID()) == userSubbands_.end() || 
      userSubbands_.find(node->getNodeID())->second.empty())
    {
        MESSAGE_SINGLE(NORMAL,logger,"No average carrier known for " << node->getName());
        return notFoundStrategy->notFoundAverageCarrier();
    }

    std::set<int> subBands = userSubbands_.find(node->getNodeID())->second;
    std::set<int>::iterator it;
    wns::Average<double> mean;
    for(it = subBands.begin(); it != subBands.end(); it++)
    {
        InterferenceCacheKey key(node, *it);
        assure(node2CarrierAverage_.find(key) != node2CarrierAverage_.end(), "No average interference found.");
        mean.put(node2CarrierAverage_.find(key)->second.get_mW());
    }
    
    MESSAGE_SINGLE(NORMAL,logger,"Getting C for " << node->getName() <<": " << wns::Power::from_mW(mean.get())
         << " average over " << subBands.size() << " resources.");

	return wns::Power::from_mW(mean.get());
}

wns::Power InterferenceCache::getPerSCAveragedInterference( wns::node::Interface* node) const
{
    if (userSubbands_.find(node->getNodeID()) == userSubbands_.end() || 
      userSubbands_.find(node->getNodeID())->second.empty())
    {
        MESSAGE_SINGLE(NORMAL,logger,"No average interference known for " << node->getName());
        return notFoundStrategy->notFoundAverageInterference();
    }

    std::set<int> subBands = userSubbands_.find(node->getNodeID())->second;
    std::set<int>::iterator it;
    wns::Average<double> mean;
    for(it = subBands.begin(); it != subBands.end(); it++)
    {
        InterferenceCacheKey key(node, *it);
        assure(node2InterferenceAverage_.find(key) != node2InterferenceAverage_.end(), "No average interference found.");
        mean.put(node2InterferenceAverage_.find(key)->second.get_mW());
    }
    
    MESSAGE_SINGLE(NORMAL,logger,"Getting I for " << node->getName() <<": " << wns::Power::from_mW(mean.get())
         << " average over " << subBands.size() << " resources.");

    return wns::Power::from_mW(mean.get());

}

wns::Ratio InterferenceCache::getPerSCAveragedPathloss( wns::node::Interface* node) const
{
    if (userSubbands_.find(node->getNodeID()) == userSubbands_.end() || 
      userSubbands_.find(node->getNodeID())->second.empty())
    {
        MESSAGE_SINGLE(NORMAL,logger,"No average pathloss known for " << node->getName());
        return notFoundStrategy->notFoundAveragePathloss();
    }

    std::set<int> subBands = userSubbands_.find(node->getNodeID())->second;
    std::set<int>::iterator it;
    wns::Average<double> mean;
    for(it = subBands.begin(); it != subBands.end(); it++)
    {
        InterferenceCacheKey key(node, *it);
        assure(node2pathloss.find(key) != node2pathloss.end(), "No average pathloss found.");
        mean.put(node2pathloss.find(key)->second);
    }

    MESSAGE_SINGLE(NORMAL,logger,"Getting PL for " << node->getName() <<": " << wns::Ratio::from_factor(mean.get())<<" Mean: "<<mean.get()
         << " average over " << subBands.size() << " resources.");

    return wns::Ratio::from_factor(mean.get());

}

wns::Ratio 
InterferenceCache::getAverageEmittedInterferencePathloss(wns::node::Interface* node) const
{
    dll::Layer2* layer = getMSR()->getLayer<dll::Layer2*>();
    
    if(layer->getStationType() != wns::service::dll::StationTypes::AP())
        return wns::Ratio();

    if(initialized_ && remoteBSCaches_.size() == 0)
        return wns::Ratio();

    if(!initialized_)
    {
        initialized_ = true;
        dll::NodeList nl = layer->getStationManager()->getNodeList();

        dll::NodeList::iterator it;
        for(it = nl.begin(); it != nl.end(); it++)
        {
            /* Only include APs*/
            dll::ILayer2* aLayer = layer->getStationManager()->getStationByNode(*it);
            if(aLayer->getStationType() == wns::service::dll::StationTypes::AP())
            {
                /* Do not include us */
                if(*it != layer->getNode())
                {
                    remoteBSCaches_.push_back(
                        aLayer->getManagementService<dll::services::management::InterferenceCache>(serviceName_));
                }
            }
        }
    }

    std::list<InterferenceCache*>::iterator iter;
    double sum = 0;
    for(iter = remoteBSCaches_.begin(); iter != remoteBSCaches_.end(); iter++)
    {
        sum += 1.0 / (*iter)->getAveragedPathloss(node).get_factor();
    }
    wns::Ratio result = wns::Ratio::from_factor(1.0 / sum);

    return result;
}

void 
InterferenceCache::updateUserSubchannels (const wns::node::Interface* node, 
                                          const std::set<int>& channels)
{
  if (userSubbands_.find(node->getNodeID()) == userSubbands_.end())
  {
      return;
  }
  std::set<int>& set1 = userSubbands_[node->getNodeID()];
  std::set<int> set2;
  std::set_intersection(set1.begin(), set1.end(), channels.begin(), channels.end(), std::inserter(set2, set2.end()));
  set1.swap(set2);
}




wns::Ratio
InterferenceCache::getEffectiveSINR(wns::node::Interface* node, 
    const std::set<unsigned int>& subchannels,
    const wns::Power& txPower,
    const std::map<unsigned int, wns::Power>& interferences)
{
    std::set<unsigned int>::const_iterator it;
    std::list<wns::Ratio> sinrs;

    for(it = subchannels.begin(); it != subchannels.end(); it++)
    {
        wns::Power i;
        if(interferences.find(*it) != interferences.end())
            i = interferences.at(*it);
        else
            getAveragedInterference(node, *it);

        /*TODO We should use "wideband" PL estimation if 
        no per SC is available for some or all SCs*/
        wns::Ratio pl = getAveragedPathloss(node, *it);

        wns::Ratio sinr = txPower / (i * pl);
        sinrs.push_back(sinr);
    }

    std::list<wns::Ratio>::iterator iter;
    
    return (*esmFunc_)(sinrs); 
}                                    

void
InterferenceCache::onMSRCreated()
{
}

wns::Power InterferenceCache::getCarrierDeviation( wns::node::Interface* node, int subBand ) const
{
	InterferenceCacheKey key(node, subBand);

	Node2Double::const_iterator itsq = node2CarrierSqExp_.find( key );
	if ( itsq == node2CarrierSqExp_.end() )
		return notFoundStrategy->notFoundDeviationCarrier();

	Node2Power::const_iterator itp = node2CarrierAverage_.find( key );
	if ( itp == node2CarrierAverage_.end() )
		return notFoundStrategy->notFoundDeviationCarrier();

	return wns::Power::from_mW( sqrt( itsq->second -
		itp->second.get_mW() * itp->second.get_mW() ) );
}

wns::Power InterferenceCache::getInterferenceDeviation( wns::node::Interface* node, int subBand ) const
{
	InterferenceCacheKey key(node, subBand);

	Node2Double::const_iterator itsq = node2InterferenceSqExp_.find( key );
	if ( itsq == node2InterferenceSqExp_.end() )
		return notFoundStrategy->notFoundDeviationInterference();

	Node2Power::const_iterator itp = node2InterferenceAverage_.find( key );
	if ( itp == node2InterferenceAverage_.end() )
		return notFoundStrategy->notFoundDeviationInterference();

	return wns::Power::from_mW( sqrt( itsq->second -
		itp->second.get_mW() * itp->second.get_mW() ) );
}

