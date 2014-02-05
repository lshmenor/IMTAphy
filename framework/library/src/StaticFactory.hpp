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

#ifndef WNS_STATICFACTORY_HPP
#define WNS_STATICFACTORY_HPP

#include <WNS/Exception.hpp>
#include <WNS/TypeInfo.hpp>

#include <sstream>
#include <string>

#include <map>
#include <vector>

namespace wns {
	/**
	 * @page StaticFactoryPage StaticFactory
	 * \tableOfContents
	 * @section staticfactoryusing Use StaticFactory
	 *
	 * A StaticFactory provides an easy to use interface for plugin handling.
	 * Plugins register at a StaticFactory in a decentralised fashion through the
	 * use of static, namespace-level objects. That means that at the time your
	 * main function gains control, all plugins have already been registered.
	 * If you choose to dynamically load shared objects that contain plugins, the
	 * plugins get registered during shared object initialisation. There is no
	 * central point where you have to register the plugins by hand.
	 *
	 * By convention, every StaticFactory template instance is typedefed for your
	 * convenience. Assume a collection of plugins that all conform to a given
	 * interface, say TestInterface:
	 *
	 * @include wns.StaticFactory.InterfaceDefinition.example
	 *
	 * Then there are two more types declared to work with:
	 * @li TestFactory, and
	 * @li TestCreator.
	 *
	 * Having these types defined, working with a StaticFactory is pretty simple:
	 *
	 * @include wns.StaticFactory.testSimpleCreation.example
	 *
	 * @section staticfactoryimplementing Write your own plugin
	 *
	 * Each StaticFactory template instance is a static class that serves
	 * as a factory for objects conforming to a specific interface.
	 * The interface includes behaviour, state and signature of the constructor.
	 *
	 * Each StaticFactory defines its interface as an abstract class. For
	 * example the StaticFactory 'TestFactory' could define a class 'TestInterface'.
	 * To write your own plugin, you simply have to write a realisation of
	 * the interface and register your plugin at the StaticFactory:
	 *
	 * @include wns.StaticFactory.InterfaceImplementation.example
	 *
	 * @note Don't forget to register your plugin using STATIC_FACTORY_REGISTER.
	 *
	 * That's all.
	 *
	 * Things get only slightly more difficult, if you want to provide
	 * a plugin for a StaticFactory that requests a constructor with another
	 * signature. As you saw, plugin instances are created using a creator.
	 * For different constructor signatures, you need different creators.
	 * Creators are named loosely after their signature by convention. For example
	 * LongCreator for interfaces that require the constructor to have one
	 * argument of type long, LayerConfigCreator for constructors with
	 * the signature (ILayer*, pyconfig::View*).
	 *
	 * Assuming a FunkFactory, that requires its plugin implementations
	 * to have a constructor with a long as single argument, an implementation
	 * could look like:
	 *
	 * @include wns.StaticFactory.Hancock.example
	 *
	 * @note Use STATIC_FACTORY_REGISTER_WITH_CREATOR when writing plugins with a
	 * non-default constructor signature.
	 *
	 * @section staticfactorycreate Raw meat: create your own StaticFactory
	 *
	 * To create your own StaticFactory, you have to provide three things:
	 * @li an interface,
	 * @li a creator, and
	 * @li the factory.
	 *
	 * Defining the interface is simple: just write an abstract class, describing you interface.
	 * Assume, you want to implement several classes that conform to a given interface (that have
	 * some public state/behaviour):
	 * @include wns.StaticFactory.InterfaceDefinition.example
	 *
	 * Second you have to choose a creator. If you are lucky, a creator with
	 * the desired constructor signature already exists. If not, have a look at LongCreator.hpp
	 * or LayerConfigCreator.hpp for examples.
	 *
	 * The only thing left to do is to provide some typedefs for convenience:
	 * @include wns.StaticFactory.Thingies.example
	 *
	 * If you chose to use a different creator than the default creator Creator, you have to
	 * use that instead. Have a look at the complete FunkInterface components for an example
	 * of how to create a factory with a different creator:
	 *
	 * @include wns.StaticFactory.FunkInterface.example
	 */

