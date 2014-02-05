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

#ifndef WNS_LOGGER_LOGGER_HPP
#define WNS_LOGGER_LOGGER_HPP

#include <WNS/logger/Message.hpp>
#include <WNS/logger/Master.hpp>
#include <WNS/pyconfig/View.hpp>

/* The two macros defined below shall help using messages and disabling them
 * when compiling an optimized version, speak defining WNS_NO_LOGGING. Example on
 * how to use these macros (assume you've a Logger called "log" which should be
 * a member of your class where you're trying to log something):
 *
 * MESSAGE_BEGIN(NORMAL, log, specifyAnyVariableNameYouLike, "Foo");
 * specifyAnyVariableNameYouLike << "bar";
 * MESSAGE_END();
 *
 * Make sure that the code between MESSAGE_BEGIN and MESSAGE_END is only used
 * for creating the message, since it's not evaluated if you define WNS_NO_LOGGING.
 */
#ifndef WNS_NO_LOGGING

#define MESSAGE_BEGIN(L, LOGGER, M, STRINGSTREAM) { const wns::logger::Logger& neverUseThisAsLoggerName = (LOGGER); if((LOGGER).getMaster() != NULL && (LOGGER).getMaster()->isEnabled() && (LOGGER).getLevel() >= wns::logger::L) {\
wns::logger::Message M("", wns::logger::L);\
wns::logger::Message& _neverUseThis = M;\
if ((LOGGER).isShowFunction()==true) {_neverUseThis << __PRETTY_FUNCTION__ << "\n";}\
_neverUseThis << STRINGSTREAM;

#define MESSAGE_END()  neverUseThisAsLoggerName.send(_neverUseThis);}}

#define MESSAGE_SINGLE(L, LOGGER, STRINGSTREAM) { if((LOGGER).getMaster() != NULL && (LOGGER).getMaster()->isEnabled() && (LOGGER).getLevel() >= wns::logger::L) {\
wns::logger::Message _neverUseThis("", wns::logger::L);\
if ((LOGGER).isShowFunction()==true) {_neverUseThis << __PRETTY_FUNCTION__ << "\n";}\
_neverUseThis << STRINGSTREAM;\
(LOGGER).send(_neverUseThis);}}

#else

#define MESSAGE_BEGIN(L, LOGGER, M, STRINGSTREAM) {if(0) {wns::logger::Message M;

#define MESSAGE_END() }}

#define MESSAGE_SINGLE(L, LOGGER, STRINGSTREAM) {}

#endif

namespace wns { namespace logger {
	class Master;

	/**
	 * @brief ENUM for better readability of logger Levels
	 */
	enum Level { OFF=0, QUIET=1, NORMAL=2, VERBOSE=3, MAXLEVEL };

	/**
	 * @brief Writes Message to logger::Master
	 * @ingroup group_main_classes
	 *
	 * @ingroup logging
	 */
	class Logger {
		friend class LoggerTest;
	public:
		/**
		 * @brief Constructor specializing a Logger to a certain
		 * module::Module
		 *
		 * @param moduleName Name of the module::Module where the Logger
		 * is used
		 *
		 * @param loggerName Name of this Logger (most likely the name
		 * of the class where the Logger is used)
		 *
		 * @param master Pointer to the logger::Master where this Logger will
		 * write its Message to
		 */
		Logger(
			const std::string& moduleName,
			const std::string& loggerName,
			logger::Master* master);

		/**
		 * @brief Constructor specializing a Logger to a certain
		 * module::Module (attempts to find the logger::Master
		 * automatically)
		 *
		 * @param moduleName Name of the module::Module where the Logger
		 * is used
		 *
		 * @param loggerName Name of this Logger (most likely the name
		 * of the class where the Logger is used)
		 */
		Logger(
			const std::string& moduleName,
			const std::string& loggerName);

		/**
		 * @brief Constructor specializing a Logger to a certain
		 * module::Module
		 *
		 * @param pyConfigView to configure the Logger
		 * @param master Pointer to the logger::Master where this Logger will
		 * write its Message to
		 */
		Logger(
			const pyconfig::View& pyConfigView,
			logger::Master* master);

		/**
		 * @brief Tries to find the logger::Master on its own
		 *
		 * @param pyConfigView to configure the Logger
		 */
		explicit
		Logger(const pyconfig::View& pyConfigView);

		/**
		 * @brief Tries to find the logger::Master on its own but does
		 * no initialisation
		 *
		 */
		explicit
		Logger();

		/**
		 * @brief Destructor
		 */
		virtual
		~Logger();

		/**
		 * @brief Copy c'tor (class contains pointer)
		 */
		Logger(const Logger& other);

		/**
		 * @brief Assigment operator (class contains pointer)
		 */
		Logger&
		operator=(const Logger& other);

		/**
		 * @brief Configure the logger with the given configuration
		 */
		void
		configure(const wns::pyconfig::View& config);

		/**
		 * @brief Send a Message to MasterLogger
		 *
		 * @param m Message to be sent to the MasterLogger
		 *
		 * If the Logger is enabled the Message will be sent to the
		 * MasterLogger that has been set in the constuctor
		 */
		void
		send(const Message& m) const;

		/**
		 * @brief Enable the Logger
		 */
		void
		switchOn();

		/**
		 * @brief Disable the Logger
		 */
		void
		switchOff();

		/**
		 * @brief get the the current Logger level
		 */
		long int
		getLevel() const;

		/**
		 * @brief set the the current Logger level
		 */
		void
		setLevel(long int _level);

		/**
		 * @brief get the the current Module name (e.g. "CONST")
		 */
		const std::string&
		getModuleName() const;

		/**
		 * @brief get the the current Logger name (e.g. "UT4.Listener")
		 */
		const std::string&
		getLoggerName() const;

		/**
		 * @brief set the the current Logger name (e.g. "UT4.Listener")
		 */
		void
		setLoggerName(const std::string&);

		/**
		 * @brief Get the logger::Master
		 */
		Master*
		getMaster() const;

		/**
		 * @brief Show full signature of current function if true and
		 * used with macros
		 */
		bool
		isShowFunction() const;

	protected:
		/**
		 * @brief The name of the module::Module where the Logger is used
		 */
		std::string mName;

		/**
		 * @brief The name of the Logger (likely the class where it is used)
		 */
		std::string lName;

		/**
		 * @brief MasterLogger that receives the Message from this Logger
		 */
		logger::Master* masterLogger;

		/**
		 * @brief Flag to show if the Logger is enabled
		 */
		bool enabled;

		/**
		 * @brief Verbosity Level of the Logger
		 */
		long int level;

		/**
		 * @brief If enabled and the macro is used for printing shows
		 * the full signature of the current function as first line
		 */
		bool showFunction;
	}; // Logger
} // logger
} // wns
#endif // NOT defined WNS_LOGGER_LOGGER_HPP


