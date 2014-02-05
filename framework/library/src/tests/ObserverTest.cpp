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


#include <WNS/TestFixture.hpp>

#include <WNS/Subject.hpp>
#include <WNS/Observer.hpp>

#include <algorithm>

namespace wns { namespace tests {

	/**
	 * @brief Test for Subject and Observer
	 *
	 * @author Swen Kuehnlein
	 */
	class ObserverTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( ObserverTest );
		// CPPUNIT_TEST( attachNull );
		CPPUNIT_TEST( attachDetach );
		CPPUNIT_TEST( reattach );
		CPPUNIT_TEST( redetach );
		CPPUNIT_TEST( destroyObserver );
		CPPUNIT_TEST( destroySubject );
		CPPUNIT_TEST( attachDetachMultiple );
		CPPUNIT_TEST( notify );
		CPPUNIT_TEST( notifyWithArg );
		CPPUNIT_TEST( notifyMultiple );
		CPPUNIT_TEST( copyObserver );
		CPPUNIT_TEST( copySubject );
		CPPUNIT_TEST( assignmentObserver );
		CPPUNIT_TEST( assignmentSubject );
		CPPUNIT_TEST( stopObservingOnNotification );
		CPPUNIT_TEST( noUnsafeDetach );
        CPPUNIT_TEST( hasObservers );
		CPPUNIT_TEST_SUITE_END();

		class BarSubject;

		class FooNotificationInterface
		{
		public:
			virtual
			~FooNotificationInterface()
			{}

			virtual void
			fooNotify() = 0;
		};

		class BarNotificationInterface
		{
		public:
			virtual
			~BarNotificationInterface()
			{}

			virtual void
			barNotify(const BarSubject *const) = 0;
		};


		template <class T>
		class Collector
		{
		public:
			void
			operator()(T* instance)
			{
				instances.push_back(instance);
			}
			typedef std::list<T*> Container;
			Container instances;
		};

		class FooSubject :
			public Subject<FooNotificationInterface>
		{
			typedef Subject<FooNotificationInterface> FooNotificationSubject;
			using Subject<FooNotificationInterface>::ObserverType;

			typedef Collector<ObserverType> ObserverCollector;
		public:
			FooSubject() :
				Subject<FooNotificationInterface>()
			{}

			FooSubject(const FooSubject& other) :
				Subject<FooNotificationInterface>(other)
			{}

            bool
            hasObservers() const
            {
                return Subject<FooNotificationInterface>::hasObservers();
            }


			virtual void
			fooUpdated()
			{
				this->sendNotifies(&FooNotificationInterface::fooNotify);
			}

			std::size_t
			fooObserverCount()
			{
				return this->forEachObserver(ObserverCollector()).instances.size();
			}

			bool
			hasFooObserver(const ObserverType* observer)
			{
				ObserverCollector::Container observers = this->forEachObserver(ObserverCollector()).instances;
				return std::find(observers.begin(), observers.end(), observer) != observers.end();
			}

		};

		class LazyFooSubject :
			public FooSubject
		{
		public:
			virtual void
			fooUpdated()
			{
				this->forEachObserverNoDetachAllowed(std::mem_fun(&FooNotificationInterface::fooNotify));
			}
		};

		class BarSubject :
			public Subject<BarNotificationInterface>
		{
			typedef Subject<BarNotificationInterface> BarNotificationSubject;
			using Subject<BarNotificationInterface>::ObserverType;

			typedef Collector<ObserverType> ObserverCollector;
		public:
			void
			barUpdated()
			{
				this->sendNotifies(&BarNotificationInterface::barNotify, this);
			}

			std::size_t
			barObserverCount()
			{
				return this->forEachObserver(ObserverCollector()).instances.size();
			}

			bool
			hasBarObserver(const ObserverType* observer)
			{
				ObserverCollector::Container observers = this->forEachObserver(ObserverCollector()).instances;
				return std::find(observers.begin(), observers.end(), observer) != observers.end();
			}

		};

		class FooBarSubject :
			public FooSubject,
			public BarSubject
		{
		};