	/**
	 * @brief Monostate class keeping a registry of creators for interface-implementations.
	 *
	 * @author Fabian Debus <fabian.debus@illator.de>
	 *
	 * Indepth documentation can be found on the @ref StaticFactoryPage page.
	 *
	 */
	template <typename CREATOR>
	class StaticFactory {
	public:

		/**
		 * @brief Register a creator.
		 *
		 * In most cases this method will not be used directly. StaticFactoryRegister instances
		 * use this method to register creators.
		 */
		static void reg(std::string name, CREATOR* creator);
		typedef void (*RegisterFunction)(std::string name, CREATOR* creator);

		/**
		 * @brief Lookup and return the creator with the given name.
		 *
		 * If no creator with the given name has been registered, throw an exception
		 * of type FIXME. Never return a NULL-pointer.
		 */
		static CREATOR* creator(std::string name);

		/**
		 * @brief Check if a creator with the given name has registered.
		 *
		 */
		static bool knows(std::string name);

		/**
		 * @brief Internal storage definition for name to creator mapping.
		 *
		 * Useful for reflection.
		 */
		typedef std::map<std::string, CREATOR*> CreateMap;

		/**
		 * @brief Return the internal mapping.
		 *
		 * Useful for reflection.
		 */
		static CreateMap* getMap();
	};


	template <typename CREATOR>
	typename StaticFactory<CREATOR>::CreateMap*
	StaticFactory<CREATOR>::getMap()
	{
		static CreateMap plugins = CreateMap();

		return &plugins;
	} // getMap


	template <typename CREATOR>
	CREATOR*
	StaticFactory<CREATOR>::creator(std::string name)
	{
		if(getMap()->find(name) == getMap()->end()) {
			std::stringstream ss;
			ss << "StaticFactory<" << wns::TypeInfo::create<CREATOR>() << "> says:\n"
			   << "You tried to create a '" << name << "' instance.\n"
			   << "Valid choices are:\n\n";
			for(typename CreateMap::iterator i = getMap()->begin();
			    i != getMap()->end();
			    ++i) {
				ss << "  * " << i->first << std::endl;
			}
			throw Exception(ss.str().c_str());
		}

		return (*getMap())[name];
	} // create


	template <typename CREATOR>
	void
	StaticFactory<CREATOR>::reg(std::string name, CREATOR* creator)
	{
		if(StaticFactory<CREATOR>::knows(name)) {
			Exception e;
			e << "there is already a plugin registered with name '"
			  << name << "', what is probably not what you want.";
			throw e;
		}

		(*getMap())[name] = creator;
	} // reg


	template <typename CREATOR>
	bool
	StaticFactory<CREATOR>::knows(std::string name)
	{
		return getMap()->count(name) == 1;
	} // knows


	/**
	 * @brief Factory method interface to use as creator for StaticFactory.
	 *
	 * Create instances of a given class. This is the default Creator, creating
	 * instances using the new-operator and the default constructor.
	 *
	 * Alternative implementations are possible to support different constructor
	 * signatures.
	 *
	 * Note that there is one StaticFactory for each Creator template instantiation
	 * and thus for each constructor signature. That is, every implementation of
	 * an abstract interface has to implement the same constructor interface.
	 *
	 * @note This creator does *not* support instantiation of interfaces. It is only
	 * capable of handling abstract interfaces. To implement a creator that can
	 * handle non-abstract interfaces, the create method of the Creator<KIND, KIND>
	 * specialization has be the same as the default template create method. Such
	 * a creator will not work for abstract interface classes (Referencing the new
	 * operator of an abstract class yields a compile time error.)
	 */
	template <typename T, typename KIND = T>
	class Creator :
		public Creator<KIND, KIND>
	{
	public:
		virtual KIND* create()
		{
			return new T;
		}
	};


