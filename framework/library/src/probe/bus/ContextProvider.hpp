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

#ifndef WNS_PROBE_BUS_CONTEXTPROVIDER_HPP
#define WNS_PROBE_BUS_CONTEXTPROVIDER_HPP

#include <WNS/probe/bus/Context.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>

#include <WNS/osi/PDU.hpp>
#include <boost/function.hpp>

namespace wns { namespace probe { namespace bus {

    /**
     * @brief Interface for contextproviders. When visited, they may add their
     * context information into the Context object
     */
    class IContextProvider
    {
    public:
        /**
         * @brief when visisted through this interface, ContextProviders
         * may add information into the context Object
         */
        virtual void
        visit(IContext&) const = 0;

        /**
         * @brief when visisted through this interface, ContextProviders
         * may add information into the context Object and use the provided PDU
         * to determine the context
         */
        virtual void
        visit(IContext&, const wns::osi::PDUPtr&) const = 0;

        virtual ~IContextProvider() {}
    };

    /**
     * @brief A contextprovider template to be specialized for specific PDUs
     */

    template<typename T>
    class PDUContextProvider:
        public IContextProvider
    {
    public:

        /**
         * @brief This method does nothing and just returns. 
         */
        virtual void
        visit(IContext&) const{return;};

        /**
         * @brief Check if we have a PDU of corresponding type and dispatch to
         * specialized method of derived class.
         */

        virtual void
        visit(IContext& c, const wns::osi::PDUPtr& pdu) const
        {
            // Check if not NULL, return else
            if (!pdu) return;

            // Check if of type T, return else
            if (NULL == dynamic_cast<T*>(pdu.getPtr())) return;

            // Do the cast and dispatch
            const SmartPtr<T>& compound = wns::dynamicCast<T>(pdu);
            this->doVisit(c, compound);
        };

    private:
        virtual void
        doVisit(IContext&, const SmartPtr<T>&) const = 0;
    };

    /**
     * @brief A contextprovider not using the provided PDU. The PDU is ignored
     * and the visit function without PDU parameter is called
     */
    class ContextProvider:
        public IContextProvider
    {
    public:
        virtual void
        visit(IContext& c) const
        {
            this->doVisit(c);
        };

        /**
         * @brief A provided PDU is ignored and the visit function without
         * PDU of derived class is called.
         */
        virtual void
        visit(IContext& c, const wns::osi::PDUPtr&) const
        {
            this->doVisit(c);
        };

    private:
        virtual void
        doVisit(IContext&) const = 0;
    };


	typedef PyConfigViewCreator<IContextProvider> ContextProviderCreator;
	typedef wns::StaticFactory<ContextProviderCreator> ContextProviderFactory;

	namespace contextprovider {

		/**
		 * @brief Provides a constant key/value pair into the Context
		 */
		class Constant :
			public ContextProvider
		{
		public:
			Constant(const std::string& key, int value_);

			explicit
			Constant(const pyconfig::View& config);

		protected:
			const std::string key_;
			const int value_;

		private:
			virtual void
			doVisit(IContext&) const;
		};

		/**
		 * @brief Variable Context Provider
		 *
		 * This Provider allows to re-set the context value it
		 * provides. Since the ContextProviderCollection stores and owns copies
		 * of the providers, you may not add this Provider
		 * directly. Instead, you have to instantiate it and wrap it in
		 * a @see Container. Thus your handle to the provider keeps
		 * intact and you can modify it whenever you like.
		 */
		class Variable :
			public ContextProvider
		{
			// prohibit use of copy constructor
			Variable(const Variable&) : ContextProvider(), key_(), value_() {}
			std::string key_;
			int value_;
		public:
			Variable(const std::string& key, int value_);

			virtual void
			set(int value_);

			virtual int
			get() const;
		private:
			virtual void
			doVisit(IContext&) const;

		};

		/**
		 * @brief Wrapper to allow keeping external handles to
		 * ContextProviders.
		 * @see Variable
		 */
		class Container :
			public IContextProvider
		{
			IContextProvider* provider;
		public:
			Container(IContextProvider* p) : provider(p) {}

			virtual void
			visit(IContext& context) const
				{
					provider->visit(context);
				}

			virtual void
			visit(IContext& context, const wns::osi::PDUPtr& pdu) const
				{
					provider->visit(context, pdu);
				}

		};

		/**
		 * @brief 'int' Member function IDProvider.
		 *
		 * The Callback ContextProvider calls you back whenever
		 * context value needs to be provided. Use this if your context
		 * changes over time.
		 *
		 * Usage:
		 * Suppose you have a class with an internal value that you want
		 * to put on the ProbeBus as Context. Example:
		 * @includelineno contextprovider.callback.userclass.example
		 * Now you can create a Callback context provider that calls the
		 * getter() function like this:
		 * @includelineno contextprovider.callback.usage.example
		 */
		class Callback :
			public ContextProvider
		{
		public:
			Callback(const std::string& key,
				 boost::function0<int> callback):
				key_(key),
				callback_(callback)
			{}

		private:
			virtual void
			doVisit(IContext& c) const
				{
					int value = callback_();
					c.insertInt(key_, value);
				}

			std::string key_;
			boost::function0<int> callback_;

		};
	}

}}}


#endif // NOT defined WNS_PROBE_IDPROVIDER_HPP

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
