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

#ifndef WNS_LDK_CONTROLSERVICEINTERFACE_HPP
#define WNS_LDK_CONTROLSERVICEINTERFACE_HPP

#include <WNS/container/Registry.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>


#include <string>

namespace wns { namespace ldk {

	class ControlServiceInterface;
	class ILayer;


       /**
         * @brief The ControlServiceRegistry keeps all instances of ControlServices.
         */
	class ControlServiceRegistry :
		public wns::container::Registry<std::string, ControlServiceInterface*, wns::container::registry::DeleteOnErase>
	{
	public:
		ControlServiceRegistry( wns::ldk::ILayer* _layer)
			: layer( _layer ) {}

		virtual	wns::ldk::ILayer*
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

		void
		onCSRCreated();
	private:
		wns::ldk::ILayer* layer;
	};


	/**
	 * @brief Interface definition for Control Services.
	 *
	 */
	class ControlServiceInterface
	{
	public:
		virtual ControlServiceRegistry*
		getCSR() const = 0;

		virtual
		~ControlServiceInterface()
		{}

		virtual void
		onCSRCreated() = 0;

	};

        /**
         * @brief Common base class for Control Services.
	 */
	class ControlService :
		public virtual ControlServiceInterface
	{
	public:
		ControlService( ControlServiceRegistry* _csr)
			: csr( _csr ) {}

		ControlServiceRegistry*
		getCSR() const
		{ return csr; }

		virtual void
		onCSRCreated(){};

	private:
		ControlServiceRegistry* csr;
	};

	/**
	 * @brief Creator implementation to be used with StaticFactory.
	 *
	 * Useful for constructors with a ControlServiceRegistry and pyconfig::View parameter.
	 *
	 */
	template <typename T, typename KIND = T>
	struct CSRConfigCreator :
		public CSRConfigCreator<KIND, KIND>
	{
		virtual KIND* create(ControlServiceRegistry* csr, wns::pyconfig::View& config)
		{
			return new T(csr, config);
		}
	};

	template <typename KIND>
	struct CSRConfigCreator<KIND, KIND>
	{
	public:
		virtual KIND* create(ControlServiceRegistry*, wns::pyconfig::View&) = 0;

		virtual ~CSRConfigCreator()
		{}
	};


	typedef CSRConfigCreator<ControlServiceInterface> ControlServiceCreator;
	typedef StaticFactory<ControlServiceCreator> ControlServiceFactory;

}}


#endif // NOT defined WNS_LDK_CONTROLSERVICEINTERFACE_HPP