		class FooObserver :
			public Observer<FooNotificationInterface>
		{
			typedef Observer<FooNotificationInterface> FooNotificationObserver;
			using Observer<FooNotificationInterface>::SubjectType;

			typedef Collector<SubjectType> SubjectCollector;
		public:
			FooObserver() :
				fooNotifyCounter(0)
			{
			}

			virtual void
			fooNotify()
			{
				++fooNotifyCounter;
			}

			int fooNotifyCounter;
		};

		class FooObserverDetachOnNotify :
			public Observer<FooNotificationInterface>
		{
			typedef Observer<FooNotificationInterface> FooNotificationObserver;
			using Observer<FooNotificationInterface>::SubjectType;

		public:
			FooObserverDetachOnNotify() :
				subject(NULL)
			{
			}

			virtual void
			fooNotify()
			{
				this->stopObserving(subject);
			}

			virtual void
			setSubject(SubjectType* si)
			{
				assure(subject == NULL, "Can't set if not NULL");
				subject = si;
			}

			SubjectType* subject;
		};

		class BarObserver :
			public Observer<BarNotificationInterface>
		{
			typedef Observer<BarNotificationInterface> BarNotificationObserver;
			using Observer<BarNotificationInterface>::SubjectType;

			typedef Collector<SubjectType> SubjectCollector;
		public:
			BarObserver() :
				barNotifyCounter(0)
			{
			}

			virtual void
			barNotify(const BarSubject *const source)
			{
				++barNotifyCounter;
				lastNotifier = source;
			}

			int barNotifyCounter;
			const BarSubject* lastNotifier;
		};

		class FooBarObserver :
			public FooObserver,
			public BarObserver
		{
		};

	public:
		void
		prepare()
		{
		}

		void
		cleanup()
		{
		}

// 		void attachNull()
// 		{
// 			FooSubject subject;
// 			WNS_ASSERT_ASSURE_NOT_NULL_EXCEPTION(subject.addObserver(NULL));
// 		}

		void attachDetach()
		{
			const std::size_t count = 3;
			std::vector<FooSubject> subjects(count);
			std::vector<FooObserver> observers(count);

			// should be no observers there
			for (std::size_t i = 0; i < count; ++i) {
				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), subjects[i].fooObserverCount());
			}

			// attach observers
			for (std::size_t subjectIdx = 0; subjectIdx < count; ++subjectIdx)
				for (std::size_t observerIdx = 0; observerIdx < count; ++observerIdx) {
					observers[observerIdx].startObserving(&subjects[subjectIdx]);
					CPPUNIT_ASSERT_EQUAL(observerIdx + 1, subjects[subjectIdx].fooObserverCount());
				}

			// check, if all observers are there
			for (std::size_t subjectIdx = 0; subjectIdx < count; ++subjectIdx)
				for (std::size_t observerIdx = 0; observerIdx < count; ++observerIdx) {
					CPPUNIT_ASSERT(subjects[subjectIdx].hasFooObserver(&observers[observerIdx]));
				}

