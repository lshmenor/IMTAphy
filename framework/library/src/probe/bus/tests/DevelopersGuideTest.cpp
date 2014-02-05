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

// begin example "wns.probe.bus.tests.DevelopersGuideTest.include.example"
#include <WNS/probe/bus/ProbeBus.hpp>
#include <WNS/probe/bus/ProbeBusRegistry.hpp>
// end example
#include <WNS/SmartPtr.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/probe/bus/tests/ProbeBusStub.hpp>
#include <WNS/TestFixture.hpp>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

namespace wns { namespace probe { namespace bus { namespace tests { namespace developersGuideTest {

// begin example "wns.probe.bus.tests.DevelopersGuideTest.job.example"
class Job
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

// begin example "wns.probe.bus.tests.DevelopersGuideTest.processor.example"
class Processor
{
public:

    Processor();

    void
    startJob(Job job);

    void
    onJobEnded(Job job);

private:
    boost::shared_ptr<wns::distribution::Distribution> dis_;

    wns::probe::bus::ProbeBus* probeBus_;
};
// end example

    class DevelopersGuideTest : public wns::TestFixture  {
        CPPUNIT_TEST_SUITE( DevelopersGuideTest );
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

using namespace wns::probe::bus::tests::developersGuideTest;

CPPUNIT_TEST_SUITE_REGISTRATION( DevelopersGuideTest );

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

// begin example "wns.probe.bus.tests.DevelopersGuideTest.constructor.example"
Processor::Processor():
    dis_(new wns::distribution::Uniform(0.0, 1.0)),
    probeBus_(NULL)
{
    wns::probe::bus::ProbeBusRegistry* reg = wns::simulator::getProbeBusRegistry();
    probeBus_ = reg->getMeasurementSource("processor.processingDelay");

    assure(probeBus_ != NULL, "The probeBus_ is NULL");
}
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTest.startJob.example"
void
Processor::startJob(Job job)
{
    wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();

    job.startedAt_ = scheduler->getTime();

    wns::simulator::Time processingTime = (*dis_)();

    wns::events::scheduler::Callable jobEndsCallable =
        boost::bind(
            &Processor::onJobEnded,
            this,
            job);

    scheduler->scheduleDelay(jobEndsCallable, processingTime);
}
// end example

// begin example "wns.probe.bus.tests.DevelopersGuideTest.stopJob.example"
void
Processor::onJobEnded(Job job)
{
    // Create the context and populate it
    wns::probe::bus::Context context;

    context.insertInt("priority", job.priority_);

    // Get the current time
    wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();

    wns::simulator::Time timestamp = scheduler->getTime();

    // Create the measurementValue
    double measurementValue = timestamp - job.startedAt_;

    assure(probeBus_ != NULL, "The probeBus_ is NULL");

    // Pass it all to the evaluation framework
    probeBus_->forwardMeasurement(timestamp,
                                  measurementValue,
                                  context);
}
// end example

void
DevelopersGuideTest::prepare()
{

}

void
DevelopersGuideTest::cleanup()
{

}

void
DevelopersGuideTest::testNoSink()
{
    Processor p;

    Job j;

    p.startJob(j);

    wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();

    scheduler->processOneEvent();

    CPPUNIT_ASSERT(scheduler->getTime() < 1.0);
}

void
DevelopersGuideTest::testSink()
{
    Processor p;

    Job j;

    j.priority = Job::bestEffort;

    ProbeBusStub* pbStubReceives = new ProbeBusStub();
    pbStubReceives->setFilter("priority", Job::bestEffort);

    ProbeBusStub* pbStubDoesNotReceive = new ProbeBusStub();
    pbStubDoesNotReceive->setFilter("priority", Job::control);

    wns::probe::bus::ProbeBusRegistry* pbr = wns::simulator::getProbeBusRegistry();

    pbStubReceives->startObserving(pbr->getMeasurementSource("processor.processingDelay"));

    pbStubDoesNotReceive->startObserving(pbr->getMeasurementSource("processor.processingDelay"));

    p.startJob(j);

    wns::events::scheduler::Interface* scheduler = wns::simulator::getEventScheduler();

    scheduler->processOneEvent();

    CPPUNIT_ASSERT(scheduler->getTime() < 1.0);

    CPPUNIT_ASSERT(pbStubReceives->receivedCounter == 1);

    CPPUNIT_ASSERT(pbStubDoesNotReceive->receivedCounter == 0);

    delete pbStubReceives;

    delete pbStubDoesNotReceive;
}
