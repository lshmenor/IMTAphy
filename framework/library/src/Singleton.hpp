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

#ifndef WNS_SINGLETON_HPP
#define WNS_SINGLETON_HPP

#include <assert.h>
#include <cstdlib>
#include <WNS/NonCopyable.hpp>
#include <WNS/simulator/ISimulator.hpp>

namespace wns {

    struct AtSimulatorExit
    {
        static void
		scheduleDestruction(void (*functionPtr)())
		{
            wns::simulator::getShutdownSignal()->connect(functionPtr);
		}
    };

    struct AtApplicationExit
    {
        static void
		scheduleDestruction(void (*functionPtr)())
		{
            std::atexit(functionPtr);
		}
    };

	template <class T>
	struct DefaultCreation
	{
		static T*
		create()
		{
			return new T;
		}

		static void
		destroy(T* p)
		{
			delete p;
		}
	};

	template <class T>
	class SingletonHolderStaticData
	{
	public:
		static T* instance;
		static bool destroyed;
	};

	template<class T>
	T* SingletonHolderStaticData<T>::instance;

	template<class T>
	bool SingletonHolderStaticData<T>::destroyed;

	/**
	 * @brief Singleton holder to assure that only one instance of the
	 * desired type may exist.
	 */
	template<class T, 
        template <class> class CreationPolicy = DefaultCreation, 
        class DestructTimePolicy = AtSimulatorExit>
	class SingletonHolder :
		private NonCopyable
	{
		typedef SingletonHolderStaticData<T> Static;
	public:
		/**
		 * @brief Creates the singleton if necessary and returns a
		 * reference to it.
		 */
		static T&
		Instance()
		{
			if (Static::instance == NULL)
			{
				Static::instance = CreationPolicy<T>::create();
				Static::destroyed = false;

				DestructTimePolicy::scheduleDestruction(&destroySingleton);
			}

			assert(Static::destroyed == false); // using a destroyed singleton

			return *Static::instance;
		}

		/**
		 * @brief Old interface returning a pointer
		 */
		static T*
		getInstance()
		{
			return &Instance();
		}

		/**
		 * @brief Destroys the Singleton, normally only for testing purpose
		 */
		static void
		reset()
		{
			CreationPolicy<T>::destroy(Static::instance);
			Static::instance = CreationPolicy<T>::create();
		}

	private:
		/**
		 * @brief Destroys the singleton. Called at the end of the
		 * programm
		 */
		static void
		destroySingleton()
		{
			assert(Static::destroyed == false); // using a destroyed singleton
			CreationPolicy<T>::destroy(Static::instance);
			Static::instance = NULL;
			Static::destroyed = true;
		}

		/**
		 * @brief No instantiation allowed
		 */
		SingletonHolder();

		/**
		 * @brief No destruction allowed
		 */
		~SingletonHolder(); // forbidden
	};
}
#endif // NOT defined WNS_SINGLETON_HPP


