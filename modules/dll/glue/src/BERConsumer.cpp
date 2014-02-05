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

#include <GLUE/BERConsumer.hpp>
#include <GLUE/BERProvider.hpp>

#include <WNS/Assure.hpp>

using namespace glue;


BERConsumer::BERConsumer() :
	berProvider(NULL)
{}

BERConsumer::~BERConsumer()
{
	if (this->berProvider != NULL)
	{
		this->berProvider->detachBERConsumer(this);
	}
}

void
BERConsumer::setBERProvider(BERProvider* _berProvider)
{
	assure((this->berProvider == NULL && _berProvider) || (this->berProvider != NULL && !_berProvider),
	       "Either BER Provider is already set, or berProvider is already NULL");
	this->berProvider = _berProvider;
}


