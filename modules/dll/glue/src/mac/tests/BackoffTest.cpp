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

#include <GLUE/mac/Backoff.hpp>

#include <WNS/events/MemberFunction.hpp>
#include <WNS/CppUnit.hpp>

namespace glue { namespace mac { namespace tests {

	class BackoffTest:
		public wns::TestFixture
	{

		class BackoffObserverMock :
			public virtual BackoffObserver
		{
		public:
			BackoffObserverMock() :
				cBackoffExpired(0)
			{
			}

			virtual void
			backoffExpired()
			{
				++cBackoffExpired;
			}

			int cBackoffExpired;
		};


		CPPUNIT_TEST_SUITE(BackoffTest);
		CPPUNIT_TEST ( onTXrequestDuringBusy );
		CPPUNIT_TEST ( onTXrequestDuringIdle );
		CPPUNIT_TEST ( onPostBOFinishedBusyInbeweenPacketAfter );
		CPPUNIT_TEST ( onPostBOFinishedBusyInbeweenPacketDuring );
		CPPUNIT_TEST ( onTXrequestDuringCollision );
		CPPUNIT_TEST_SUITE_END();


		/**
		 * @brief Fixes the backoff to 3 slots for testing purpose
		 */
		class BackoffFixed :
			public Backoff
		{
		public:
			BackoffFixed(
				BackoffObserver* _backoffObserver,
				simTimeType _sifsLength,
				simTimeType _slotLength,
				simTimeType _ackLength) :

				Backoff(
					_backoffObserver,
					_sifsLength,
					_slotLength,
					_ackLength,
					wns::logger::Logger("GLUE", "Backoff"))
			{}

			// fix counter to 3 to have deterministic tests
			virtual void
			fixBackoffCounter(int slots)
			{
				this->counter = slots;
			}
		};

		// members
		BackoffObserverMock* backoffObserver;
		BackoffFixed* backoffFixed;
		const simTimeType sifsLength;
		const simTimeType slotLength;
		const simTimeType ackLength;
		const simTimeType difsLength;
		const simTimeType eifsLength;

		// Used to send events to Backoff entitity
		typedef wns::events::MemberFunction<Backoff> BackoffEvent;

	public:
		BackoffTest() :
			wns::TestFixture(),
			backoffObserver(NULL),
			backoffFixed(NULL),
			sifsLength(15e-6),
			slotLength(10e-6),
			ackLength(44e-6),
			difsLength(sifsLength + 2.0*slotLength),
			eifsLength(sifsLength + slotLength + ackLength)
		{
		}

	private:
		virtual void
		prepare()
		{
			assure(this->backoffObserver == NULL, "not properly deleted");
			assure(this->backoffFixed == NULL, "not properly deleted");

			this->backoffObserver = new BackoffObserverMock();

			this->backoffFixed = new BackoffFixed(
				this->backoffObserver,
				this->sifsLength,
				this->slotLength,
				this->ackLength);
		}

		virtual void
		cleanup()
		{
			assure(this->backoffFixed != NULL, "not properly created");
			delete this->backoffFixed;
			this->backoffFixed = NULL;

			assure(this->backoffObserver != NULL, "not properly created");
			delete this->backoffObserver;
			this->backoffObserver = NULL;
		}

		void onTXrequestDuringBusy()
	    {
			int boSlots = 3;

			wns::events::scheduler::Interface* es = wns::simulator::getEventScheduler();
			// directly block the channel
			simTimeType nextEventAt = this->difsLength/10.0;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierBusy),
				nextEventAt);
			// run until channel blocked
			es->start();

			// send transmission request during channel busy
			this->backoffFixed->transmissionRequest(false);
			this->backoffFixed->fixBackoffCounter(boSlots);

			// free channel after some time
			nextEventAt += 20*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierIdle),
				nextEventAt);

			// run until all events are consumed
			es->start();

			// Expected backoff expired time
			simTimeType expectedBackoffExpired = nextEventAt + this->difsLength + boSlots*this->slotLength;

			WNS_ASSERT_MAX_REL_ERROR(
				expectedBackoffExpired,
				es->getTime(),
				1E-12 );

