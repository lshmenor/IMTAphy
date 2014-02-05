/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#ifndef WNS_SLIDINGWINDOW_HPP
#define WNS_SLIDINGWINDOW_HPP


#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/scheduler/Interface.hpp>

#include <WNS/Assure.hpp>

#include <list>

namespace wns {

	/**
	 * @brief provides a way to measure the sum of sample values recorded
	 * within a given window duration
	 *
	 * Usage is simple:
	 *
	 * - On a put() call the sample is added to the SlidingWindow object
	 * together with the current simulation time.
	 *
	 * - getAbsolute() will compute the sum of the sample values added
	 * (putted) in the interval (now-windowDuration, now].
	 *
	 * - getPerSecond() will compute getAbsolute()/windowDuration
	 *
	 * - reset will throw away all samples.
	 */
	class SlidingWindow
	{
		/**
		 * @brief A helper to keep values and time together
		 */
		struct Helper
		{
			Helper(simTimeType _time, double _value) :
				time(_time),
				value(_value)
			{
			}

			simTimeType time;
			double value;
		};

	public:
		/**
		 * @brief Constructor
		 *
		 * @param _windowSize The window size to be used
		 */
		explicit
		SlidingWindow(simTimeType _windowSize, bool _includeNow=false) :
			windowSize(_windowSize),
			includeNow(_includeNow),
			queue()
		{
			assure(this->windowSize > 0.0, "Window size must be > 0.0");
		}

		/**
		 * @brief Destructor
		 */
		virtual
		~SlidingWindow()
		{
			this->reset();
		}

		/**
		 * @brief sample is added together with the current simulation
		 * time
		 */
		virtual void
		put(double value)
		{
			this->removeOutdatedValues();
			simTimeType now = wns::simulator::getEventScheduler()->getTime();
			// add new value
			this->queue.push_back(Helper(now, value));
		}


		/**
		 * @brief compute getAbsolute()/windowDuration
		 */
		virtual double
		getPerSecond()
		{
			return this->getAbsolute() / this->windowSize;
		}


		/**
		 * @brief compute the sum of the sample values added (putted) in
		 * the interval [now-windowDuration, now)
		 *
		 * @note Samples recorded at NOW are not included in the evaluation
		 */
		virtual double
		getAbsolute()
		{
			this->removeOutdatedValues();
			double absolute = 0.0;
			simTimeType now = wns::simulator::getEventScheduler()->getTime();
			for(
				std::list<Helper>::const_iterator itr = this->queue.begin();
				itr != this->queue.end();
				++itr)
			{
				if ((itr->time < now) || includeNow)
				{
					absolute += itr->value;
				}
			}

			return absolute;
		}

		/**
		 * @brief Throw away all samples
		 */
		virtual void
		reset()
		{
			queue.clear();
		}
		
		/**
		 * @brief Return number of samples in the current window
		 *
		 * @note As in getAbsolute, the samples recorded at NOW are not included
		 */
		virtual int
		getNumSamples()
		{
			this->removeOutdatedValues();
			int numSamples = 0;
			simTimeType now = wns::simulator::getEventScheduler()->getTime();
			for(std::list<Helper>::const_iterator itr = this->queue.begin();
						 itr != this->queue.end();
						 ++itr)
			{
				if((itr->time < now) || includeNow)
				{
					++numSamples;
				}
			}
			return numSamples;
		}

	private:

		/**
		 * @brief Clean up queue
		 */
		void
		removeOutdatedValues()
		{
			simTimeType now = wns::simulator::getEventScheduler()->getTime();
			while(
				(this->queue.empty() == false) &&
				(this->queue.front().time < now - this->windowSize)
				)
			{
				this->queue.pop_front();
			}
		}


		simTimeType windowSize;
		
		bool includeNow;

		std::list<Helper> queue;
	};

} // wns

#endif // WNS_SLIDINGWINDOW_HPP


