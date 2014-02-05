/******************************************************************************
 * DLLBase (DLL Base classes to create FUN-based DLL)                         *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2006                                                         *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <DLL/RANG.hpp>
#include <DLL/StationManager.hpp>
#include <DLL/Layer2.hpp>
#include <DLL/UpperConvergence.hpp>

#include <WNS/node/component/FQSN.hpp>
#include <WNS/module/Base.hpp>

#include <sstream>

using namespace dll;

STATIC_FACTORY_REGISTER_WITH_CREATOR(RANG,
                     wns::node::component::Interface,
                     "dll.RANG",
                     wns::node::component::ConfigCreator);

RANG::RANG(wns::node::Interface* node, const wns::pyconfig::View& _config) :
    wns::node::component::Component(node, _config),
    accessPointLookup(),
    config(_config),
    learnAPfromIncomingData((config.knows("learnAPfromIncomingData") ? config.get<bool>("learnAPfromIncomingData") : true)),
    logger(config.get("logger"))
{
} // RANG

void
RANG::doStartup()
{
    addService(config.get<std::string>("dataTransmission"), this);
    addService(config.get<std::string>("notification"), this);
}

void
RANG::onData(const wns::osi::PDUPtr& _data,
             wns::service::dll::FlowID)
{
    assure(dataHandlerRegistry.knows(wns::service::dll::protocolNumberOf(_data)), "no data handler set");
    dataHandlerRegistry.find(wns::service::dll::protocolNumberOf(_data))->onData(_data);
}

void
RANG::onData(const wns::osi::PDUPtr& _data,
             wns::service::dll::UnicastAddress _sourceMACAddress,
             wns::service::dll::UnicastDataTransmission* _ap,
             wns::service::dll::FlowID _dllFlowID)
{
    if(learnAPfromIncomingData)
    {
        updateAPLookUp(_sourceMACAddress, _ap);
    }

    MESSAGE_BEGIN(NORMAL, logger, m,"Receiving incoming data from MAC Address: ");
    m << _sourceMACAddress;
    MESSAGE_END();

    assure(dataHandlerRegistry.knows(wns::service::dll::protocolNumberOf(_data)), "no data handler set");
    dataHandlerRegistry.find(wns::service::dll::protocolNumberOf(_data))->onData(_data, _dllFlowID);
}

void
RANG::sendData(
    const wns::service::dll::UnicastAddress& _peer,
    const wns::osi::PDUPtr& pdu,
    wns::service::dll::protocolNumber protocol,
    wns::service::dll::FlowID _dllFlowID)
{
    wns::service::dll::UnicastDataTransmission* ap = NULL;
    if( knowsAddress(_peer) )
    {
        ap = getAccessPointFor(_peer);
    } else
    {
        MESSAGE_BEGIN(NORMAL, logger, m, "No valid access point found. Dropping packet to ");
        m << _peer;
        MESSAGE_END();
        return;
    }
    MESSAGE_BEGIN(NORMAL, logger, m, getName());
    m << ": doSendData(), RANG forwarding to convergence::Upper\n";
    m << "target is ";
    m << _peer;
    MESSAGE_END();
    ap->sendData(_peer, pdu, protocol, _dllFlowID);
}

void
RANG::registerHandler(wns::service::dll::protocolNumber protocol, wns::service::dll::Handler* dh)
{
    assure(dh, "no data handler set");
    assure(!dataHandlerRegistry.knows(protocol), "data handler already registered");
    dataHandlerRegistry.insert(protocol, dh);
}

void
RANG::onNodeCreated()
{
}


void
RANG::onWorldCreated()
{
    int numAPs = config.len("dllDataTransmissions");
    assure( numAPs == config.len("dllNotifications"),
            "mismatch in number of DataTransmission / Notification services");
    // browse through the list of connected APs
    for (int i = 0; i < numAPs; ++i)
    {
        wns::node::component::FQSN dataTransmission(config.get<wns::pyconfig::View>("dllDataTransmissions",i));
        wns::node::component::FQSN notification(config.get<wns::pyconfig::View>("dllNotifications",i));

        dll::UpperConvergence* dataTransmissionService =
            getRemoteService<dll::UpperConvergence*>(dataTransmission);
        dll::UpperConvergence* notificationService =
            getRemoteService<dll::UpperConvergence*>(notification);

        wns::service::dll::UnicastAddress macAdr = dataTransmissionService->getMACAddress();

        notificationService->registerHandler(wns::service::dll::TUNNEL, this);
        accessPointLookup.insert(macAdr, dataTransmissionService);

        MESSAGE_BEGIN(NORMAL, logger, m, "Added AP");
        m << i + 1 << " with MAC Adr.: " << macAdr << " to RANG!";
        MESSAGE_END();
    }
}

bool
RANG::knowsAddress(wns::service::dll::UnicastAddress _sourceMACAddress)
{
    return accessPointLookup.knows(_sourceMACAddress);
}

wns::service::dll::UnicastDataTransmission*
RANG::getAccessPointFor(wns::service::dll::UnicastAddress _sourceMACAddress)
{
    assure(knowsAddress(_sourceMACAddress), "No valid access point found.");
    return accessPointLookup.find(_sourceMACAddress);
}

// Modified Handler Interface for APs
void
RANG::updateAPLookUp( wns::service::dll::UnicastAddress _sourceMACAddress,
                      wns::service::dll::UnicastDataTransmission* _ap)
{
    // keep the MAC Address table up to date
    if (knowsAddress(_sourceMACAddress))
        accessPointLookup.erase(_sourceMACAddress);

    accessPointLookup.insert(_sourceMACAddress, _ap);

    MESSAGE_BEGIN(NORMAL, logger, m, "updated APLookup for UT with MAC Adr.: ");
    m << _sourceMACAddress << " (AP with MAC Adr.: " << dynamic_cast<dll::UpperConvergence*>(_ap)->getMACAddress() << ").";
    MESSAGE_END();
}

void
RANG::removeAddress(wns::service::dll::UnicastAddress _sourceMACAddress,
                    wns::service::dll::UnicastDataTransmission* _ap)
{
    if (knowsAddress(_sourceMACAddress))
    {
        if (getAccessPointFor(_sourceMACAddress) == _ap)
        {
            accessPointLookup.erase(_sourceMACAddress);

            MESSAGE_BEGIN(NORMAL, logger, m,"removed UT with MAC Adr.: ");
            m << _sourceMACAddress << " from APLookup.";
            MESSAGE_END();
        }
        else
        {
            throw wns::Exception("AP unrelated to UT tried to remove its routing entry!");
        }
    }
    else
        throw wns::Exception("Tried to remove unknown UT address from RANG APlookup table!");
}

void
RANG::onShutdown()
{
}

