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

#ifndef WNS_MODULE_MODULE_HPP
#define WNS_MODULE_MODULE_HPP

#include <WNS/module/Base.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Exception.hpp>

#define MODULE_REGISTER_SERVICE(CreateType, Name, CreateFunctionName)\
registerService<CreateType, &CreateFunctionName>(Name);

namespace wns { namespace module {

	/**
	 * @brief Support for the integration of extension modules to openWNS
	 *
	 * Here is an example what your class might look like:
	 * @include module::ModuleClass.example
	 */
	template <typename Deriver>
	class Module :
		public Base
	{
	public:
		/**
		 * @brief Default constructor
		 */
		Module(const pyconfig::View& _pyConfigView) :
			Base(_pyConfigView)
		{
			pyConfigView()=_pyConfigView;
			if(instantiated == true) {
				Exception e("Only one instance of a module::Module allowed");
				throw e;
			}
			instantiated = true;
		}

		/**
		 * @brief Destructor
		 */
		virtual
		~Module()
		{
			instantiated = false;
		}

		/**
		 * @brief Is the module already instantiated
		 */
		static bool
		isInstantiated()
		{
			return instantiated;
		}

		/**
		 * @brief wns::pyconfig::View of this module::Module as provided by WNS
                 * @todo dbn : Align the method name with Component. There it is named getConfig()
		 */
		static wns::pyconfig::View&
		getPyConfigView()
		{
			return pyConfigView();
		}

	protected:
		/**
		 * @brief Register a service of "this" module
		 */
		template <typename T,
			  T (Deriver::*callback)()>
		void
		registerService(const std::string& s)
		{
			struct Local
			{
				static wns::ChamaeleonBase* Trampoline(
					Base* mod)
				{
					assureType(mod, Deriver*);
					Deriver* m = static_cast<Deriver*>(mod);
					return new wns::Chamaeleon<T>(((*m).*callback)());
				}
			};
			addServiceToGlobalMap(s);
			convertMap[s]=&Local::Trampoline;
		}

		/**
		 * @brief pyconfig::View of this module::Module
		 *
		 * Since the pyconfig::View is static in the specialized version
		 * of the module it can be easily accessed from every where
		 *
		 * This version returns a reference and therefor the static
		 * variable inside can be changed. This only to be used in
		 * module::Module<Deriver> itself.
		 */
		static pyconfig::View&
		pyConfigView()
		{
			// Initialize with some default value
			static pyconfig::View p = pyconfig::Parser();
			return p;
		}

	private:
		/**
		 * @brief indicates if a module has be instantiated
		 */
		static bool instantiated;
	};

	template <typename Deriver>
	bool
	Module<Deriver>::instantiated = false;

} // module
} // wns
#endif  // NOT defined WNS_MODULE_HPP

