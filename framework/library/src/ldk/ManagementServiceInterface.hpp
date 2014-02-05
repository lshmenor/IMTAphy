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

#ifndef WNS_LDK_MANAGEMENTSERVICEINTERFACE_HPP
#define WNS_LDK_MANAGEMENTSERVICEINTERFACE_HPP

#include <WNS/container/Registry.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>

#include <string>

namespace wns { namespace ldk {

	class ManagementServiceInterface;
	class ILayer;

       /**
         * @brief The ManagementServiceRegistry keeps all instances of ManagementServices.
         */
	class ManagementServiceRegistry :
		public wns::container::Registry<std::string, ManagementServiceInterface*, wns::container::registry::DeleteOnErase>
	{
	public:
		ManagementServiceRegistry( wns::ldk::ILayer* _layer )
			: layer( _layer ) {}

		virtual wns::ldk::ILayer*
		getLayer() const { return layer; }

		/**
		 * @brief getLayer with templated casting of the layertype
		 */
		template <typename LAYERTYPE>
		LAYERTYPE
		getLayer() const
		{
			wns::ldk::ILayer* aLayer = getLayer();
			assureType(aLayer, LAYERTYPE);
			// we can't use C-Style downcasts here!
			return dynamic_cast<LAYERTYPE>(aLayer);
		}

		/**
		 * @brief Inform all registered ManagementServices about the
		 * creation of the MSR.
		 */
		void
		onMSRCreated();

	private:
		wns::ldk::ILayer* layer;
	};

	/**
	 * @brief Interface definition for Management Services.
	 *
	 */
	class ManagementServiceInterface
	{
	public:
		virtual ManagementServiceRegistry*
		getMSR() const = 0;

		virtual
		~ManagementServiceInterface()
		{}

		virtual void
		onMSRCreated() = 0;
	};

	class ManagementService :
		public virtual ManagementServiceInterface
	{
	public:
		ManagementService( ManagementServiceRegistry* _msr)
			: msr( _msr )
		{}

		ManagementServiceRegistry*
		getMSR() const
		{ return msr; }

		virtual void
		onMSRCreated(){}

	private:
		ManagementServiceRegistry* msr;
	};

	/**
	 * @brief Creator implementation to be used with StaticFactory.
	 *
	 * Useful for constructors with a ManagementServiceRegistry and pyconfig::View parameter.
	 *
	 */
	template <typename T, typename KIND = T>
	struct MSRConfigCreator :
		public MSRConfigCreator<KIND, KIND>
	{
		virtual KIND* create(ManagementServiceRegistry* msr, wns::pyconfig::View& config)
		{
			return new T(msr, config);
		}
	};

	template <typename KIND>
	struct MSRConfigCreator<KIND, KIND>
	{
	public:
		virtual KIND* create(ManagementServiceRegistry*, wns::pyconfig::View&) = 0;

		virtual ~MSRConfigCreator()
		{}
	};


	typedef MSRConfigCreator<ManagementServiceInterface> ManagementServiceCreator;
	typedef StaticFactory<ManagementServiceCreator> ManagementServiceFactory;

} // ldk
} // wns


#endif // NOT defined WNS_LDK_MANAGEMENTSERVICEINTERFACE_HPP


