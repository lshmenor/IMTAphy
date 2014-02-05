/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/BERProvider.hpp>
#include <GLUE/BERConsumer.hpp>

#include <WNS/Assure.hpp>

#include <iostream>
#include <algorithm>

using namespace glue;

BERProvider::BERProvider() :
	berConsumers()
{
} // BERProvider


BERProvider::~BERProvider()
{
	while(!this->berConsumers.empty())
	{
		BERConsumer* bc = *(this->berConsumers.begin());
		bc->onBERProviderDeleted();
		this->detachBERConsumer(bc);
	}
} // ~BERProvider


void
BERProvider::attachBERConsumer(BERConsumer* berConsumer)
{
	assure(std::find(berConsumers.begin(),
			 berConsumers.end(),
			 berConsumer)
	       == berConsumers.end(),
	       "BERConsumer is already added to PERProvider");

	berConsumer->setBERProvider(this);
	this->berConsumers.push_back(berConsumer);
} // attachBERConsumer


void
BERProvider::detachBERConsumer(BERConsumer* berConsumer)
{
	assure(std::find(berConsumers.begin(),
			 berConsumers.end(),
			 berConsumer)
	       != berConsumers.end(),
	       "unknown BERConsumer");

	berConsumer->setBERProvider(NULL);
	this->berConsumers.remove(berConsumer);
} // detachBERConsumer


void
BERProvider::notifyBERConsumers(double BER, int packetSize) const
{
	BERConsumerContainer::const_iterator itEnd = this->berConsumers.end();
	for (BERConsumerContainer::const_iterator it = this->berConsumers.begin();
	     it != itEnd;
	     ++it)
	{
		(*it)->onNewMeasurement(BER, packetSize);
	}
} // notifyBERConsumers


const BERProvider::BERConsumerContainer&
BERProvider::getBERConsumersAttached() const
{
	return this->berConsumers;
} // getBERConsumersAttached



