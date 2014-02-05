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

#ifndef WNS_MODULE_BASE_HPP
#define WNS_MODULE_BASE_HPP


#include <WNS/module/MultiTypeFactory.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/logger/Logger.hpp>

#include <map>

namespace wns { namespace module {
	/**
	 * @brief The module::Module interface to communicate with WNS
	 */
	class Base :
		public MultiTypeFactory<std::string>
	{
	public:
		/**
		 * @brief Default constructor
		 */
		Base(const pyconfig::View& pyco);

		/**
		 * Destructor
		 */
		virtual
		~Base();

		/**
		 * @brief Overload in your module
		 */
		virtual void
		configure() =0;

		/**
		 * @brief Overload in your module
		 */
		virtual void
		shutDown() =0;

		/**
		 * @brief get version information
		 */
		//VersionInformation
		//getVersionInformation() const;

		/**
		 * @brief Ask if a service is registered
		 */
		static bool
		existsService(const std::string& s);

		/**
		 * @brief Retrieve a service
		 *
		 * To request a service call:
		 * @code wns::logger::MasterLogger* u = getService<wns::logger::MasterLogger>("W-NS-MSG"); @endcode
		 */
		template <typename T>
		static T
		getService(const std::string& s)
		{
			typename TypeMap::iterator itr =
				getGlobalMap().find(s);

			try {
				assure(itr!=getGlobalMap().end(),
				       "Service not registered");
			}
			catch (wns::Exception e)
			{
				std::stringstream ss;
				ss << "Service \"" << s
				   << "\" not registered!" << std::endl;
				ss << "Registered services are:" << std::endl;
				for (itr = getGlobalMap().begin();
				     itr != getGlobalMap().end();
				     ++itr) {
					ss << "  " << itr->first << std::endl;
				}
				throw(Exception(ss.str()));
			}

			return itr->second->get<T>(s);
		}

		bool
		getGlobalFlag() const;

		/**
		 * @brief Load a Module (shared library)
		 */
		static bool
		load(const std::string& name, bool absolutePath, bool beVerbose, bool lazyBinding);

	protected:
		/**
		 * @brief Function pointer to automatically convert return types
		 */
		typedef wns::ChamaeleonBase* (*ConvertFunction)(Base*);

		/**
		 * @brief Type for the local map
		 */
		typedef std::map<std::string, ConvertFunction> ConvertMap;

		/**
		 * @brief Add a service and the responsible module
		 */
		void
		addServiceToGlobalMap(const std::string& s);

		/**
		 * @brief Keeps the local mappings: service-to-function
		 */
		ConvertMap convertMap;

		/**
		 * @brief Version information
		 */
		//VersionInformation version;

		/**
		 * @brief Specifies if the module should be loaded globally
		 */
		bool global;

		/**
		 * @brief Logger of this module::Module
		 */
		logger::Logger logger;

	private:
		/**
		 * @brief Type for the global Service Map
		 */
		typedef std::map<std::string, Base*> TypeMap;

		/**
		 * @brief Use this to acces the service map
		 */
		static TypeMap& getGlobalMap();

		/**
		 * @brief called by MultTypeFactory to create the respecctive
		 * type
		 */
		ChamaeleonBase*
		create(const std::string& s);

		/**
		 * @brief Remove a service
		 */
		void
		removeServiceFromGlobalMap(const std::string& s);
	};

	/**
	 * @brief Special Creator for module::Modules (takes a pyconfig::View)
	 */
	typedef PyConfigViewCreator<Base> Creator;

	/**
	 * @brief Provide a StaticFactory for module::Modules
	 */
	typedef StaticFactory<Creator> Factory;
} // module
} // wns
#endif  // NOT defined WNS_MODULEBASE_HPP
