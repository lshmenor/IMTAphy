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
#ifndef WNS_LDK_FCF_PHASEDESCRIPTOR_H
#define WNS_LDK_FCF_PHASEDESCRIPTOR_H

#include <WNS/Cloneable.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/StaticFactory.hpp>

#include <WNS/ldk/fcf/FrameBuilderConfigCreator.hpp>

namespace wns { namespace ldk {

    class FunctionalUnit;

    namespace fcf
    {

        class FrameBuilder;
        class CompoundCollectorInterface;

        /**
         * @brief Duration Policies used by the PhaseDescriptor.
         *
         * @ingroup frameConfigurationFramework
         */
        enum DurationPolicy
            {
                Fixed = 1,
                Expanding = 2
            };

        /**
         * @brief Interface definition of PhaseDescriptor.
         *
         * @ingroup frameConfigurationFramework
         */
        class PhaseDescriptorInterface
        {
        public:
            virtual ~PhaseDescriptorInterface()
            {
            }

            /**
             * @brief Returns the name of this PhaseDescriptor.
             */
            virtual std::string getName() const = 0;

            /**
             * @brief Trigger the PhaseDescriptor to find its friends.
             */
            virtual void onFUNCreated() = 0;

            /**
             * @brief  Do further configuration.
             */
            //virtual void setMaximumDuration() = 0;

            /**
             * @brief Get the FunctionalUnit that handles the compounds of this phase.
             */
            virtual CompoundCollectorInterface*
            getCompoundCollector() const = 0;

        };

        /**
         * @brief Description of frame phases.
         *
         * The PhaseDescriptor is the configuration element of the
         * FrameBuilder. Available configuration settings are
         *  - the name of the phase
         *  - the duration policy (fixed, expanding)
         *
         * @ingroup frameConfigurationFramework
         */
        class PhaseDescriptor
            : public PhaseDescriptorInterface,
              virtual public wns::RefCountable
        {
        public:
            PhaseDescriptor( FrameBuilder* frameBuilder, const wns::pyconfig::View& config );


            std::string getName() const
            {
                return name_;
            }

            void onFUNCreated();

            bool operator==(const PhaseDescriptor& rhs) const
            {
                return name_ == rhs.name_;
            }

            FrameBuilder* getFrameBuilder() const
            {
                assure( frameBuilder_, "frame builder not specified yet" );
                return frameBuilder_;
            }

            CompoundCollectorInterface* getCompoundCollector() const
            {
                assure( compoundCollector_, "CompoundCollector not specified yet");
                return compoundCollector_;
            }

            void setName( const std::string& name )
            {
                name_ = name;
            }

        protected:
            std::string name_;
            FrameBuilder* frameBuilder_;
            wns::pyconfig::View config_;
            CompoundCollectorInterface* compoundCollector_;

            friend class FrameBuilder;
        };

        typedef wns::SmartPtr<PhaseDescriptor> PhaseDescriptorPtr;

        typedef FrameBuilderConfigCreator<PhaseDescriptor> PhaseDescriptorCreator;
        typedef wns::StaticFactory<PhaseDescriptorCreator> PhaseDescriptorFactory;

        /**
         * @brief Basic phase descriptor for generic purposes.
         *
         * Use this PhaseDescriptor to create general purpose phases.
         * Instantiation of the interface is not allowed yet, see StaticFactory.hpp
         *
         * @ingroup frameConfigurationFramework
         */
        class BasicPhaseDescriptor
            : public PhaseDescriptor,
              public wns::Cloneable<BasicPhaseDescriptor>
        {
        public:
            BasicPhaseDescriptor( FrameBuilder* frameBuilder,
                          const wns::pyconfig::View& config ):
                PhaseDescriptor( frameBuilder, config ),
                wns::Cloneable<BasicPhaseDescriptor>()
            {
            }

            wns::CloneableInterface* clone() const
            {
                return wns::Cloneable<BasicPhaseDescriptor>::clone();
            }
        };
}
}
}
#endif

