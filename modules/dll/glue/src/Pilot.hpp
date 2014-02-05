/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef GLUE_PILOT_HPP
#define GLUE_PILOT_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/events/PeriodicTimeout.hpp>

#include <list>

namespace glue {

    class Pilot :
	public wns::ldk::FunctionalUnit,
	public wns::ldk::CommandTypeSpecifier<wns::ldk::EmptyCommand>,
	public wns::ldk::HasReceptor<>,
	public wns::ldk::HasConnector<>,
	public wns::ldk::HasDeliverer<>,
	public wns::Cloneable<Pilot>,
	private wns::events::PeriodicTimeout
    {
    public:

	class Observer
	{
	public:
	    struct Information
	    {
	    };

	    virtual ~Observer() {};

	    virtual void
	    receivedBeacon(const Information& info) = 0;
	};

	Pilot(wns::ldk::fun::FUN* fun, wns::pyconfig::View& config);

	virtual ~Pilot();

	void
	enable();

	void
	disable();

	void
	attach(Observer* observer);

	void
	detach(Observer* observer);

	void
	sendBeacon();

    private:
	// FunctionalUnit / CompoundHandlerInterface
	virtual bool
	doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

	virtual void
	doSendData(const wns::ldk::CompoundPtr& compound);

	virtual void
	doWakeup();

	virtual void
	doOnData(const wns::ldk::CompoundPtr& compound);

	// PeriodicTimeout
	virtual void
	periodically();

	// Own functions
	Observer::Information
	createObserverInformation(const wns::ldk::CompoundPtr& compound);

	void
	notifyObservers(const Observer::Information& info);

	typedef std::list<Observer*> ObserverList;

	bool enabled;
	double timeout;
	ObserverList observers;
	bool delayedSend;
	wns::logger::Logger logger;
    };

}

#endif // GLUE_PILOT_HPP