			CPPUNIT_ASSERT_EQUAL( 1, this->backoffObserver->cBackoffExpired );
		}

		void onTXrequestDuringIdle()
	    {
	  		int boSlots = 3;

			wns::events::scheduler::Interface* es = wns::simulator::getEventScheduler();

			// directly block the channel
			simTimeType nextEventAt = this->difsLength/10.0;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierBusy),
				nextEventAt);
			// free channel after some time
			nextEventAt += 20*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierIdle),
				nextEventAt);

            // How to wait for some time without doing nothing?
			// We just send another free channel
			// Let the post-backoff run out
			nextEventAt += this->difsLength + boSlots * this->slotLength;
			nextEventAt += this->difsLength*2;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierIdle),
				nextEventAt);

			// Do not forget to fix the number of slots
			this->backoffFixed->fixBackoffCounter(boSlots);
			// run until finished
			es->start();

			// send transmission request during idle
			this->backoffFixed->transmissionRequest(false);

			// Expected backoff expired time -> directly!
			simTimeType expectedBackoffExpired = nextEventAt;

			WNS_ASSERT_MAX_REL_ERROR(
				expectedBackoffExpired,
				es->getTime(),
				1E-12 );

			CPPUNIT_ASSERT_EQUAL( 1, this->backoffObserver->cBackoffExpired );
		}

		void onPostBOFinishedBusyInbeweenPacketAfter()
		{
			int boSlots = 3;
			wns::events::scheduler::Interface* es = wns::simulator::getEventScheduler();

			// directly block the channel
			simTimeType nextEventAt = this->difsLength/10.0;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierBusy),
				nextEventAt);
			// free channel after some time
			nextEventAt += 20*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierIdle),
				nextEventAt);
			// give some time to run the post-backoff and then block again
			nextEventAt += 20*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierBusy),
				nextEventAt);
			this->backoffFixed->fixBackoffCounter(boSlots);
			// run until finished
			es->start();

			// free channel after some time
			nextEventAt += 20*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierIdle),
				nextEventAt);
			// How to wait for some time without doing nothing?
			// We just send another free channel
			// Wait only half a DIFS
			nextEventAt += 0.5*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierIdle),
				nextEventAt);
			// run until finished
			es->start();

			// send transmission request after 0.5*DIFS
			this->backoffFixed->transmissionRequest(false);
            // Expected backoff expired time -> after complete DIFS finished
			simTimeType expectedBackoffExpired = nextEventAt + 0.5*this->difsLength;

			WNS_ASSERT_MAX_REL_ERROR(
				expectedBackoffExpired,
				es->getTime(),
				1E-12 );

			CPPUNIT_ASSERT_EQUAL( 1, this->backoffObserver->cBackoffExpired );
		}

		void onPostBOFinishedBusyInbeweenPacketDuring()
		{
			int boSlots = 3;
			wns::events::scheduler::Interface* es = wns::simulator::getEventScheduler();
			// directly block the channel
			simTimeType nextEventAt = this->difsLength/10.0;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierBusy),
				nextEventAt);
			// free channel after some time
			nextEventAt += 20*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierIdle),
				nextEventAt);
			// give some time to run the post-backoff and then block again
			nextEventAt += 20*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierBusy),
				nextEventAt);
			this->backoffFixed->fixBackoffCounter(boSlots);

			// How to wait for some time without doing nothing?
			// We just send another block channel
			nextEventAt += 10*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierBusy),
				nextEventAt);

            // run until finished
			es->start();

			// send transmission request during blocked channel
			this->backoffFixed->transmissionRequest(false);
			this->backoffFixed->fixBackoffCounter(boSlots);

			// free channel after some time
			nextEventAt += 10*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierIdle),
				nextEventAt);
			// run until finished
			es->start();

            // Expected backoff expired time -> after a complete backoff round
			simTimeType expectedBackoffExpired = nextEventAt + this->difsLength + boSlots*this->slotLength;

			WNS_ASSERT_MAX_REL_ERROR(
				expectedBackoffExpired,
				es->getTime(),
				1E-12 );

			CPPUNIT_ASSERT_EQUAL( 1, this->backoffObserver->cBackoffExpired );
		}

		void onTXrequestDuringCollision()
	    {
			int boSlots = 3;

			wns::events::scheduler::Interface* es = wns::simulator::getEventScheduler();
			// directly block the channel with a collision
			simTimeType nextEventAt = this->difsLength/10.0;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCollision),
				nextEventAt);
			// run until channel blocked
			es->start();

			// send transmission request during channel busy
			this->backoffFixed->transmissionRequest(false);
			this->backoffFixed->fixBackoffCounter(boSlots);

			// free channel after some time
			nextEventAt += 20*this->difsLength;
			es->schedule(
				BackoffEvent(this->backoffFixed, &Backoff::onCarrierIdle),
				nextEventAt);

			// run until all events are consumed
			es->start();

			// Expected backoff expired time
			simTimeType expectedBackoffExpired = nextEventAt + this->eifsLength + boSlots*this->slotLength;

			WNS_ASSERT_MAX_REL_ERROR(
				expectedBackoffExpired,
				es->getTime(),
				1E-12 );

			CPPUNIT_ASSERT_EQUAL( 1, this->backoffObserver->cBackoffExpired );
		}

	};

	CPPUNIT_TEST_SUITE_REGISTRATION( BackoffTest );

} // tests
} // mac
} // glue
