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

#ifndef WNS_LOGGER_MASTER_HPP
#define WNS_LOGGER_MASTER_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/logger/OutputStrategy.hpp>
#include <WNS/logger/FormatStrategy.hpp>

#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/scheduler/Interface.hpp>

#include <string>
#include <map>
#include <deque>

namespace wns { namespace logger {
	/**
	 * @brief Base class for logging in WNS
	 *
	 * @ingroup logging
	 *
	 * WNS will instantiate one logger::Master and offer
	 * this as service ("W-NS-MSG"). The Logger in the module will write to
	 * this logger::Master.
	 *
	 * \pyco{openwns.Logger.MasterLogger}
	 */
	class Master
	{
		typedef std::map<OutputStrategy*, FormatStrategy*> LoggerChain;
	public:
		/**
		 * @brief Create MasterLogger with empty logger chain
		 *
		 * The logger chain and other configuration parameters can be
		 * set afterwards using:
		 * MasterLogger::configure(const pyconfig::View& pyco)
		 */
		explicit
		Master();

		/**
		 * @brief Default constructor
		 *
		 * This is essentially the same as calling:
		 * @code
		 * wns::logger::Master m;
		 * m.configure(pycoView);
		 * @endcode
		 * @note Logger is switched on by default
		 */
		explicit
		Master(const pyconfig::View& pyco);

		/**
		 * @brief Destructor
		 */
		virtual
		~Master();

		void
		configure(const pyconfig::View& pyco);

		/**
		 * @brief The Logger calls this method when it starts up.
		 *
		 */
		void
		registerLogger(const std::string& aModuleRef,
			       const std::string& aLocationRef);

		/**
		 * @brief The Logger calls this method to write Messages
		 *
		 * @todo Another method that takes a Message would definetly
		 * make sense :)
		 */
		void
		write(const std::string& aModuleRef,
		      const std::string& aLocationRef,
		      const std::string& aMsgRef)
		{
			if (doLogging)
			{
				// If we have loggin enabled we have to process
				// the message
				RawMessage m;
				/**
				 * @todo libwns can have an own EventScheduler
				 * which can forward stuff to the EventScheduler
				 */
				m.time = wns::simulator::getEventScheduler()->getTime();
				m.module = aModuleRef;
				m.location = aLocationRef;
				m.message = aMsgRef;

				if (haveBacktrace)
				{
					// if we have logging enabled, and the
					// backtracing is enabled we will not
					// write the message, but rather keep it
					// in the backtrace buffer
					saveForBacktrace(m);
				}
				else
				{
					// if we have logging enabled and
					// backtrace disable we are going to
					// write the message
					outputMessage(m);
				}
			}
			// If we have don't have debugging and backtrace
			// enabled we will do nothing
		};

		/**
		 * @brief If no backtrace is configured, will output nothing.
		 */
		void outputBacktrace() const;

		bool isEnabled() const;

	private:
		bool doLogging;
		bool haveBacktrace;
		size_t numberOfLinesForBacktrace;
		std::deque<RawMessage> backtrace;

		LoggerChain loggerChain;

		void saveForBacktrace(const RawMessage& m);
		void outputMessage(const RawMessage& m) const;
		void clearLoggerChain();
	}; // Master

} // logger
} // wns
#endif // not defined WNS_LOGGER_MASTER_HPP