	template <typename KIND>
	class Creator<KIND, KIND>
	{
	public:
		virtual KIND* create() = 0;

		virtual ~Creator()
		{};
	};


	/**
	 * @brief Provide creator interface and register at a StaticFactory.
	 *
	 * Enable implementations to statically register at a StaticFactory.
	 */
	template <typename T, typename KIND,
		  template <typename, typename> class CREATOR = Creator >
	class StaticFactoryRegister :
		public CREATOR<T, KIND>
	{
	public:
		StaticFactoryRegister(std::string name)
		{
			StaticFactory<CREATOR<KIND, KIND> >::reg(name, this);
		}
	};

// this was taken from cppunit
#define STATIC_FACTORY_JOIN(SYMBOL1, SYMBOL2) STATIC_FACTORY_DO_JOIN(SYMBOL1, SYMBOL2)
#define STATIC_FACTORY_DO_JOIN(SYMBOL1, SYMBOL2) STATIC_FACTORY_DO_JOIN2(SYMBOL1, SYMBOL2)
#define STATIC_FACTORY_DO_JOIN2(SYMBOL1, SYMBOL2) SYMBOL1##SYMBOL2

#define STATIC_FACTORY_UNIQUE_NAME(PREFIX) \
    STATIC_FACTORY_JOIN(PREFIX,__LINE__)


	/**
	 * @brief Register a class at a static factory.
	 *
	 * Register CLASS at the static factory for INTERFACE. INTERFACE must be a
	 * super class of CLASS. CLASS will be registered using the name NAME.  <p>
	 *
	 * The default constructor will be used to create instances. Thus, CLASS
	 * must have an implementation for the default constructor. To register a
	 * CLASS at a static factory with non default constructor signature, use
	 * STATIC_FACTORY_REGISTER_WITH_CREATOR instead.
	 *
	 */
#define STATIC_FACTORY_REGISTER(CLASS, INTERFACE, NAME) \
    namespace { \
	static wns::StaticFactoryRegister<CLASS, INTERFACE> STATIC_FACTORY_UNIQUE_NAME(_)(NAME); \
    } \
    class StaticFactoryRegisterDummy

	/**
	 * @brief Register a class at a static factory using a non-default creator.
	 *
	 * Register CLASS at the static factory for INTERFACE. INTERFACE must be a
	 * super class of CLASS. CLASS will be registered using the name NAME. <p>
	 *
	 * The constructor used to instantiate CLASS depends on the type of
	 * CREATOR. To register a CLASS at a static factory with the default
	 * constructor signature, use STATIC_FACTORY_REGISTER instead. Note that
	 * CREATOR is a property of a static factory. Using different CREATORs
	 * results in distinct static factory instances.
	 */
#define STATIC_FACTORY_REGISTER_WITH_CREATOR(CLASS, INTERFACE, NAME, CREATOR) \
    namespace { \
	static wns::StaticFactoryRegister<CLASS, INTERFACE, CREATOR> STATIC_FACTORY_UNIQUE_NAME(_)(NAME); \
    } \
    class StaticFactoryRegisterDummy
}

#endif // NOT defined WNS_STATICFACTORY_HPP

#define STATIC_FACTORY_DEFINE(INTERFACE, CREATOR) \
    typedef CREATOR<INTERFACE> INTERFACE ## Creator;\
    typedef wns::StaticFactory<INTERFACE ## Creator> INTERFACE ## Factory;

#define STATIC_FACTORY_NEW_INSTANCE(INTERFACE, CREATOR, VIEW, ARGS...) \
    wns::StaticFactory< CREATOR<INTERFACE> >::creator(VIEW.get<std::string>("nameInStaticFactory"))->create(ARGS)