			// detach observers
			for (std::size_t subjectIdx = 0; subjectIdx < count; ++subjectIdx)
				for (std::size_t observerIdx = 0; observerIdx < count; ++observerIdx) {
					observers[observerIdx].stopObserving(&subjects[subjectIdx]);
					CPPUNIT_ASSERT_EQUAL(count - observerIdx - 1, subjects[subjectIdx].fooObserverCount());
					CPPUNIT_ASSERT(!subjects[subjectIdx].hasFooObserver(&observers[observerIdx]));
				}
		}

		void reattach()
		{
			FooSubject subject;
			FooObserver observer1, observer2;

			observer1.startObserving(&subject);
			observer2.startObserving(&subject);

			WNS_ASSERT_ASSURE_EXCEPTION(observer1.startObserving(&subject));
			WNS_ASSERT_ASSURE_EXCEPTION(observer2.startObserving(&subject));

			observer1.stopObserving(&subject);
			observer2.stopObserving(&subject);
		}

		void redetach()
		{
			FooSubject subject;
			FooObserver observer1, observer2;

			observer1.startObserving(&subject);
			observer2.startObserving(&subject);

			observer1.stopObserving(&subject);
			WNS_ASSERT_ASSURE_EXCEPTION(observer1.stopObserving(&subject));

			observer2.stopObserving(&subject);
			WNS_ASSERT_ASSURE_EXCEPTION(observer2.stopObserving(&subject));

		}

		void destroyObserver()
		{
			FooSubject subject;
			CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), subject.fooObserverCount());
			{
				FooObserver observer1;
				observer1.startObserving(&subject);
				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), subject.fooObserverCount());
				CPPUNIT_ASSERT(subject.hasFooObserver(&observer1));
				{
					FooObserver observer2;
					observer2.startObserving(&subject);
					CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), subject.fooObserverCount());
					CPPUNIT_ASSERT(subject.hasFooObserver(&observer2));
				}
				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), subject.fooObserverCount());
				CPPUNIT_ASSERT(subject.hasFooObserver(&observer1));
			}
			CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), subject.fooObserverCount());
		}

		void destroySubject()
		{
			// we cannot assert anything inside the observer,
			// so we just test, if the following runs
			FooSubject subject1;
			{
				FooObserver observer;
				{
					FooSubject subject2, subject3;
					observer.startObserving(&subject2);
					observer.startObserving(&subject1);
					observer.startObserving(&subject3);
				}
				// whoops, subject2 and subject3 are gone. If the observer
				// didn't notice, something should fail here, when he
				// tries to detach during his destruction.
			}
			CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), subject1.fooObserverCount());
		}

		void attachDetachMultiple()
		{
			const std::size_t count = 5;
			std::vector<FooSubject*> fooSubjects(count);
			std::vector<BarSubject*> barSubjects(count);
			std::vector<FooBarSubject*> fooBarSubjects(count);

			std::vector<FooObserver*> fooObservers(count);
			std::vector<BarObserver*> barObservers(count);
			std::vector<FooBarObserver*> fooBarObservers(count);

			for (std::size_t i = 0; i < count; ++i) {
				fooSubjects[i] = new FooSubject();
				barSubjects[i] = new BarSubject();
				fooBarSubjects[i] = new FooBarSubject();
				fooObservers[i] = new FooObserver();
				barObservers[i] = new BarObserver();
				fooBarObservers[i] = new FooBarObserver();
			}

			// attach observers
			for (std::size_t observerIdx = 0; observerIdx < count; ++observerIdx)
				for (std::size_t subjectIdx = 0; subjectIdx < count; ++subjectIdx) {
					fooObservers[observerIdx]->startObserving(fooSubjects[subjectIdx]);
					fooBarObservers[observerIdx]->FooObserver::startObserving(fooSubjects[subjectIdx]);
					barObservers[observerIdx]->startObserving(barSubjects[subjectIdx]);
					fooBarObservers[observerIdx]->BarObserver::startObserving(barSubjects[subjectIdx]);
					fooObservers[observerIdx]->startObserving(fooBarSubjects[subjectIdx]);
					barObservers[observerIdx]->startObserving(fooBarSubjects[subjectIdx]);
					fooBarObservers[observerIdx]->FooObserver::startObserving(fooBarSubjects[subjectIdx]);
					fooBarObservers[observerIdx]->BarObserver::startObserving(fooBarSubjects[subjectIdx]);
					CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(observerIdx + 1) * 2, fooSubjects[subjectIdx]->fooObserverCount());
					CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(observerIdx + 1) * 2, barSubjects[subjectIdx]->barObserverCount());
					CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(observerIdx + 1) * 2, fooBarSubjects[subjectIdx]->fooObserverCount());
					CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(observerIdx + 1) * 2, fooBarSubjects[subjectIdx]->barObserverCount());
				}

			// check if all observers are known
			for (std::size_t observerIdx = 0; observerIdx < count; ++observerIdx)
				for (std::size_t subjectIdx = 0; subjectIdx < count; ++subjectIdx) {
					CPPUNIT_ASSERT(fooSubjects[subjectIdx]->hasFooObserver(fooObservers[observerIdx]));
					CPPUNIT_ASSERT(fooSubjects[subjectIdx]->hasFooObserver(fooBarObservers[observerIdx]));
					CPPUNIT_ASSERT(barSubjects[subjectIdx]->hasBarObserver(barObservers[observerIdx]));
					CPPUNIT_ASSERT(barSubjects[subjectIdx]->hasBarObserver(fooBarObservers[observerIdx]));
					CPPUNIT_ASSERT(fooBarSubjects[subjectIdx]->hasFooObserver(fooObservers[observerIdx]));
					CPPUNIT_ASSERT(fooBarSubjects[subjectIdx]->hasBarObserver(barObservers[observerIdx]));
					CPPUNIT_ASSERT(fooBarSubjects[subjectIdx]->hasFooObserver(fooBarObservers[observerIdx]));
					CPPUNIT_ASSERT(fooBarSubjects[subjectIdx]->hasBarObserver(fooBarObservers[observerIdx]));
				}

			// detach 1st observer
			for (std::size_t subjectIdx = 0; subjectIdx < count; ++subjectIdx) {
				fooObservers[0]->stopObserving(fooSubjects[subjectIdx]);
				fooBarObservers[0]->FooObserver::stopObserving(fooSubjects[subjectIdx]);
				barObservers[0]->stopObserving(barSubjects[subjectIdx]);
				fooBarObservers[0]->BarObserver::stopObserving(barSubjects[subjectIdx]);
				fooObservers[0]->stopObserving(fooBarSubjects[subjectIdx]);
				barObservers[0]->stopObserving(fooBarSubjects[subjectIdx]);
				fooBarObservers[0]->FooObserver::stopObserving(fooBarSubjects[subjectIdx]);
				fooBarObservers[0]->BarObserver::stopObserving(fooBarSubjects[subjectIdx]);

				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(count - 1) * 2, fooSubjects[subjectIdx]->fooObserverCount());
				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(count - 1) * 2, barSubjects[subjectIdx]->barObserverCount());
				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(count - 1) * 2, fooBarSubjects[subjectIdx]->fooObserverCount());
				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(count - 1) * 2, fooBarSubjects[subjectIdx]->barObserverCount());
			}

			// destroy last and first of each
			delete fooSubjects[0];
			delete fooSubjects[count - 1];
			delete barSubjects[0];
			delete barSubjects[count - 1];
			delete fooBarSubjects[0];
			delete fooBarSubjects[count - 1];

			delete fooObservers[0];
			delete fooObservers[count - 1];
			delete barObservers[0];
			delete barObservers[count - 1];
			delete fooBarObservers[0];
			delete fooBarObservers[count - 1];

			for (std::size_t subjectIdx = 1; subjectIdx < count - 1; ++subjectIdx) {
				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(count - 2) * 2, fooSubjects[subjectIdx]->fooObserverCount());
				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(count - 2) * 2, barSubjects[subjectIdx]->barObserverCount());
				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(count - 2) * 2, fooBarSubjects[subjectIdx]->fooObserverCount());
				CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(count - 2) * 2, fooBarSubjects[subjectIdx]->barObserverCount());
			}

			for (std::size_t i = 1; i < count - 1; ++i) {
				delete fooSubjects[i];
				delete barSubjects[i];
				delete fooBarSubjects[i];
				delete fooObservers[i];
				delete barObservers[i];
				delete fooBarObservers[i];
			}
		}


		void notify()
		{
			FooSubject subject;
			FooObserver observer1;
			{
				FooObserver observer2;
				observer1.startObserving(&subject);
				observer2.startObserving(&subject);

				// should be still 0
				CPPUNIT_ASSERT_EQUAL(0, observer1.fooNotifyCounter);
				CPPUNIT_ASSERT_EQUAL(0, observer2.fooNotifyCounter);

				// notify both observers
				subject.fooUpdated();
				CPPUNIT_ASSERT_EQUAL(1, observer1.fooNotifyCounter);
				CPPUNIT_ASSERT_EQUAL(1, observer2.fooNotifyCounter);

				// notify only observer2
				observer1.stopObserving(&subject);
				subject.fooUpdated();
				CPPUNIT_ASSERT_EQUAL(1, observer1.fooNotifyCounter);
				CPPUNIT_ASSERT_EQUAL(2, observer2.fooNotifyCounter);
			}
			// no observer attached
			subject.fooUpdated();
			CPPUNIT_ASSERT_EQUAL(1, observer1.fooNotifyCounter);
		}

		void notifyWithArg()
		{
			BarSubject subject1;
			BarObserver observer1;
			{
				BarSubject subject2;
				BarObserver observer2;
				observer1.startObserving(&subject1);
				observer2.startObserving(&subject1);
				observer2.startObserving(&subject2);

				// should be still 0
				CPPUNIT_ASSERT_EQUAL(0, observer1.barNotifyCounter);
				CPPUNIT_ASSERT_EQUAL(0, observer2.barNotifyCounter);

				// notify both observers from subject1
				subject1.barUpdated();
				CPPUNIT_ASSERT_EQUAL(1, observer1.barNotifyCounter);
				CPPUNIT_ASSERT_EQUAL(1, observer2.barNotifyCounter);
				CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(&subject1), observer1.lastNotifier);
				CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(&subject1), observer2.lastNotifier);

				// notify observer2 from subject2
				subject2.barUpdated();
				CPPUNIT_ASSERT_EQUAL(1, observer1.barNotifyCounter);
				CPPUNIT_ASSERT_EQUAL(2, observer2.barNotifyCounter);
				CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(&subject1), observer1.lastNotifier);
				CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(&subject2), observer2.lastNotifier);
			}
			// no observer attached
			observer1.stopObserving(&subject1);
			subject1.barUpdated();
			CPPUNIT_ASSERT_EQUAL(1, observer1.barNotifyCounter);
			CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(&subject1), observer1.lastNotifier);
		}

		void notifyMultiple()
		{
			const std::size_t count = 5;
			std::vector<FooSubject*> fooSubjects(count);
			std::vector<BarSubject*> barSubjects(count);
			std::vector<FooBarSubject*> fooBarSubjects(count);

			std::vector<FooObserver*> fooObservers(count);
			std::vector<BarObserver*> barObservers(count);
			std::vector<FooBarObserver*> fooBarObservers(count);

			for (std::size_t i = 0; i < count; ++i) {
				fooSubjects[i] = new FooSubject();
				barSubjects[i] = new BarSubject();
				fooBarSubjects[i] = new FooBarSubject();
				fooObservers[i] = new FooObserver();
				barObservers[i] = new BarObserver();
				fooBarObservers[i] = new FooBarObserver();
			}

			// attach observers
			for (std::size_t observerIdx = 0; observerIdx < count; ++observerIdx)
				for (std::size_t subjectIdx = 0; subjectIdx < count; ++subjectIdx) {
					fooObservers[observerIdx]->startObserving(fooSubjects[subjectIdx]);
					fooBarObservers[observerIdx]->FooObserver::startObserving(fooSubjects[subjectIdx]);
					barObservers[observerIdx]->startObserving(barSubjects[subjectIdx]);
					fooBarObservers[observerIdx]->BarObserver::startObserving(barSubjects[subjectIdx]);
					fooObservers[observerIdx]->startObserving(fooBarSubjects[subjectIdx]);
					barObservers[observerIdx]->startObserving(fooBarSubjects[subjectIdx]);
					fooBarObservers[observerIdx]->FooObserver::startObserving(fooBarSubjects[subjectIdx]);
					fooBarObservers[observerIdx]->BarObserver::startObserving(fooBarSubjects[subjectIdx]);
				}

			// notifies
			for (std::size_t subjectIdx = 0; subjectIdx < count; ++subjectIdx) {
				for (std::size_t observerIdx = 0; observerIdx < count; ++observerIdx) {
					fooObservers[observerIdx]->fooNotifyCounter = 0;
					fooBarObservers[observerIdx]->fooNotifyCounter = 0;
					barObservers[observerIdx]->barNotifyCounter = 0;
					fooBarObservers[observerIdx]->barNotifyCounter = 0;
				}
				fooSubjects[subjectIdx]->fooUpdated();
				fooBarSubjects[subjectIdx]->fooUpdated();
				for (std::size_t observerIdx = 0; observerIdx < count; ++observerIdx) {
					CPPUNIT_ASSERT_EQUAL(2, fooObservers[observerIdx]->fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(2, fooBarObservers[observerIdx]->fooNotifyCounter);
				}
				barSubjects[subjectIdx]->barUpdated();
				for (std::size_t observerIdx = 0; observerIdx < count; ++observerIdx) {
					CPPUNIT_ASSERT_EQUAL(1, barObservers[observerIdx]->barNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(1, fooBarObservers[observerIdx]->barNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(barSubjects[subjectIdx]), barObservers[observerIdx]->lastNotifier);
					CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(barSubjects[subjectIdx]), fooBarObservers[observerIdx]->lastNotifier);
				}
				fooBarSubjects[subjectIdx]->barUpdated();
				for (std::size_t observerIdx = 0; observerIdx < count; ++observerIdx) {
					CPPUNIT_ASSERT_EQUAL(2, barObservers[observerIdx]->barNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(2, fooBarObservers[observerIdx]->barNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(fooBarSubjects[subjectIdx]), barObservers[observerIdx]->lastNotifier);
					CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(fooBarSubjects[subjectIdx]), fooBarObservers[observerIdx]->lastNotifier);
				}
			}

			// detach 1st observer
			for (std::size_t subjectIdx = 0; subjectIdx < count; ++subjectIdx) {

				fooObservers[0]->stopObserving(fooSubjects[subjectIdx]);
				fooBarObservers[0]->FooObserver::stopObserving(fooSubjects[subjectIdx]);
				barObservers[0]->stopObserving(barSubjects[subjectIdx]);
				fooBarObservers[0]->BarObserver::stopObserving(barSubjects[subjectIdx]);
				fooObservers[0]->stopObserving(fooBarSubjects[subjectIdx]);
				barObservers[0]->stopObserving(fooBarSubjects[subjectIdx]);
				fooBarObservers[0]->FooObserver::stopObserving(fooBarSubjects[subjectIdx]);
				fooBarObservers[0]->BarObserver::stopObserving(fooBarSubjects[subjectIdx]);
			}

			// destroy last and first of each
			delete fooSubjects[0];
			delete fooSubjects[count - 1];
			delete barSubjects[0];
			delete barSubjects[count - 1];
			delete fooBarSubjects[0];
			delete fooBarSubjects[count - 1];

			delete fooObservers[0];
			delete fooObservers[count - 1];
			delete barObservers[0];
			delete barObservers[count - 1];
			delete fooBarObservers[0];
			delete fooBarObservers[count - 1];

			// notify again
			for (std::size_t subjectIdx = 1; subjectIdx < count - 1; ++subjectIdx) {
				for (std::size_t observerIdx = 1; observerIdx < count - 1; ++observerIdx) {
					fooObservers[observerIdx]->fooNotifyCounter = 0;
					fooBarObservers[observerIdx]->fooNotifyCounter = 0;
					barObservers[observerIdx]->barNotifyCounter = 0;
					fooBarObservers[observerIdx]->barNotifyCounter = 0;
				}
				fooSubjects[subjectIdx]->fooUpdated();
				fooBarSubjects[subjectIdx]->fooUpdated();
				for (std::size_t observerIdx = 1; observerIdx < count - 1; ++observerIdx) {
					CPPUNIT_ASSERT_EQUAL(2, fooObservers[observerIdx]->fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(2, fooBarObservers[observerIdx]->fooNotifyCounter);
				}
				barSubjects[subjectIdx]->barUpdated();
				for (std::size_t observerIdx = 1; observerIdx < count - 1; ++observerIdx) {
					CPPUNIT_ASSERT_EQUAL(1, barObservers[observerIdx]->barNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(1, fooBarObservers[observerIdx]->barNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(barSubjects[subjectIdx]), barObservers[observerIdx]->lastNotifier);
					CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(barSubjects[subjectIdx]), fooBarObservers[observerIdx]->lastNotifier);
				}
				fooBarSubjects[subjectIdx]->barUpdated();
				for (std::size_t observerIdx = 1; observerIdx < count - 1; ++observerIdx) {
					CPPUNIT_ASSERT_EQUAL(2, barObservers[observerIdx]->barNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(2, fooBarObservers[observerIdx]->barNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(fooBarSubjects[subjectIdx]), barObservers[observerIdx]->lastNotifier);
					CPPUNIT_ASSERT_EQUAL(static_cast<const BarSubject*>(fooBarSubjects[subjectIdx]), fooBarObservers[observerIdx]->lastNotifier);
				}
			}

			for (std::size_t i = 1; i < count - 1; ++i) {
				delete fooSubjects[i];
				delete barSubjects[i];
				delete fooBarSubjects[i];
				delete fooObservers[i];
				delete barObservers[i];
				delete fooBarObservers[i];
			}
		}

		void
		copyObserver()
		{
			FooSubject subject;

			{
				FooObserver observer;
				observer.startObserving(&subject);

				{
					FooObserver observerCopy(observer);
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), subject.fooObserverCount());

					subject.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(1, observer.fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(1, observerCopy.fooNotifyCounter);
				}

				CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject.fooObserverCount());
				subject.fooUpdated();
				CPPUNIT_ASSERT_EQUAL(2, observer.fooNotifyCounter);
			}

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), subject.fooObserverCount());
		}

		void
		copySubject()
		{
			FooObserver observer;

			{
				FooSubject subject;
				observer.startObserving(&subject);

				{
					FooSubject subjectCopy(subject);
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject.fooObserverCount());
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subjectCopy.fooObserverCount());
					subject.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(1, observer.fooNotifyCounter);
					subjectCopy.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(2, observer.fooNotifyCounter);
				}
				CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject.fooObserverCount());
				subject.fooUpdated();
				CPPUNIT_ASSERT_EQUAL(3, observer.fooNotifyCounter);
			}
		}

		void
		assignmentObserver()
		{
			FooSubject subject;
			FooSubject subject2;

			{
				FooObserver observer;
				observer.startObserving(&subject);

				{
					FooObserver observer2;
                    observer2.startObserving(&subject2);
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject.fooObserverCount());
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject2.fooObserverCount());

					subject.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(1, observer.fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(0, observer2.fooNotifyCounter);

					subject2.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(1, observer.fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(1, observer2.fooNotifyCounter);

                    observer2 = observer;
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), subject.fooObserverCount());
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), subject2.fooObserverCount());
					subject2.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(1, observer.fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(1, observer.fooNotifyCounter);

					subject.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(2, observer.fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(2, observer.fooNotifyCounter);
				}

				CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject.fooObserverCount());
				subject.fooUpdated();
				CPPUNIT_ASSERT_EQUAL(3, observer.fooNotifyCounter);
			}

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), subject.fooObserverCount());
		}

		void
		assignmentSubject()
		{
			FooObserver observer;
			FooObserver observer2;

			{
				FooSubject subject;
				observer.startObserving(&subject);

				{
					FooSubject subject2;
                    observer2.startObserving(&subject2);
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject.fooObserverCount());
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject2.fooObserverCount());
					subject.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(1, observer.fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(0, observer2.fooNotifyCounter);
					subject2.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(1, observer.fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(1, observer2.fooNotifyCounter);

                    // remove own observers, assign other observers
                    subject2 = subject;
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject.fooObserverCount());
					CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject2.fooObserverCount());
					subject.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(2, observer.fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(1, observer2.fooNotifyCounter);
					subject2.fooUpdated();
					CPPUNIT_ASSERT_EQUAL(3, observer.fooNotifyCounter);
					CPPUNIT_ASSERT_EQUAL(1, observer2.fooNotifyCounter);
				}
				CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), subject.fooObserverCount());
				subject.fooUpdated();
				CPPUNIT_ASSERT_EQUAL(4, observer.fooNotifyCounter);
			}
		}

		void
		stopObservingOnNotification()
		{
			FooObserverDetachOnNotify observer;
			FooSubject subject;
			observer.startObserving(&subject);
			observer.setSubject(&subject);
			subject.fooUpdated();
		}

		void
		noUnsafeDetach()
		{
			FooObserverDetachOnNotify observer;
			LazyFooSubject subject;
			observer.startObserving(&subject);
			observer.setSubject(&subject);
			CPPUNIT_ASSERT_THROW(subject.fooUpdated(), wns::Exception);
		}

        void
        hasObservers()
        {
			FooObserver observer;
			FooObserver observer2;
			FooSubject subject;
            CPPUNIT_ASSERT( !subject.hasObservers() );
			observer.startObserving(&subject);
            CPPUNIT_ASSERT( subject.hasObservers() );
			observer2.startObserving(&subject);
            CPPUNIT_ASSERT( subject.hasObservers() );
			observer.stopObserving(&subject);
            CPPUNIT_ASSERT( subject.hasObservers() );
			observer2.stopObserving(&subject);
            CPPUNIT_ASSERT( !subject.hasObservers() );
        }
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( ObserverTest );
} // tests
} // wns
