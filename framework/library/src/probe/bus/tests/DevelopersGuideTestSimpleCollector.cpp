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

// begin example "wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.include.example"
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

namespace wns { namespace probe { namespace bus { namespace tests { namespace developersGuideTestSimpleCollector {

// begin example "wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.job.example"
class Job:
    virtual public wns::RefCountable
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

// begin example "wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.processor.example"
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

    boost::shared_ptr<wns::distribution::Distribution> dis_;

    wns::probe::bus::ContextCollector processingDelayCC_;

    int id_;
};
// end example

    class DevelopersGuideTestSimpleCollector : public wns::TestFixture  {
        CPPUNIT_TEST_SUITE( DevelopersGuideTestSimpleCollector );
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

using namespace wns::probe::bus::tests::developersGuideTestSimpleCollector;

CPPUNIT_TEST_SUITE_REGISTRATION( DevelopersGuideTestSimpleCollector );

Job::Job():
    priority_(Job::bestEffort),
    startedAt_(0.0)
{
}


Job::Job(const Job& other)
{
    priority_ = other.priority_;
    startedAt_ = other.startedAt_;
}

// begin example "wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.constructor.example"
Processor::Processor():
    dis_(new wns::distribution::Uniform(0.0, 1.0)),
    id_(generateID()),
    processingDelayCC_("processor.processingDelay")
{
}
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.startJob.example"
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

// begin example "wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.stopJob.example"
void
Processor::onJobEnded(const wns::SmartPtr<Job>& job)
{
    // Create the measurementValue
    double measurementValue =
        wns::simulator::getEventScheduler()->getTime() - job->startedAt_;

    // Pass it all to the evaluation framework
    processingDelayCC_.put(measurementValue,
                           boost::make_tuple("priority", job->priority_,
                                             "processorID", this->id_));
}
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTestSimpleCollector.generateID.example"
int
Processor::generateID()
{
    static int lastID = 0;

    lastID++;

    return lastID;
}
// end example

void
DevelopersGuideTestSimpleCollector::prepare()
{

}

void
DevelopersGuideTestSimpleCollector::cleanup()
{

}

void
DevelopersGuideTestSimpleCollector::testNoSink()
{
    Processor p;

    wns::SmartPtr<Job> j(new Job());

    p.startJob(j);

    wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();

    scheduler->processOneEvent();

    CPPUNIT_ASSERT(scheduler->getTime() < 1.0);

}

void
DevelopersGuideTestSimpleCollector::testSink()
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
