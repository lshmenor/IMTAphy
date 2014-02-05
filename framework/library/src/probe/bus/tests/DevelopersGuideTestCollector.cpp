/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

// begin example "wns.probe.bus.tests.DevelopersGuideTestCollector.include.example"
#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/probe/bus/ProbeBusRegistry.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
// end example
#include <WNS/SmartPtr.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/probe/bus/tests/ProbeBusStub.hpp>
#include <WNS/TestFixture.hpp>
#include <WNS/osi/PDU.hpp>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

namespace wns { namespace probe { namespace bus { namespace tests { namespace developersGuideTestCollector {

// begin example "wns.probe.bus.tests.DevelopersGuideTestCollector.job.example"
class Job:
        public wns::osi::PDU
{
public:
    enum Priority
    {
        control = 0,
        realtime,
        multimedia,
        bestEffort
    } priority;

    Job();

    Job(const Job&);

    Priority priority_;

    wns::simulator::Time startedAt_;
};
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTestCollector.jobcp.example"
class PriorityProvider:
        public wns::probe::bus::PDUContextProvider<Job>
{
public:
    PriorityProvider(std::string key);

private:
    virtual void
    doVisit(wns::probe::bus::IContext& c, const wns::SmartPtr<Job>& job) const;

    std::string key_;
};
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTestCollector.processor.example"
class Processor
{
public:

    Processor();

    void
    startJob(const wns::SmartPtr<Job>& job);

    void
    onJobEnded(const wns::SmartPtr<Job>& job);

    int
    getID();

private:
    int
    generateID();

    wns::probe::bus::ContextProviderCollection
    getContextProviderCollection();

    boost::shared_ptr<wns::distribution::Distribution> dis_;

    wns::probe::bus::ContextCollectorPtr processingDelayCC_;

    int id_;
};
// end example

    class DevelopersGuideTestCollector : public wns::TestFixture  {
        CPPUNIT_TEST_SUITE( DevelopersGuideTestCollector );
        CPPUNIT_TEST( testNoSink );
        CPPUNIT_TEST( testSink );
        CPPUNIT_TEST_SUITE_END();

    public:
        void prepare();
        void cleanup();

        void testNoSink();
        void testSink();

    private:
        ProbeBus* thePassThroughProbeBus_;
    };

} // DeveloeprsGuideTest
} // tests
} // probe
} // bus
} // wns

using namespace wns::probe::bus::tests::developersGuideTestCollector;

CPPUNIT_TEST_SUITE_REGISTRATION( DevelopersGuideTestCollector );

Job::Job():
    PDU(),
    priority_(Job::bestEffort),
    startedAt_(0.0)
{
}


Job::Job(const Job& other):
    PDU()
{
    priority_ = other.priority_;
    startedAt_ = other.startedAt_;
}

PriorityProvider::PriorityProvider(std::string key):
    key_(key)
{
}

// begin example "wns.probe.bus.tests.DevelopersGuideTestCollector.jobcpDoVisit.example"
void
PriorityProvider::doVisit(wns::probe::bus::IContext& c,
                          const wns::SmartPtr<Job>& job) const
{
    c.insertInt(key_, job->priority_);
}
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTestCollector.constructor.example"
Processor::Processor():
    dis_(new wns::distribution::Uniform(0.0, 1.0)),
    id_(generateID())
{
    processingDelayCC_ = wns::probe::bus::ContextCollectorPtr(
        new wns::probe::bus::ContextCollector(
            this->getContextProviderCollection(),
            "processor.processingDelay"));
}
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTestCollector.getcpc.example"
wns::probe::bus::ContextProviderCollection
Processor::getContextProviderCollection()
{
    wns::probe::bus::ContextProviderCollection cpc;

    PriorityProvider jobPriorityProvider("priority");

    wns::probe::bus::contextprovider::Callback
        processorIDProvider("processorID",
                            boost::bind(&Processor::getID,this));

    cpc.addProvider(jobPriorityProvider);
    cpc.addProvider(processorIDProvider);

    return cpc;
}
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTestCollector.startJob.example"
void
Processor::startJob(const wns::SmartPtr<Job>& job)
{
    wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();

    job->startedAt_ = scheduler->getTime();

    wns::simulator::Time processingTime = (*dis_)();

    wns::events::scheduler::Callable jobEndsCallable =
        boost::bind(
            &Processor::onJobEnded,
            this,
            job);

    scheduler->scheduleDelay(jobEndsCallable, processingTime);
}
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTestCollector.stopJob.example"
void
Processor::onJobEnded(const wns::SmartPtr<Job>& job)
{
    // Create the measurementValue
    double measurementValue =
        wns::simulator::getEventScheduler()->getTime() - job->startedAt_;

    // Pass it all to the evaluation framework
    processingDelayCC_->put(job, measurementValue);
}
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTestCollector.generateID.example"
int
Processor::generateID()
{
    static int lastID = 0;

    lastID++;

    return lastID;
}
// end example

int
Processor::getID()
{
    return this->id_;
}

void
DevelopersGuideTestCollector::prepare()
{

}

void
DevelopersGuideTestCollector::cleanup()
{

}

void
DevelopersGuideTestCollector::testNoSink()
{
    Processor p;

    wns::SmartPtr<Job> j(new Job());

    p.startJob(j);

    wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();

    scheduler->processOneEvent();

    CPPUNIT_ASSERT(scheduler->getTime() < 1.0);

}

void
DevelopersGuideTestCollector::testSink()
{
    Processor p;

    wns::SmartPtr<Job> j(new Job());

    j->priority = Job::bestEffort;

    ProbeBusStub* pbStubReceives = new ProbeBusStub();
    pbStubReceives->setFilter("priority", Job::bestEffort);

    ProbeBusStub* pbStubDoesNotReceive = new ProbeBusStub();
    pbStubDoesNotReceive->setFilter("priority", Job::control);

    ProbeBusStub* pbStubReceivesP = new ProbeBusStub();
    pbStubReceivesP->setFilter("processorID", 2);

    ProbeBusStub* pbStubDoesNotReceiveP = new ProbeBusStub();
    pbStubDoesNotReceiveP->setFilter("processorID", 3);

    wns::probe::bus::ProbeBusRegistry* pbr = wns::simulator::getProbeBusRegistry();

    pbStubReceives->startObserving(pbr->getMeasurementSource("processor.processingDelay"));

    pbStubDoesNotReceive->startObserving(pbr->getMeasurementSource("processor.processingDelay"));

    pbStubReceivesP->startObserving(pbr->getMeasurementSource("processor.processingDelay"));

    pbStubDoesNotReceiveP->startObserving(pbr->getMeasurementSource("processor.processingDelay"));

    p.startJob(j);

    wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();

    scheduler->processOneEvent();

    CPPUNIT_ASSERT(scheduler->getTime() < 1.0);

    CPPUNIT_ASSERT(pbStubReceives->receivedCounter == 1);

    CPPUNIT_ASSERT(pbStubDoesNotReceive->receivedCounter == 0);

    CPPUNIT_ASSERT(pbStubReceivesP->receivedCounter == 1);

    CPPUNIT_ASSERT(pbStubDoesNotReceiveP->receivedCounter == 0);

    delete pbStubReceives;

    delete pbStubDoesNotReceive;
}
