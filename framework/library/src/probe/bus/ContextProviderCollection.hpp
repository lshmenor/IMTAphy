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

#ifndef WNS_PROBE_BUS_CONTEXTPROVIDERREGISTRY_HPP
#define WNS_PROBE_BUS_CONTEXTPROVIDERREGISTRY_HPP

#include <WNS/probe/bus/ContextProvider.hpp>

#include <WNS/osi/PDU.hpp>

#include <string>
#include <list>

namespace wns { namespace probe { namespace bus {

	/**
	 * @brief Registry for ContextProvider.
	 *
	 * The implementation assumes that the ContextProviderCollection has the
	 * ownership of the ContextProviders. Hence it will delete them when being
	 * shut down.
	 */
	class ContextProviderCollection
	{
		/**
		 * @brief Interface Providing clone functionality
		 */
		class IProviderWrapper :
			public wns::probe::bus::IContextProvider
		{
		public:
			virtual IProviderWrapper*
			clone() const = 0;
		};

		/**
		 * @brief Wrapper class for typesafe cloning and storage of the ContextProviders
		 */
		template <typename T>
		class ProviderWrapper :
			public IProviderWrapper
		{
			T wrapee_;

			virtual IProviderWrapper*
			clone() const
				{
					return new ProviderWrapper<T>(*this);
				}
		public:
			explicit
			ProviderWrapper(T t) : wrapee_(t) {}

            virtual void
            visit(IContext& context, const wns::osi::PDUPtr& pdu) const
                {
                    wrapee_.visit(context,  pdu);
                }

            virtual void
            visit(IContext& context) const
                {
                    wrapee_.visit(context);
                }

		};


	public:
		typedef std::list<IProviderWrapper*> ProviderContainer;
		typedef ProviderContainer::const_iterator ProviderIterator;

		ContextProviderCollection() :
			parent_(NULL),
			providers_()
		{}

		ContextProviderCollection(ContextProviderCollection* parent) :
			parent_(parent),
			providers_()
		{
		}

		ContextProviderCollection(const ContextProviderCollection& other) :
			parent_(other.parent_),
			providers_()
		{
			for (ProviderContainer::const_iterator iter = other.providers_.begin();
			     iter != other.providers_.end();
			     ++iter)
			{
				this->providers_.push_back( (*iter)->clone() );
			}
		}

		~ContextProviderCollection(){
			parent_ = NULL;

			while (!providers_.empty())
			{
				delete providers_.front();
				providers_.pop_front();
			}
		}

		/**
		 * @brief Add a wrapped copy of the Provider Object into the registry.
		 */
		template <typename T>
		void
		addProvider(T provider)
		{
			providers_.push_back(new ProviderWrapper<T>(provider));
		} // addProvider

        /**
         * @brief fill Context Object with context Information.
         */
        void
        fillContext(IContext& context) const
        {
            if (parent_ != NULL)
            {
                parent_->fillContext(context);
            }
            for (ProviderContainer::const_iterator iter = providers_.begin();
                 iter != providers_.end();
                 ++iter)
            {
                (*iter)->visit(context);
            }
        }

        /**
         * @brief fill Context Object with context Information. Include a PDU to help to determine the context.
         */
        void
        fillContext(IContext& context, const wns::osi::PDUPtr& compound) const
        {
            if (parent_ != NULL)
            {
                parent_->fillContext(context, compound);
            }
            for (ProviderContainer::const_iterator iter = providers_.begin();
                 iter != providers_.end();
                 ++iter)
            {
                (*iter)->visit(context, compound);
            }
        }

	private:
		ContextProviderCollection* parent_;
		ProviderContainer providers_;
	};
}}}

#endif // NOT defined WNS_PROBE_BUS_CONTEXTPROVIDERREGISTRY_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
