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

#ifndef WNS_LDK_FLOWSEPARATOR_HPP
#define WNS_LDK_FLOWSEPARATOR_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/ControlServiceInterface.hpp>
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Key.hpp>
#include <WNS/ldk/flowseparator/NotFoundStrategy.hpp>
#include <WNS/ldk/flowseparator/FlowInfoProvider.hpp>
#include <WNS/ldk/flowseparator/CreatorStrategy.hpp>

#include <WNS/distribution/Uniform.hpp>

#include <WNS/DerefLess.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/pyconfig/View.hpp>

#include <WNS/Enum.hpp>

#include <string>
#include <map>
#include <memory>

namespace wns { namespace ldk {

	/**
	 * @defgroup ldkflowseparator FlowSeparator
	 * @brief Dynamic FU instantiation.
	 * @ingroup ldk
	 */
	ENUM_BEGIN(Direction);
	ENUM(INCOMING, 0);
	ENUM(OUTGOING, 1);
	ENUM_END();

	/**
	 * @brief Manage FU instances for different flows.
	 * @ingroup ldkflowseparator
	 * @todo pab, 2007-02-12: cloning the flowseparator currently results in multiple
	 * flowseparators sharing the same notFoundStrategy Object and the same
	 * KeyBuilder Object
	 *
	 * Static Structure of FlowSeparator:
	 * @image html FlowSeparator.png "" width=10cm
	 */
	class FlowSeparator :
		public FunctionalUnit,
		public CommandTypeSpecifier<>,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>,
		public NotCloneable
	{
		class InstanceNotFound :
			public Exception
		{
		public:
			explicit
			InstanceNotFound(const ConstKeyPtr& _key) :
				Exception("Instance to the following key not found:"),
				key(_key)
			{
				(*this) << this->key->str();
			}

			virtual
			~InstanceNotFound() throw()
			{}

			ConstKeyPtr key;
		};

            template <class RECEPTACLETYPE>
            class ReceptacleManagement
            {
            public:
                typedef std::map<ConstKeyPtr, RECEPTACLETYPE*, DerefLess<ConstKeyPtr> > ReceptacleContainer;

                ReceptacleManagement(FlowSeparator* fs)
                    : fs_(fs),
                      receptacleContainer_()
                {}

                ~ReceptacleManagement()
                {}

                void
                addInstance(const ConstKeyPtr& key, RECEPTACLETYPE* receptacle)
                {
                    assure(NULL == getInstance(key),
                           "trying to add an instance for an already known key.");

                    MESSAGE_BEGIN(NORMAL, fs_->logger, m, fs_->getFUN()->getName());
                    m <<": Add receptacle for Instance/Flow      Key: "<< key->str()
                      << ";       FU: " << receptacle->getFU()->getName();
                    MESSAGE_END();

                    receptacleContainer_[key] = receptacle;
                }

                void
                removeInstance(const ConstKeyPtr& key)
                {
                    typename ReceptacleContainer::iterator it = receptacleContainer_.find(key);
                    assure(it != receptacleContainer_.end(),
                           "trying to disintegrate receptacle for an unknown key.");

                    assure(fs_->instanceBusy != fs_->instances.find(key)->second,
                            "ReceptacleManagement::removeInstance: Can't remove busy Instance/Flow!");

                    receptacleContainer_.erase(it);
                }

                RECEPTACLETYPE*
                getInstance(const ConstKeyPtr& key) const
                {
                    typename ReceptacleContainer::const_iterator it = receptacleContainer_.find(key);

                    if(it == receptacleContainer_.end())
                    {
                        return NULL;
                    }

                    return it->second;
                } // getInstance

                RECEPTACLETYPE*
                _getInstance(const CompoundPtr& compound, int direction) const
                {
                    ConstKeyPtr key = (*(fs_->keyBuilder))(compound, direction);

                    typename ReceptacleContainer::const_iterator it = receptacleContainer_.find(key);

                    if(it == receptacleContainer_.end())
                    {
                        throw InstanceNotFound(key);
                    }

                    MESSAGE_BEGIN(VERBOSE, fs_->logger, m, fs_->getFUN()->getName());
                    m << ": "
                      << "reusing instance for key "
                      << key->str();
                    MESSAGE_END();

                    return it->second;
                } // getInstance

                RECEPTACLETYPE*
                tryGetInstanceAndInsertPermanent(const CompoundPtr& compound, int direction)
                {
                    RECEPTACLETYPE* receptacle = NULL;
                    try
                    {
                        receptacle = _getInstance(compound, direction);
                    }
                    catch(const InstanceNotFound& ifn)
                    {
                        MESSAGE_SINGLE(
                            VERBOSE,
                            fs_->logger,
                            fs_->getFUN()->getName()<<": " <<
                            "adding new instance for key (" <<
                            ifn.key->str() <<
                            ") permanently to FlowSeparator");
                        fs_->addInstance(ifn.key);

                        // try, if we now get the missing instance
                        try
                        {
                            receptacle = _getInstance(compound, direction);
                        }
                        catch(const InstanceNotFound& ifn)
                        {
                            wns::Exception e;
                            e << "Creation of new instance failed";
                            throw e;
                        }
                    }
                    // throw on
                    catch(...)
                    {
                        throw;
                    }
                    return receptacle;
                }

            protected:
                FlowSeparator* fs_;
                ReceptacleContainer receptacleContainer_;
            };

            class ConnectorReceptacleSeparator
                : public ReceptacleManagement<IConnectorReceptacle>,
                  public IConnectorReceptacle
            {
            public:

                ConnectorReceptacleSeparator(FlowSeparator* fs, std::string portname);
                ~ConnectorReceptacleSeparator();

                virtual void
                sendData(const CompoundPtr& compound);

                virtual void
                doSendData(const CompoundPtr& compound);

                virtual bool
                isAccepting(const CompoundPtr& compound);

                virtual bool
                doIsAccepting(const CompoundPtr& compound) const;

                virtual FunctionalUnit*
                getFU();

            private:
                std::string portname_;

            };

            class DelivererReceptacleSeparator
                : public ReceptacleManagement<IDelivererReceptacle>,
                  public IDelivererReceptacle
            {
            public:
                DelivererReceptacleSeparator(FlowSeparator* fs);
                ~DelivererReceptacleSeparator();

                virtual void
                onData(const CompoundPtr& compound);

                virtual void
                doOnData(const CompoundPtr& compound);

                virtual FunctionalUnit*
                getFU();
            };

            class ReceptorReceptacleSeparator
                : public ReceptacleManagement<IReceptorReceptacle>,
                  public IReceptorReceptacle
            {
            public:
                ReceptorReceptacleSeparator(FlowSeparator* fs);
                ~ReceptorReceptacleSeparator();

                virtual void
                wakeup();

                virtual void
                doWakeup();

                virtual FunctionalUnit*
                getFU();
            };

	public:
 		typedef std::map<ConstKeyPtr, FunctionalUnit*, DerefLess<ConstKeyPtr> > InstanceMap;
            typedef std::list<std::string> StringList;
            typedef std::list<ConnectorReceptacleSeparator*> ConnectorReceptacleSeparatorList;
            typedef std::list<DelivererReceptacleSeparator*> DelivererReceptacleSeparatorList;
            typedef std::list<ReceptorReceptacleSeparator*> ReceptorReceptacleSeparatorList;

            friend class ConnectorReceptacleSeparator;

		/**
		 * @brief Constructor for FlowSeparator usage outside automatic FUN creation
		 *
		 */
		FlowSeparator(
			fun::FUN* fuNet,
			const pyconfig::View& _config,
			std::auto_ptr<KeyBuilder> _keyBuilder,
			std::auto_ptr<flowseparator::NotFoundStrategy> _notFound);

		/**
		 * @brief FunctionalUnitFactory compliant constructor for automatic FUN creation
		 *
		 */
		FlowSeparator(
			fun::FUN* fuNet,
			const pyconfig::View& _config);

            FlowSeparator(const FlowSeparator&);


		virtual
		~FlowSeparator();


		virtual void
		onFUNCreated();

		/**
		 * @brief Return the number of instances currently managed by the FlowSeparator
		 *
		 */
		virtual std::size_t
		size() const;

		/**
		 * @brief Return the instance matching a key.
		 *
		 * Returns NULL if no instance matching the key is found.
		 */
		FunctionalUnit*
		getInstance(const ConstKeyPtr& key) const;

		FunctionalUnit*
		getInstance(const CompoundPtr& compound, int direction) const;
		/**
		 * @brief Return the matching key for a given compound/direction.
		 */
		ConstKeyPtr
		getKey(const CompoundPtr& compound, int direction) const;

		/**
		 * @brief Register an instance for a given key.
		 *
		 * Don't keep references to key or functionalUnit, since FlowSeparator is authorized
		 * to delete them.
		 */
		void
		addInstance(const ConstKeyPtr& key, FunctionalUnit* functionalUnit);

		/**
		 * @brief Register an instance for a given key.
		 *
		 * Don't keep references to key or functionalUnit, since FlowSeparator is authorized
		 * to delete them.
		 */
		void
		addInstance(const ConstKeyPtr& key);

		/**
		 * @brief Remove an instance matching a key.
		 *
		 * Removes the FU matching the key and frees all resources (key and FU).
		 */
		void
		removeInstance(const ConstKeyPtr& key);

	private:
            void
            init();

		/**
		 * @name CompoundHandlerInterface
		 *
		 * @brief The CompoundHandlerInterface methods (isAccepting,
		 * doSendData, doOnData, wakeup) are delegated to the matching
		 * instance.
		 *
		 * Instance retrieval is delegated to the internal
		 * implementation _getInstance.  If no matching instance is
		 * found, new instances may get instantiated on the fly.  In
		 * future this may depend on the FlowSeparator configuration.
		 */
		//@{
		virtual void
		doSendData(const CompoundPtr& compound);

		virtual void
		doOnData(const CompoundPtr& compound);

		virtual bool
		doIsAccepting(const CompoundPtr& compound) const;

		virtual void
		doWakeup();
		//@}

		/**
		 * @brief Integrate FU instance.
		 *
		 * Apply bookkeeping and link instance into the FUN.
		 *
		 */
		void
		connectFU(FunctionalUnit* functionalUnit) const;

		/**
		 * @brief Integrate FU instance.
		 *
		 * Apply bookkeeping and link instance into the FUN.
		 *
		 */
		void
		integrate(const ConstKeyPtr& key, FunctionalUnit* functionalUnit);

		/**
		 * @brief Disintegrate FU instance.
		 *
		 * Undo a previous integrateInstance.
		 */
		void
		disintegrate(const ConstKeyPtr& key);


		InstanceMap instances;
            ConnectorReceptacleSeparatorList crsList_;
            DelivererReceptacleSeparatorList drsList_;
            ReceptorReceptacleSeparatorList rrsList_;

            FunctionalUnit* prototypeFU_;
            ConnectorReceptacleSeparator* connectorReceptacleSinglePort_;
            DelivererReceptacleSeparator* delivererReceptacleSinglePort_;
            ReceptorReceptacleSeparator* receptorReceptacleSinglePort_;
		FunctionalUnit* instanceBusy;

		pyconfig::View config;

		std::auto_ptr<KeyBuilder> keyBuilder;

		std::auto_ptr<flowseparator::NotFoundStrategy> notFound;

		wns::distribution::StandardUniform dis;

		logger::Logger logger;
	};

} // namespace ldk
} // namespace wns


#endif // NOT defined WNS_LDK_FLOWSEPARATOR_HPP


