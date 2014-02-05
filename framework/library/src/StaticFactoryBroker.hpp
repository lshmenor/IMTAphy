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

#ifndef WNS_STATICFACTORYBROKER_HPP
#define WNS_STATICFACTORYBROKER_HPP

#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/container/Registry.hpp>
#include <WNS/Assure.hpp>

#include <utility>
#include <string>

namespace wns {

    /**
     * @brief Able to create ELEMENTs from pyconfig::View using StaticFactory.
     *
     * If an object for a specific View has already been created this object is
     * returned (instead of creating a new one).
     */
    template <typename ELEMENT, typename CREATOR>
    class StaticFactoryBroker
    {
	};

	template <typename ELEMENT>
	class StaticFactoryBroker<ELEMENT, PyConfigViewCreator<ELEMENT> >
	{
		/**
		 * @brief The keys for Registry.
		 *
		 *  Registry needs operator<< for Exceptions, thus
		 *  we need to extend std::pair.
		 */
		class RegistryKey :
			public std::pair<std::string, pyconfig::View>
			{
			public:
				RegistryKey(const std::string& s, const pyconfig::View& pyco)
					: std::pair<std::string, pyconfig::View>(s, pyco)
					{}

				friend std::ostream& operator <<(std::ostream& str, const StaticFactoryBroker<ELEMENT, PyConfigViewCreator<ELEMENT> >::RegistryKey& k)
				{
					str << "(" << k.first << ", " << k.second << ")";
					return str;
				}
			};

		/**
		 * @brief Store objects created from pyconfig::View.
		 */
		typedef container::Registry<RegistryKey, ELEMENT*, container::registry::DeleteOnErase> Registry;
		/**
		 * @brief ELEMENTs are stored here.
		 */
		Registry registry;
	public:
		/**
		 * @brief Will delete all elements.
		 */
		~StaticFactoryBroker()
		{
			registry.clear();
		}
		/**
		 * @brief Returns pyco configured object specified by name.
		 */
		ELEMENT*
		procure(const std::string name, const pyconfig::View& pyco)
		{
			typedef PyConfigViewCreator<ELEMENT> ElementCreator;
			typedef StaticFactory<ElementCreator> ElementFactory;
			assure(ElementFactory::knows(name), "StaticFactoryBroker: Factory does not know '" + name + "'");
			const RegistryKey key(name, pyco);
			try
			{
				return registry.find(key);
			}
			catch (const typename Registry::UnknownKeyValue&)
			{
				ELEMENT* element = ElementFactory::creator(name)->create(pyco);
				registry.insert(key, element);
				return element;
			}
			catch (...)
			{
				throw;
			}
		}

	};


	template <typename ELEMENT>
	class StaticFactoryBroker<ELEMENT, Creator<ELEMENT> >
	{
		/**
		 * @brief Store objects created from name.
		 */
		typedef container::Registry<std::string, ELEMENT*, container::registry::DeleteOnErase> Registry;
		/**
		 * @brief ELEMENTs are stored here.
		 */
		Registry registry;
	public:
		/**
		 * @brief Will delete all elements.
		 */
		~StaticFactoryBroker()
		{
			registry.clear();
		}

		/**
		 * @brief Returns object specified by name.
		 */
		ELEMENT*
		procure(const std::string name)
		{
			typedef wns::Creator<ELEMENT> ElementCreator;
			typedef StaticFactory<ElementCreator> ElementFactory;
			assure(ElementFactory::knows(name), "StaticFactoryBroker: Factory does not know '" + name + "'");
			const std::string key(name);
			try
			{
				return registry.find(key);
			}
			catch (const typename Registry::UnknownKeyValue&)
			{
				ELEMENT* element = ElementFactory::creator(name)->create();
				registry.insert(key, element);
				return element;
			}
			catch (...)
			{
				throw;
			}
		}
	};

} // wns

#define STATIC_FACTORY_BROKER_REGISTER(CLASS, INTERFACE, NAME) STATIC_FACTORY_REGISTER_WITH_CREATOR(CLASS, INTERFACE, NAME, ::wns::PyConfigViewCreator)
#define STATIC_FACTORY_BROKER_REGISTER_PLAIN(CLASS, INTERFACE, NAME) STATIC_FACTORY_REGISTER(CLASS, INTERFACE, NAME)

#endif // NOT defined WNS_STATICFACTORYBROKER_HPP
