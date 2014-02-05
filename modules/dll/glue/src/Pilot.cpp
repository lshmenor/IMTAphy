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

#include <GLUE/Pilot.hpp>

#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Exception.hpp>

using namespace glue;

Pilot::Pilot(wns::ldk::fun::FUN* fun, wns::pyconfig::View& config)
    : wns::ldk::CommandTypeSpecifier<wns::ldk::EmptyCommand>(fun),
      
      enabled(config.get<bool>("startEnabled")),
      timeout(config.get<double>("transmissionTimeout")),
      delayedSend(false),
      logger(config.get<wns::pyconfig::View>("logger"))
{
    if (enabled) enable();
} // Pilot

Pilot::~Pilot()
{
} // ~Pilot

void
Pilot::enable()
{
    enabled = true;
    startPeriodicTimeout(timeout);
} // enable

void
Pilot::disable()
{
    enabled = false;
    cancelPeriodicTimeout();
} // disable

void
Pilot::attach(Observer* observer)
{
    observers.push_back(observer);
} // attach

void
Pilot::detach(Observer* observer)
{
    observers.remove(observer);
} // detach

void
Pilot::sendBeacon()
{
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(getFUN()->getProxy()->createCommandPool(), wns::ldk::helper::FakePDUPtr()));

    activateCommand(compound->getCommandPool());

    if (getConnector()->hasAcceptor(compound)) {

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Sending beacon";
	if (delayedSend) m << " (delayed)";
	MESSAGE_END();

	getConnector()->getAcceptor(compound)->sendData(compound);
	delayedSend = false;

    } else {
	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Delaying sending of beacon";
	MESSAGE_END();
	delayedSend = true;
    }
} // sendBeacon

bool
Pilot::doIsAccepting(const wns::ldk::CompoundPtr&) const
{
    return false;
} // doIsAccepting

void
Pilot::doSendData(const wns::ldk::CompoundPtr&)
{
    throw wns::Exception("glue::Pilot does not send data.");
} // doSendData

void
Pilot::doWakeup()
{
    if (delayedSend) sendBeacon();
} // doWakeUp

void
Pilot::doOnData(const wns::ldk::CompoundPtr& compound)
{
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " Received beacon";
    MESSAGE_END();

    notifyObservers(createObserverInformation(compound));
} // doOnData

void
Pilot::periodically()
{
    sendBeacon();
} // periodically

Pilot::Observer::Information
Pilot::createObserverInformation(const wns::ldk::CompoundPtr& /* compound */)
{
    Observer::Information info;
    return info;
} // createObserverInformation

void
Pilot::notifyObservers(const Observer::Information& info)
{
    for (ObserverList::iterator anObserver = observers.begin(); anObserver != observers.end() ; ++anObserver)
	(*anObserver)->receivedBeacon(info);
} // notifyObservers

